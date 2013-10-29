#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/12
	created:	12:4:2013   20:21
	file:		State.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	A game state.
				I.e. menu, level 1, level 2, map screen, game over, etc.
				NOTE: I changed from iterators to int it, so that I can add layers
				any time I want without worrying about invalid iterators.
				This way I can add and new layers will only be updated next loop
				automatically.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#include <memory>
#include <vector>

#include "Timer.h"
#include "Layer.h"

namespace game{

	class StateMachine;

	typedef std::vector<shared_Layer_ptr> StateLayers;
	typedef std::vector<LAYERINDEX> LayerIndexes;

	//========================================================================
	// 
	//========================================================================
	class State{

		friend StateMachine;

	public:

		Timer<double> m_timer;

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~State(){}

		//------------------------------------------------------------------------
		// layer stuff
		//------------------------------------------------------------------------
		void AddLayer( shared_Layer_ptr && pNewLayer_p ){

			assert( pNewLayer_p->m_bDead );

			pNewLayer_p->m_bDead = false;

			if( pNewLayer_p->m_currentLayerIndex == INVALID_LAYERINDEX
				||
				pNewLayer_p->m_pStateOwner != this ){

				pNewLayer_p->m_currentLayerIndex = (OBJECTINDEX)m_layers.size();
				pNewLayer_p->m_pStateOwner = this;

				pNewLayer_p->VOnInit();

				m_layers.push_back( std::move(pNewLayer_p) );
			}
			else{

				pNewLayer_p->VOnInit();
			}			
		}
		void AddLayer( const shared_Layer_ptr & pNewLayer_p ){

			assert( pNewLayer_p->m_bDead );

			pNewLayer_p->m_bDead = false;

			if( pNewLayer_p->m_currentLayerIndex == INVALID_LAYERINDEX
				||
				pNewLayer_p->m_pStateOwner != this ){

				pNewLayer_p->m_currentLayerIndex = (OBJECTINDEX)m_layers.size();
				pNewLayer_p->m_pStateOwner = this;

				m_layers.push_back( pNewLayer_p );
			}

			pNewLayer_p->VOnInit();
		}
		void RemoveLayer( LAYERINDEX layerCurrentIndex_p ){

			assert( layerCurrentIndex_p != INVALID_LAYERINDEX );
			assert( !m_layers[layerCurrentIndex_p]->m_bDead );

			m_layers[layerCurrentIndex_p]->m_bDead = true;

			m_removedLayers.push_back( layerCurrentIndex_p );
		}
		void RemoveLayer(  const shared_Layer_ptr & pNewLayer_p  ){

			RemoveLayer( pNewLayer_p->m_currentLayerIndex );
		}
		void RemoveLayer(  const Layer * pNewLayer_p  ){

			RemoveLayer( pNewLayer_p->m_currentLayerIndex );
		}

	private:

		StateLayers m_layers;
		LayerIndexes m_removedLayers;

		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VOnInit(){}
		virtual void VOnUpdate(double, double){}	// called before layers update
		virtual void VLateUpdate(double, double){}	// called after layers update
		virtual void VOnDraw(){}					// called after layers draw
		virtual void VOnDestroy(){}

		virtual void VOnResize(){}

		//------------------------------------------------------------------------
		// updates state timer and call updates with timer time, for each layer
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p ){

			m_timer.Update( dDeltaTime_p );

			VOnUpdate(m_timer.GetTime(), m_timer.GetDelta() );

			// traverse layers and update them, if active

			for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

				if( m_layers[it]->m_bActive ){

					m_layers[it]->Update( m_timer.GetDelta() );
				}
			}

			VLateUpdate( m_timer.GetTime(), m_timer.GetDelta() );

			// clean dead layers

			if( !m_removedLayers.empty() )
				CleanRemovedLayers();

			//
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void OnResize( int W_p, int H_p){

			VOnResize();

			for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

				m_layers[it]->VOnResize( W_p, H_p );
			}
		}

		//------------------------------------------------------------------------
		// calls all layers VOnDestroy than state VOnDestroy
		//------------------------------------------------------------------------
		void Destroy(){

			for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

				m_layers[it]->VOnDestroy();
			}

			VOnDestroy();
		}

		//------------------------------------------------------------------------
		// traverse layer and call draw
		//------------------------------------------------------------------------
		void Draw( const double dInterpolation_p ){

			for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

				if( m_layers[it]->m_bVisible ){

					m_layers[it]->VOnDraw( dInterpolation_p );
				}
			}

			VOnDraw();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void CleanRemovedLayers(){

			unsigned int nRemoved =	(unsigned int)m_removedLayers.size(); // cache
			unsigned int nObjects =		(unsigned int)m_layers.size();

			for( unsigned int itRemoved = 0, itLast = nObjects - 1;
				itRemoved < nRemoved;
				++itRemoved, --itLast ){

					if( !m_layers[m_removedLayers[itRemoved]]->m_bDead ){

						--nRemoved;
						continue; // untested
					}


					m_layers[m_removedLayers[itRemoved]]->VOnDestroy();
					m_layers[m_removedLayers[itRemoved]]->m_currentLayerIndex = INVALID_LAYERINDEX;
					m_layers[m_removedLayers[itRemoved]]->m_pStateOwner = nullptr;

					if( m_removedLayers[itRemoved] == itLast ){

						continue;
					}

					std::swap( m_layers[m_removedLayers[itRemoved]], m_layers[itLast] );

					if( m_layers[m_removedLayers[itRemoved]]->m_bDead ){

						// find the swaped task on the list to be destroyed, update the index

						for( unsigned int itToBeDestroyed = itRemoved; itToBeDestroyed < nRemoved; ++itToBeDestroyed ){

							if( m_removedLayers[itToBeDestroyed] == m_layers[m_removedLayers[itRemoved]]->m_currentLayerIndex ){

								m_removedLayers[itToBeDestroyed] = m_removedLayers[itRemoved];
							}
						}
					}
					else{

						m_layers[m_removedLayers[itRemoved]]->m_currentLayerIndex = m_removedLayers[itRemoved];
					}
			}

			// "trim"

			m_layers.resize(nObjects - nRemoved);

			m_removedLayers.clear();
		}
	};

	typedef std::shared_ptr<State> shared_State_ptr;
	typedef std::weak_ptr<State> weak_State_ptr;
}
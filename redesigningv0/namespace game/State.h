#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/12
	created:	12:4:2013   20:21
	file:		State.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	A game state.
				I.e. menu, level 1, level 2, map screen, game over, etc.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#include <memory>
#include <vector>

#include "Timer.h"
#include "Layer.h"

#define private public
#define protected public

namespace game{

	//class ALayer;
	class StateMachine;
	typedef std::vector<shared_Layer_ptr> StateLayers;
	typedef std::vector<LAYER_STATEINDEX> LayerIndexes;


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
		void AddLayer( const shared_Layer_ptr & pNewLayer_p ){

			pNewLayer_p->VOnInit();

			m_layers.push_back( pNewLayer_p );

			pNewLayer_p->m_currentStateIndex = (LAYER_STATEINDEX)(m_layers.size());
			pNewLayer_p->m_pStateOwner = this;
		}
		void AddLayer( shared_Layer_ptr && pNewLayer_p ){

			pNewLayer_p->m_pStateOwner = this;
			pNewLayer_p->m_currentStateIndex = (LAYER_STATEINDEX)(m_layers.size());
			pNewLayer_p->VOnInit();
			
			m_layers.push_back( std::move(pNewLayer_p) );

			// THIS CODE CALLS LVALUE VERSION!!!
			//shared_Layer_ptr && pNewLayer = std::move(pNewLayer_p);
			//m_layers.push_back( pNewLayer );
		}

		void RemoveLayer( LAYER_STATEINDEX layerCurrentIndex_p ){

			m_layers[layerCurrentIndex_p]->VOnDestroy();
			m_removedLayers.push_back(layerCurrentIndex_p);
		}

	private:

		StateLayers m_layers;
		LayerIndexes m_removedLayers;

		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VOnInit(){}
		virtual void VOnUpdate(double, double){} // called before layers update
		virtual void VOnDraw(){}				 // called after layers draw
		virtual void VOnDestroy(){}

		virtual void VOnResize(){}

		//------------------------------------------------------------------------
		// updates state timer and call updates with timer time, for each layer
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p ){

			m_timer.Update( dDeltaTime_p );

			VOnUpdate(m_timer.GetTime(), m_timer.GetDelta() );

			// traverse layers and update them, if active

			for( StateLayers::const_iterator it = m_layers.cbegin(), itEnd = m_layers.cend();
				it != itEnd; ++ it ){

					if( (*it)->m_bActive ){
					
						(*it)->Update( m_timer.GetDelta() );
					}
			}

			// clean dead layers

			if( !m_removedLayers.empty() )
				CleanRemovedLayers();

			//
		}

		void OnResize( int W_p, int H_p){

			VOnResize();

			for( StateLayers::const_iterator it = m_layers.cbegin(), itEnd = m_layers.cend();
				it != itEnd; ++ it ){

					(*it)->VOnResize( W_p, H_p );
			}

		}
		//------------------------------------------------------------------------
		// traverse layer and call draw
		//------------------------------------------------------------------------
		void Draw( const double dInterpolation_p ){

			for( StateLayers::const_iterator it = m_layers.cbegin(), itEnd = m_layers.cend();
				it != itEnd; ++ it ){

					if( (*it)->m_bActive ){

						(*it)->VOnDraw( dInterpolation_p );
					}
			}

			VOnDraw();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void CleanRemovedLayers(){

			// swap all destroyed layers to the end of the vector, than resizes

			unsigned int nDestroyed = (unsigned int)m_removedLayers.size(); // cache
			unsigned int nLayers = (unsigned int)m_layers.size();

			if( nDestroyed == nLayers ){

				m_layers.clear();
				m_removedLayers.clear();
				return;
			}

			for( unsigned int it = 0, itLast = nLayers - 1; it < nDestroyed; ++it ){

				// check if "to be removed" already at end

				if( m_removedLayers[it] == itLast - it){

					continue;
				}

				std::swap( m_layers[m_removedLayers[it]], m_layers[itLast - it] ); // size - 1, size -2, size -3

				m_layers[m_removedLayers[it]]->m_currentStateIndex = m_removedLayers[it]; // update index
			}

			// "trim"
			m_layers.resize(nLayers - nDestroyed);

			m_removedLayers.clear();
		}
	};

	typedef std::shared_ptr<State> shared_State_ptr;
}
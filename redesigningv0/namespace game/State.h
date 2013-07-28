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

//#define private public
//#define protected public

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

			assert( pNewLayer_p->m_currentStateIndex == INVALID_LAYERINDEX );

			pNewLayer_p->m_pStateOwner = this;
			pNewLayer_p->m_currentStateIndex = (LAYER_STATEINDEX)(m_layers.size());
			pNewLayer_p->VOnInit();

			m_layers.push_back( pNewLayer_p );
		}
		void AddLayer( shared_Layer_ptr && pNewLayer_p ){

			assert( pNewLayer_p->m_currentStateIndex == INVALID_LAYERINDEX );

			pNewLayer_p->m_pStateOwner = this;
			pNewLayer_p->m_currentStateIndex = (LAYER_STATEINDEX)(m_layers.size());
			pNewLayer_p->VOnInit();
			
			m_layers.push_back( std::move(pNewLayer_p) );
		}

		void RemoveLayer( LAYER_STATEINDEX layerCurrentIndex_p ){

			// cant destroy here, since it happens only in the end of the frame, its possible
			// the layer will still update this frame

			m_removedLayers.push_back(layerCurrentIndex_p);
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

				if( m_layers[it]->m_bActive ){

					m_layers[it]->VOnDraw( dInterpolation_p );
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

			for( unsigned int it = 0, itLast = nLayers - 1; it < nDestroyed; ++it ){

				m_layers[m_removedLayers[it]]->VOnDestroy();
				m_layers[m_removedLayers[it]]->m_currentStateIndex = INVALID_LAYERINDEX;
				m_layers[m_removedLayers[it]]->m_pStateOwner = nullptr;

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

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		//void AddAddedLayers(){

		//	//for( )
		//}
	};

	typedef std::shared_ptr<State> shared_State_ptr;
}
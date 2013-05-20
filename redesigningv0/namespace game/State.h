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

	class State{

		friend StateMachine;

	public:
		Timer<double> m_timer;

	private:
		StateLayers m_layers;


		//------------------------------------------------------------------------
		// updates state timer and call updates with timer time, for each layer
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p ){

			m_timer.Update( dDeltaTime_p );

			// traverse layers and update them, if active

			for( StateLayers::const_iterator it = m_layers.cbegin(), itEnd = m_layers.cend();
				it != itEnd; ++ it ){

					if( (*it)->m_bActive ){
					
						(*it)->Update( m_timer.GetDelta() );
					}
			}
		}

		//------------------------------------------------------------------------
		// traverse layer and call draw
		//------------------------------------------------------------------------
		void Draw( const double dInterpolation_p ){

			for( StateLayers::const_iterator it = m_layers.cbegin(), itEnd = m_layers.cend();
				it != itEnd; ++ it ){

					if( (*it)->m_bActive ){

						(*it)->VDraw( dInterpolation_p );
					}
			}
		}


	public:

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~State(){}

		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VInit(){}
		virtual void VDestroy(){}


		//------------------------------------------------------------------------
		// layer stuff
		//------------------------------------------------------------------------
		void AddLayer( const shared_Layer_ptr & pNewLayer_p ){

			pNewLayer_p->VInit();

			m_layers.push_back( pNewLayer_p );
			
			pNewLayer_p->m_currentStateIndex = (LAYER_STATEINDEX)(m_layers.size()-1);
		}
		void AddLayer( shared_Layer_ptr && pNewLayer_p ){

			pNewLayer_p->VInit();

			m_layers.push_back( pNewLayer_p );

			pNewLayer_p->m_currentStateIndex = (LAYER_STATEINDEX)(m_layers.size()-1);
		}
		void RemoveLayer( LAYER_STATEINDEX layerCurrentIndex_p ){

			m_layers[layerCurrentIndex_p]->VDestroy();

			std::swap( m_layers[layerCurrentIndex_p], m_layers[m_layers.size()-1] );
			m_layers[layerCurrentIndex_p]->m_currentStateIndex = layerCurrentIndex_p; // update index
			m_layers.pop_back();
		}

	};

	typedef std::shared_ptr<State> shared_State_ptr;
}
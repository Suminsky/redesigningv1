//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/12
	created:	12:4:2013   20:46
	file:		StateMachine.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	Control the game states, its updates and transitions.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

// private headers

#include "State.h"

namespace game{

	class StateMachine{

		shared_State_ptr m_pCurrentState;
		shared_State_ptr m_pToBeChangedTo_Tmp;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		StateMachine():m_pCurrentState(nullptr), m_pToBeChangedTo_Tmp(nullptr){}
		virtual ~StateMachine(){}

		//------------------------------------------------------------------------
		// Updates current state
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p ){

			if( m_pCurrentState ){

				m_pCurrentState->Update( dDeltaTime_p );
			}
		}

		//------------------------------------------------------------------------
		// Draws current state
		//------------------------------------------------------------------------
		void Draw( const double dInterpolation_p ){

			if( m_pCurrentState ){

				m_pCurrentState->Draw( dInterpolation_p );
			}
		}

		//------------------------------------------------------------------------
		// set state to be changed to
		//------------------------------------------------------------------------
		void ChangeState( const shared_State_ptr & pNewState_p ){

			// TODO: warning if already set to change

			m_pToBeChangedTo_Tmp = pNewState_p;
		}
		void ChangeState( shared_State_ptr && pNewState_p ){

			m_pToBeChangedTo_Tmp = std::move(pNewState_p);
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void OnResize( int W_p, int H_p){

			if( m_pCurrentState ){

				m_pCurrentState->OnResize( W_p, H_p );
			}
		}

		//------------------------------------------------------------------------
		// Changes current state, destroying current and initializing the new properly.
		//------------------------------------------------------------------------
		void ResolveStateChange(){

			if( !m_pToBeChangedTo_Tmp ) return;

			// destroy current state, if any

			if( m_pCurrentState ){

				m_pCurrentState->Destroy();
			}

			// assign new state

			m_pCurrentState = std::move( m_pToBeChangedTo_Tmp );
			m_pToBeChangedTo_Tmp.reset();

			// init new state, if any

			if( m_pCurrentState ){

				m_pCurrentState->VOnInit(); // TODO: give old state to new state
			}
		}
	};
}
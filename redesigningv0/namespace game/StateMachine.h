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

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		StateMachine():m_pCurrentState(nullptr){}
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
		// Changes current state, destroying current and initializing the new properly.
		//------------------------------------------------------------------------
		void ChangeState( shared_State_ptr pNewState_p ){

			// destroy current state, if any

			if( m_pCurrentState ){

				m_pCurrentState->VDestroy();
			}

			// assign new state

			m_pCurrentState = pNewState_p;

			// init new state, if any

			if( m_pCurrentState ){

				m_pCurrentState->VInit(); // TODO: give old state to new state
			}
		}
	};
}
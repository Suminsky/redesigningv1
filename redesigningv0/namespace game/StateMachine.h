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

#include <memory>

namespace game{

	class State;
	typedef std::shared_ptr<State> shared_State_ptr;

	class StateMachine{

		shared_State_ptr m_pCurrentState;
		shared_State_ptr m_pToBeChangedTo_Tmp;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		StateMachine();
		virtual ~StateMachine();

		//------------------------------------------------------------------------
		// Updates current state
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p );

		//------------------------------------------------------------------------
		// Draws current state
		//------------------------------------------------------------------------
		void Draw( const double dInterpolation_p );

		//------------------------------------------------------------------------
		// set state to be changed to
		//------------------------------------------------------------------------
		void ChangeState( const shared_State_ptr & pNewState_p );
		void ChangeState( shared_State_ptr && pNewState_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void OnResize( int W_p, int H_p);

		//------------------------------------------------------------------------
		// Changes current state, destroying current and initializing the new properly.
		//------------------------------------------------------------------------
		void ResolveStateChange();


		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Destroy();
	};
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/12
	created:	12:4:2013   20:18
	file:		Game.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

// private headers

#include "StateMachine.h"
#include "../namespace win/HighPerformanceTimer.h"

namespace game{

	class Game{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Game(): m_dFixedTimeStep(1.0/30.0), m_dMaxFrameTimeDelay(1.0/3.0){}
		virtual ~Game(){}

		//------------------------------------------------------------------------
		// initializes stuff
		//------------------------------------------------------------------------
		void Init(){

			m_timer.Init();
		}

		//------------------------------------------------------------------------
		// allows for graceful destruction. Calling Destroy on the layers,
		// than on the state, in order.
		//------------------------------------------------------------------------
		void Destroy(){

			m_stateControl.Destroy();
		}

		//------------------------------------------------------------------------
		// The main loop
		// the loop calls updates passing m_dFixedStep as time, accumulating or
		// skipping, and then calls draw passing the interpolation amount
		//------------------------------------------------------------------------
		void FixedStepLoop();

	protected:

		StateMachine				m_stateControl;
		win::HighPerformanceTimer	m_timer;
		double						m_dFixedTimeStep, m_dMaxFrameTimeDelay;
	};
}
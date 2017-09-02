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
		Game( double dFixedTimeStep = 1.0/30.0, double dMaxFrameTimeDelay = 1.0/3.0 )
			:
		m_dFixedTimeStep(dFixedTimeStep),
		m_dMaxFrameTimeDelay(dMaxFrameTimeDelay){}
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
		void SmoothedFixedStepLoop( double dVsyncRefreshRate_p );
		void LooseStepLoop();

		void SmoothDelta( double & dDelta_p, double dVsyncRefreshRate_p );

	protected:

		StateMachine				m_stateControl;
		win::HighPerformanceTimer	m_timer;
		double						m_dFixedTimeStep, m_dMaxFrameTimeDelay;

	private:

		double m_dFrameDeltaRemainingsAccumulated = 0.0;
		double m_dSmoothDeltaBuffer = 0;
	};
}
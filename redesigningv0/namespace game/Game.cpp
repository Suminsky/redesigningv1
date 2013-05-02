#include "Game.h"

using namespace game;

void Game::FixedStepLoop(){

	// update timer
	m_timer.Update();

	// get time since last timer update
	double && dDeltaSeconds = m_timer.GetDeltaSeconds();
	double && dSeconds = m_timer.GetSecondsAccum();

	// cap to max frame delay allowed, this is to avoid spiral of death

	if( dDeltaSeconds > m_dMaxFrameTimeDelay ) dDeltaSeconds = m_dMaxFrameTimeDelay;

	// add the delta to the delta remained from previous updates (deltas not used)

	static double dFrameDeltaRemainingsAccumulated = 0.0;
	dFrameDeltaRemainingsAccumulated += dDeltaSeconds;

	// perform updates by fixed steps, any time remaining will accumulate to the next iteration
	// any time missing will skip the update till a fix step amount is reached

	while( dFrameDeltaRemainingsAccumulated >= m_dFixedTimeStep ){

		// consume the fixed amount from the accumulated buffer
		dFrameDeltaRemainingsAccumulated -= m_dFixedTimeStep;

		m_stateControl.Update( dSeconds, m_dFixedTimeStep );
	}

	// compute the interpolation factor based on the remaining timing
	// the interpolation is used to fix rendering motion across the discrete update steps

	const double dInterpolationAmount = dFrameDeltaRemainingsAccumulated/m_dFixedTimeStep;

	// draw

	m_stateControl.Draw( dInterpolationAmount );
}
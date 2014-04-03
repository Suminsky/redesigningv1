#include "Game.h"
#include "../namespace win/FileLogger.h"

using namespace game;

void Game::FixedStepLoop(){

	// update timer
	m_timer.Update();

	// get time since last timer update
	double dDeltaSeconds = m_timer.GetDeltaSeconds();

	win::UniqueFileLogger()<<"O DELTA: "<<dDeltaSeconds<<SZ_NEWLINE;

	// cap to max frame delay allowed, this is to avoid spiral of death
	if( dDeltaSeconds > m_dMaxFrameTimeDelay )
		dDeltaSeconds = m_dMaxFrameTimeDelay;

	// add the delta to the delta remained from previous updates (deltas not used)
	static double dFrameDeltaRemainingsAccumulated = 0.0;
	dFrameDeltaRemainingsAccumulated += dDeltaSeconds;

	// perform updates by fixed steps, any time remaining will accumulate to the next iteration
	// any time missing will skip the update till a fix step amount is reached
	int count = 0;
	while( dFrameDeltaRemainingsAccumulated >= m_dFixedTimeStep ){

		++count;

		// consume the fixed amount from the accumulated buffer
		dFrameDeltaRemainingsAccumulated -= m_dFixedTimeStep;

		m_stateControl.Update( m_dFixedTimeStep );

		win::UniqueFileLogger()<<"*FIXED UPDATE*"<<SZ_NEWLINE;
		win::UniqueFileLogger()<<"FIX: "<<m_dFixedTimeStep<<SZ_NEWLINE;
	}

	if( count > 1 ){
		win::UniqueFileLogger()<<"COUNT"<<SZ_NEWLINE;
	}

	// compute the interpolation factor based on the remaining timing
	// the interpolation is used to fix rendering motion across the discrete update steps
	double dInterpolationAmount = dFrameDeltaRemainingsAccumulated/m_dFixedTimeStep;

	// draw

	if( dInterpolationAmount > 1.0 )
		dInterpolationAmount = 1.0;

	m_stateControl.Draw( dInterpolationAmount, dDeltaSeconds );

	win::UniqueFileLogger()<<"*DRAW*"<<SZ_NEWLINE;
	win::UniqueFileLogger()<<"i:  "<<dInterpolationAmount<<SZ_NEWLINE<<SZ_NEWLINE;

	// scream?

	m_stateControl.ResolveStateChange();
}

void game::Game::LooseStepLoop()
{
	// update timer
	m_timer.Update();

	// get time since last timer update
	double dDeltaSeconds = m_timer.GetDeltaSeconds();

	m_stateControl.Update( dDeltaSeconds );

	m_stateControl.Draw( 1.0, dDeltaSeconds );

	m_stateControl.ResolveStateChange();
}

void game::Game::SmoothedFixedStepLoop( double dVsyncRefreshRate_p )
{
	//m_dFixedTimeStep = 1.0 / dVsyncRefreshRate_p;

	// update timer
	m_timer.Update();

	// get time since last timer update
	double dDeltaSeconds = m_timer.GetDeltaSeconds();

	win::UniqueFileLogger()<<"O DELTA: "<<dDeltaSeconds<<SZ_NEWLINE;

	SmoothDelta( dDeltaSeconds, dVsyncRefreshRate_p );

	// cap to max frame delay allowed, this is to avoid spiral of death
	if( dDeltaSeconds > m_dMaxFrameTimeDelay )
		dDeltaSeconds = m_dMaxFrameTimeDelay;

	win::UniqueFileLogger()<<"S DELTA: "<<dDeltaSeconds<<SZ_NEWLINE;

	// add the delta to the delta remained from previous updates (deltas not used)
	static double dFrameDeltaRemainingsAccumulated = 0.0;
	dFrameDeltaRemainingsAccumulated += dDeltaSeconds;

	// perform updates by fixed steps, any time remaining will accumulate to the next iteration
	// any time missing will skip the update till a fix step amount is reached

	int count = 0;
	while( dFrameDeltaRemainingsAccumulated >= m_dFixedTimeStep ){

		++count;

		// consume the fixed amount from the accumulated buffer
		dFrameDeltaRemainingsAccumulated -= m_dFixedTimeStep;

		m_stateControl.Update( m_dFixedTimeStep );

		win::UniqueFileLogger()<<"*FIXED UPDATE*"<<SZ_NEWLINE;
		win::UniqueFileLogger()<<"FIX: "<<m_dFixedTimeStep<<SZ_NEWLINE;
	}

	if( count > 1 ){
		win::UniqueFileLogger()<<"COUNT"<<SZ_NEWLINE;
	}

	// compute the interpolation factor based on the remaining timing
	// the interpolation is used to fix rendering motion across the discrete update steps
	double dInterpolationAmount = dFrameDeltaRemainingsAccumulated/m_dFixedTimeStep;

	// draw

	if( dInterpolationAmount > 1.0 )
		dInterpolationAmount = 1.0;

	m_stateControl.Draw( dInterpolationAmount, dDeltaSeconds );

	// scream?

	win::UniqueFileLogger()<<"*DRAW*"<<SZ_NEWLINE;
	win::UniqueFileLogger()<<"i:  "<<dInterpolationAmount<<SZ_NEWLINE<<SZ_NEWLINE;

	m_stateControl.ResolveStateChange();
}

void game::Game::SmoothDelta( double & dDelta_p, double dVsyncRefreshRate_p )
{
	// this buffer keeps track of the extra bits of time
	static double dDeltaBuffer = 0;

	// add in whatever time we currently have saved in the buffer
	dDelta_p += dDeltaBuffer;

	// calculate how many frames will have passed on the next vsync
	int frameCount = (int)(dDelta_p * dVsyncRefreshRate_p + 1);
	//double frameCount = (dDelta_p * dVsyncRefreshRate_p + 0.5);

	// if less then a full frame, increase delta to cover the extra
	if( frameCount <= 0 )	frameCount = 1;

	// save off the delta, we will need it later to update the buffer
	const double dOldDelta = dDelta_p;

	// recalculate delta to be an even frame rate multiple
	dDelta_p = frameCount / dVsyncRefreshRate_p;

	// update delta buffer so we keep the same time on average
	dDeltaBuffer = dOldDelta - dDelta_p;
}
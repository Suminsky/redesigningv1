//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2012/08/08
	created:	8:8:2012   0:13
	filename: 	c:\Users\Gateway\Documents\Visual Studio 2008\Projects\TestandoGFrameOnly\TestandoGFrameOnly\HighPerformanceTimer.h
	file path:	c:\Users\Gateway\Documents\Visual Studio 2008\Projects\TestandoGFrameOnly\TestandoGFrameOnly
	file base:	HighPerformanceTimer
	file ext:	h
	author:		Giuliano Suminsky (a.k.a. Icebone1000)
	
	purpose:	This is the other half of the timerlogger class, the timer part

	© Giuliano Suminsky (a.k.a. Icebone1000) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

#include <windows.h>

namespace win{

	class HighPerformanceTimer{

		LARGE_INTEGER	m_counterFrequency;
		DOUBLE			m_counterFrequency_inverse;	// cached precomputed value

		//time per frame data:-----------------------
		LARGE_INTEGER m_nowCount, m_lastCount;

		LONGLONG m_nDeltaCounts, m_nCounts_persistent;	// persistent == since the first call
		LONGLONG m_nFrameCount;						// obviously persistent, also works like a frame ID
		//-------------------------------------------

		//========================================================================
		// time interval(paired functions) data
		//========================================================================
		struct Cronometer{

			LARGE_INTEGER	m_startCount_cache;
			DOUBLE		&	m_counterFrequency_inverse;

			Cronometer( DOUBLE & counterFrequency_inverse_p )
				: m_counterFrequency_inverse(counterFrequency_inverse_p), m_startCount_cache(){}

			//----------------------------------------------
			// paired functions, depends of StartCount, they gatter duration(in counts) between their calls
			//----------------------------------------------
			VOID StartCount();						
			VOID EndCountSeconds( DOUBLE &sec );	
			VOID EndCountMilliseconds( DOUBLE &msec );
			VOID EndCountFrameCount( LONGLONG &nCount );
			//----------------------------------------------

		private:

			Cronometer & operator =( const Cronometer & other_p );

		};

		//========================================================================
		// frame/s data
		// temp data cleared at each "interval"(aka PER INTERVAL data)
		// NOTE: per INTERVAL!!-> specified by user
		//========================================================================
		struct FPI{
			
			DOUBLE m_timeInterval_ms;					//its a per frame checker
			DOUBLE m_nTimeElapsed_accum_ms_tmp;
			
			FPI( const DOUBLE interval_p):m_timeInterval_ms(interval_p), m_nTimeElapsed_accum_ms_tmp(0.0){}

			void Reset( const DOUBLE interval_p ){

				m_timeInterval_ms = interval_p;
				m_nTimeElapsed_accum_ms_tmp = 0.0;
			}
			BOOL FpI( const DOUBLE &deltaMS );//frames per interval(delta inputed)

		};

	public:

		Cronometer m_cronometer;
		FPI m_fpi;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		HighPerformanceTimer( DOUBLE interval_ms_p = 500.0f);

		//------------------------------------------------------------------------------------------------------
		// per frame
		//------------------------------------------------------------------------------------------------------
		VOID Init();	//function called once to get initial start count
		VOID Update();	//function called per frame, calculates frame duration
		//----------------------
		DOUBLE GetDeltaSeconds()const;		//get frame duration on seconds(based on frame interval)
		DOUBLE GetDeltaMilliseconds()const;	//get frame duration on milliseconds(based on frame interval)
		//-----------------------
		DOUBLE GetSecondsAccum()const;		//get how much seconds ellapsed since Init
		DOUBLE GetMillisecondsAccum()const;	//get how much milliseconds ellapsed since Init
		LONGLONG GetFrameCountAccum()const;	//get frame count accumulated since begining
		LONGLONG GetCountAccum() const;
		//--------------------------------------------------------
		VOID GetDeltaSeconds( DOUBLE &sec )const;
		VOID GetDeltaMilliseconds( DOUBLE &msec )const;
		//-----------------------
		VOID GetSecondsAccum( DOUBLE &secAccum )const;
		VOID GetMillisecondsAccum( DOUBLE &msecAccum )const;
		VOID GetFrameCountAccum( LONGLONG &nCountAccum )const;
		//------------------------------------------------------------------------------------------------------

		//------------------------------------------------------------------------
		// call i after Update for more accurate interval measure
		//------------------------------------------------------------------------
		BOOL FPI();

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~HighPerformanceTimer(void){};
	};


	static HighPerformanceTimer & UniqueHigPerfTimer(){

		static HighPerformanceTimer timer;
		return timer;
	}
}
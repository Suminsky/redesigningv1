#include "HighPerformanceTimer.h"

#include <assert.h>

using namespace win;


HighPerformanceTimer::HighPerformanceTimer( DOUBLE interval_ms_p /*= 500.0f*/ ):m_cronometer(m_counterFrequency_inverse), m_fpi(interval_ms_p){

	QueryPerformanceFrequency( &m_counterFrequency );
	//DOUBLE test = (double)m_counterFrequency.QuadPart; seems like no info is lost in that conversion
	//test = test;
	m_counterFrequency_inverse = 1.0 / (DOUBLE)m_counterFrequency.QuadPart;

	m_nFrameCount = 0LL;
	m_nCounts_persistent = 0LL;
}
//=====================================================================================================================
//paired:
//=======================================
VOID HighPerformanceTimer::Cronometer::StartCount(){

	QueryPerformanceCounter( &m_startCount_cache );
}
//=======================================
VOID HighPerformanceTimer::Cronometer::EndCountSeconds( DOUBLE &sec ){

	assert( m_startCount_cache.QuadPart );//fail if m_startCount_cache not called

	LARGE_INTEGER endCount;
	QueryPerformanceCounter( &endCount );

	//get how many counts:
	LONGLONG nCounts_sinceStartCount  = endCount.QuadPart - m_startCount_cache.QuadPart;

	//calculate seconds:
	sec = nCounts_sinceStartCount * m_counterFrequency_inverse;

	//reset m_startCount_cache
	//m_startCount_cache.QuadPart = 0L;
}
VOID HighPerformanceTimer::Cronometer::EndCountMilliseconds( DOUBLE &msec ){

	assert( m_startCount_cache.QuadPart );

	LARGE_INTEGER endCount;
	QueryPerformanceCounter( &endCount );//fail if m_startCount_cache not called

	//get how many counts:
	LONGLONG nCounts_sinceStartCount  = endCount.QuadPart - m_startCount_cache.QuadPart;

	//calculate seconds:
	msec = (nCounts_sinceStartCount * m_counterFrequency_inverse)*1000.0;

	//reset m_startCount_cache
	//m_startCount_cache.QuadPart = 0LL;
}
VOID HighPerformanceTimer::Cronometer::EndCountFrameCount(LONGLONG &nCount){

	assert( m_startCount_cache.QuadPart );

	LARGE_INTEGER endCount;
	QueryPerformanceCounter( &endCount );//fail if m_startCount_cache not called

	//get how many counts:(here its also the return value)

	nCount = endCount.QuadPart - m_startCount_cache.QuadPart;

	//reset m_startCount_cache
	//m_startCount_cache.QuadPart = 0L;
}
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//per frame:
//=======================================
VOID HighPerformanceTimer::Init(){

	QueryPerformanceCounter( &m_nowCount );
}
//=======================================
VOID HighPerformanceTimer::Update(){

	m_lastCount = m_nowCount;

	QueryPerformanceCounter( &m_nowCount );

	// gatter how many counts since last call(or since init frame)

	m_nDeltaCounts = m_nowCount.QuadPart - m_lastCount.QuadPart;

	// gatter how many counts since begining:

	m_nCounts_persistent += m_nDeltaCounts;

	// increment framecount:

	++m_nFrameCount;//never cleared(like frame IDs)
}
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//per interval:
//=======================================
BOOL HighPerformanceTimer::FPI(){

	return m_fpi.FpI(GetDeltaMilliseconds());
}
//=======================================
BOOL HighPerformanceTimer::FPI::FpI( const DOUBLE &deltaMS ){

	m_nTimeElapsed_accum_ms_tmp += deltaMS;

	//check if interval is over:
	if( m_nTimeElapsed_accum_ms_tmp >= m_timeInterval_ms ){

		m_nTimeElapsed_accum_ms_tmp = 0.0;//-= m_timeInterval_ms;
		return TRUE;
	}
	return FALSE;
}
//=======================================
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//gatters:
//=======================================
DOUBLE HighPerformanceTimer::GetDeltaSeconds()const{

	return m_nDeltaCounts * m_counterFrequency_inverse;
}
VOID HighPerformanceTimer::GetDeltaSeconds( DOUBLE &sec )const{

	sec = m_nDeltaCounts * m_counterFrequency_inverse;
}
//
DOUBLE HighPerformanceTimer::GetDeltaMilliseconds()const{

	return (m_nDeltaCounts * m_counterFrequency_inverse)*1000.0;
}
VOID HighPerformanceTimer::GetDeltaMilliseconds( DOUBLE &msec )const{

	msec = (m_nDeltaCounts * m_counterFrequency_inverse)*1000.0;
}
//=======================================
//accum gatters:
//=======================================
DOUBLE HighPerformanceTimer::GetSecondsAccum()const{

	return m_nCounts_persistent * m_counterFrequency_inverse;
}
VOID HighPerformanceTimer::GetSecondsAccum( DOUBLE &secAccum )const{

	secAccum = m_nCounts_persistent * m_counterFrequency_inverse;
}
//
DOUBLE HighPerformanceTimer::GetMillisecondsAccum()const{

	return (m_nCounts_persistent * m_counterFrequency_inverse)*1000.0;
}
VOID HighPerformanceTimer::GetMillisecondsAccum( DOUBLE &msecAccum )const{

	msecAccum = (m_nCounts_persistent * m_counterFrequency_inverse)*1000.0;
}
//
LONGLONG HighPerformanceTimer::GetFrameCountAccum()const{

	return m_nFrameCount;
}
VOID HighPerformanceTimer::GetFrameCountAccum(LONGLONG &nCountAccum)const{

	nCountAccum = m_nFrameCount;
}
//
LONGLONG HighPerformanceTimer::GetCountAccum()const{
	return m_nCounts_persistent;
}
//=====================================================================================================================

namespace win{
	HighPerformanceTimer & UniqueHigPerfTimer(){

		static HighPerformanceTimer timer;
		return timer;
	}
}
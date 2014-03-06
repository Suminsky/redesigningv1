#include "FadeTask.h"

using namespace game;

FadeTask::FadeTask( float & fValueToFade_p,
					float fTimeToComplete_p /*= 1.0f*/,
					float from /*= 1.0f*/, float to /*= 0.0f*/,
					shared_Task_ptr pChainedTask_p /*= nullptr */ )
	: Task(pChainedTask_p),
	m_fTimeToComplete(fTimeToComplete_p),
	m_from(from),m_to(to),
	m_pfFadingReference(&fValueToFade_p),
	m_elapsed(0.0f)
{

}

void game::FadeTask::VOnUpdate( double, double delta )
{
	m_elapsed += delta;

	*m_pfFadingReference = gen::Tweening<>::Linear( (float)m_elapsed, m_fTimeToComplete, m_from, m_to );

	if( m_elapsed >=  m_fTimeToComplete ){

		Chain();
	}

}

void game::FadeTask::Set( float fTimeToComplete_p /*= 1.0f*/, float from /*= 1.0f*/, float to /*= 0.0f */ )
{
	m_elapsed = 0.0;
	m_fTimeToComplete = fTimeToComplete_p;
	*m_pfFadingReference = m_from = from;
	m_to = to;
}
void game::FadeTask::Set( float * fValueToFade_p, float fTimeToComplete_p /*= 1.0f*/, float from /*= 1.0f*/, float to /*= 0.0f */ )
{
	m_elapsed = 0.0;
	m_fTimeToComplete = fTimeToComplete_p;
	m_pfFadingReference = fValueToFade_p;
	*m_pfFadingReference = m_from = from;
	m_to = to;
}

void game::FadeTask::Set( const shared_Task_ptr & pChainedTask_p, float & fValueToFade_p, float fTimeToComplete_p /*= 1.0f*/, float from /*= 1.0f*/, float to /*= 0.0f */ )
{
	m_pChainedTask = pChainedTask_p;
	m_elapsed = 0.0;
	m_fTimeToComplete = fTimeToComplete_p;
	m_pfFadingReference = &fValueToFade_p;
	*m_pfFadingReference = m_from = from;
	m_to = to;
}
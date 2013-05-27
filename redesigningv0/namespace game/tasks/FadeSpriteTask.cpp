#include "FadeSpriteTask.h"

#include "../components/SpriteComponent.h"

using namespace game;

FadeSpriteTask::FadeSpriteTask( const shared_SpriteComponent_ptr & pSpriteComp_p,
								float fTimeToComplete_p /*= 1.0f*/,
								float from /*= 1.0f*/, float to /*= 0.0f*/,
								shared_Task_ptr pChainedTask_p /*= nullptr */ )
	: Task(pChainedTask_p),
	m_fTimeToComplete(fTimeToComplete_p),
	m_from(from),m_to(to),
	m_alphaReference(pSpriteComp_p->m_renderData.m_color.w),
	m_elapsed(0.0f)
{

}

void game::FadeSpriteTask::VOnUpdate( double, double delta )
{
	m_elapsed += delta;

	float alpha = gen::Tweening<>::Linear( (float)m_elapsed, m_fTimeToComplete, m_from, m_to );

	if( m_elapsed >=  m_fTimeToComplete ){

		m_alphaReference = m_to;

		Chain();
	}
	else{

		m_alphaReference = alpha;
	}
}

void game::FadeSpriteTask::Set( float fTimeToComplete_p /*= 1.0f*/, float from /*= 1.0f*/, float to /*= 0.0f */ )
{
	m_elapsed = 0.0;
	m_fTimeToComplete = fTimeToComplete_p;
	m_alphaReference = m_from = from;
	m_to = to;
}

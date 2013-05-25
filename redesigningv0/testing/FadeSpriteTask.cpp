#include "FadeSpriteTask.h"
#include "TestState.h"

FadeSpriteTask::FadeSpriteTask( float fTimeToComplete_p, const shared_SpriteComp_ptr & pSpriteComp_p, game::shared_Task_ptr pChainedTask_p /*= nullptr */ )
	: game::Task(pChainedTask_p),
	m_fTimeToComplete(fTimeToComplete_p),
	m_alphaReference(pSpriteComp_p->m_renderData.m_color.w)
{

}
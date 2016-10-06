#include "PCHF.h"
#include "SpriteAnimationSystem.h"
#include "../Layer.h"

using namespace game;

void game::SpriteAnimationSystem::VOnInit()
{
	shared_SpriteAnimCompo_Factory_ptr pFactory = std::static_pointer_cast<SpriteAnimCompo_Factory>(
		m_pSysMachineOwner->GetLayer()->m_componentFactory.GetComponentFactory<SpriteAnimCompo_>()
		);

	m_poolAccess = pFactory->m_pool.GetAccess();
}

void game::SpriteAnimationSystem::VOnUpdate( double /*dAccum_p*/, double dDelta_p )
{
	for( int itSprite = 0, iSize = m_poolAccess.GetNAllocated();
		itSprite < iSize;
		++itSprite ){

			SpriteAnimCompo_ & animCompo = (*m_poolAccess.GetAllocated( itSprite ));
			if( !animCompo.GetObjectOwner()->IsAttached() ) continue;

			if( animCompo.m_pClips[ animCompo.m_iCurrentClip ].state.eState == E_ANIMSTATE_PLAYING ){

				 animCompo.Update( (animTimeUnit)dDelta_p );
			}

			if( animCompo.m_iPreviousFrame != animCompo.m_iCurrentFrame ){

				animCompo.m_iPreviousFrame = animCompo.m_iCurrentFrame;

				animCompo.GetObjectOwner()->DispatchComponentEventImmediately( COMPONENT_TYPE(SpriteAnimCompo_), &animCompo );
			}
	}
}
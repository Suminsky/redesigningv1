#include "SpriteAnimationSystem.h"
#include "../Layer.h"

using namespace game;

void game::SpriteAnimationSystem::VOnInit()
{
	shared_SpriteAnimationComponentFactory_ptr pFactory = std::static_pointer_cast<SpriteAnimationComponentFactory>(
		m_pSysMachineOwner->GetLayer()->m_componentFactories.GetComponentFactory<SpriteAnimationComponent>()
		);

	m_poolAccess = pFactory->m_pool.GetAccess();
}

void game::SpriteAnimationSystem::VOnUpdate( double /*dAccum_p*/, double dDelta_p )
{
	for( int itSprite = 0, iSize = m_poolAccess.GetNAllocated();
		itSprite < iSize;
		++itSprite ){

			SpriteAnimationComponent & animCompo = (*m_poolAccess.GetAllocated( itSprite ));

			if( animCompo.m_vClips[ animCompo.m_iCurrentClip ].stateData.eState == E_ANIMSTATE_PLAYING ){

				 animCompo.Update( dDelta_p );
			}

			if( animCompo.m_previousFrame != animCompo.m_currentFrame ){

				if( animCompo.IsAttached() )
					animCompo.GetObjectOwner()->DispatchComponentEventImmediately( COMPONENT_TYPE(SpriteAnimationComponent), &animCompo );
			}
	}
}
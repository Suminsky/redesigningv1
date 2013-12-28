#include "SpriteSystem.h"
#include "../Layer.h"

using namespace game;

void game::SpriteSystem::VOnInit()
{
	shared_SpriteComponent_Factory_ptr pTrafoFactory = std::static_pointer_cast<SpriteComponent_Factory>(
		m_pSysMachineOwner->GetLayer()->m_componentFactory.GetComponentFactory<SpriteComponent_>()
		);

	m_poolAccess = pTrafoFactory->m_pool.GetAccess();
}

void game::SpriteSystem::VOnDraw( double dInterpolation_p )
{
	for( int itSprite = 0, iSize = m_poolAccess.GetNAllocated();
		itSprite < iSize;
		++itSprite ){

			SpriteComponent_ & spriteCompo = (*m_poolAccess.GetAllocated( itSprite ));
			spriteCompo.OnDraw( dInterpolation_p );

			m_pSpriteRendererRef->Render( &spriteCompo, m_pCameraRef );
	}
}

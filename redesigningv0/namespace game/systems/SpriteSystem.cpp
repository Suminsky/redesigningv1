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

			// what if object isnt attached to layer..notting different will happen at all

			SpriteComponent_ & spriteCompo = (*m_poolAccess.GetAllocated( itSprite ));
			if( !spriteCompo.GetObjectOwner()->IsAttached() ) continue;

			spriteCompo.OnDraw( dInterpolation_p );

			// cull invisible sprites
			if( spriteCompo.m_renderData.m_color.w > 0.0f )
			// cull out of view sprites
			//if( m_pCameraRef->)

			m_pSpriteRendererRef->Render( &spriteCompo, m_pCameraRef );
	}
}

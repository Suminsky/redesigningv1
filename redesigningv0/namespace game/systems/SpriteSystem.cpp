#include "SpriteSystem.h"
#include "../Layer.h"
#include "../../namespace phys/AABB2D.h"

#include "../../namespace win/FileLogger.h"

using namespace game;
using namespace DirectX;

void game::SpriteSystem::VOnInit()
{
	shared_SpriteComponent_Factory_ptr pTrafoFactory = std::static_pointer_cast<SpriteComponent_Factory>(
		m_pSysMachineOwner->GetLayer()->m_componentFactory.GetComponentFactory<SpriteComponent_>()
		);

	m_poolAccess = pTrafoFactory->m_pool.GetAccess();
}

void game::SpriteSystem::VOnDraw( double dInterpolation_p )
{
	// TODO interpolate camera

	// create cam box

	float zoom = m_pCameraRef->m_fZoom * 0.5f; // halving on zoom (order of prod dont aff res)
	float camHalfW = m_pCameraRef->m_viewPort.Width * zoom;
	float camHalfH = m_pCameraRef->m_viewPort.Height * zoom;
	XMFLOAT2 camPos(
		-XMVectorGetX( m_pCameraRef->m_mView.r[3] ),
		-XMVectorGetY( m_pCameraRef->m_mView.r[3] )
		);

	phys::AABB2D camBox = { camPos, camHalfW, camHalfH };

	
	//int iCulled = 0;
	for( int itSprite = 0, iSize = m_poolAccess.GetNAllocated();
		itSprite < iSize;
		++itSprite ){


			SpriteComponent_ & spriteCompo = (*m_poolAccess.GetAllocated( itSprite ));
			if( !spriteCompo.GetObjectOwner()->IsAttached() ) continue;

			spriteCompo.OnDraw( dInterpolation_p );

			
			if( spriteCompo.m_renderData.m_color.w > 0.0f ){ // cull invisible sprites

				// cull out of view sprites (needs zoom info)
				
				// create sprite box

				float halfW = spriteCompo.m_renderData.m_res.x * 0.5f;
				float halfH = spriteCompo.m_renderData.m_res.y * 0.5f;
				XMFLOAT2 pos( 
					XMVectorGetX( spriteCompo.m_renderData.m_mWorld.r[3] ) + spriteCompo.m_renderData.m_padding.x,
					XMVectorGetY( spriteCompo.m_renderData.m_mWorld.r[3] ) + spriteCompo.m_renderData.m_padding.y
					);

				phys::AABB2D spriteBox = { pos, halfW, halfH };

				// if sprite have its own camera

				if( spriteCompo.m_pCamera ){

					float zoom = spriteCompo.m_pCamera->m_fZoom * 0.5f; // halving on zoom (order of prod dont aff res)
					float camHalfW = spriteCompo.m_pCamera->m_viewPort.Width * zoom;
					float camHalfH = spriteCompo.m_pCamera->m_viewPort.Height * zoom;
					XMFLOAT2 camPos(
						-XMVectorGetX( spriteCompo.m_pCamera->m_mView.r[3] ),
						-XMVectorGetY( spriteCompo.m_pCamera->m_mView.r[3] )
						);

					phys::AABB2D ScamBox = { camPos, camHalfW, camHalfH };

					if( spriteBox.IsColliding_e( ScamBox ) )
						m_pSpriteRendererRef->Render( &spriteCompo, spriteCompo.m_pCamera );
					
					//else
					//	++iCulled;

				}else

				// if not

				if( spriteBox.IsColliding_e( camBox ) )
					m_pSpriteRendererRef->Render( &spriteCompo, m_pCameraRef );

				//else
				//	++iCulled;
			}
	}
	//win::UniqueFileLogger()<<"culled: "<<iCulled<<SZ_NEWLINE;
}

#include "TestState.h"
#include "gameWindow.h"
#include "FadeSpriteTask.h"

//========================================================================
// LAYER LAYER LAYER LAYER LAYER LAYER LAYER LAYER LAYER LAYER LAYER LAYER
//========================================================================
void TestLayer::VOnInit()
{
	// build obj 1
	//		create object		
	game::shared_Object_ptr pObject = std::make_shared<game::Object>(game::Object());
	//		create sprite component
	MAKE_SHARED_ALIGN16( shared_SpriteComp_ptr pSpriteComponent, SpriteComponent,
		(	&m_pGameWindowRef->m_device, "Contents/bg_mainmenu.png",
		1280.0f, 720.0f, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		sprite::E_ALPHA_BLENDED, sprite::E_LINEAR,
		&m_pGameWindowRef->m_spriteRenderer, nullptr	)			);
	//		add component to object
	pObject->AddComponent(pSpriteComponent);

	// build obj 2
	//		create object
	game::shared_Object_ptr pObject2 = std::make_shared<game::Object>(game::Object());
	//		create movable component
	MAKE_SHARED_ALIGN16(game::shared_MovableComponent_ptr pMovableComponent2, game::MovableComponent, () );
	pMovableComponent2->m_vCurrentPosition = DirectX::XMVectorSetY(pMovableComponent2->m_vCurrentPosition, -155.0f);
	//pSpriteComponent->m_pMovableRef.m_vCurrentPosition = DirectX::XMVectorSet(-1280.0f/2.0f, -720.0f/2.0f, 0.0f, 1.0f);
	//		create sprite component
	MAKE_SHARED_ALIGN16( shared_SpriteComp_ptr pSpriteComponent2, SpriteComponent,
		(	&m_pGameWindowRef->m_device, "Contents/buttons_mainmenu.png",
		626.0f, 648.0f/6.0f, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f/6.0f),
		sprite::E_ALPHA_BLENDED, sprite::E_LINEAR,
		&m_pGameWindowRef->m_spriteRenderer, pMovableComponent2 )		);
	//		create button component
	shared_ButtonComponent_ptr pButtonComponent2 = 
		std::make_shared<ButtonComponent>( pSpriteComponent2, XMFLOAT4(0.0f, 1.0f/6.0f, 1.0f, 1.0f/6.0f), 20.0f, 30.0f);
	//		add components to object
	pObject2->AddComponent(pSpriteComponent2);
	pObject2->AddComponent(pButtonComponent2);
	pObject2->AddComponent(pMovableComponent2);

	// build obj 3
	//		create object
	game::shared_Object_ptr pObject3 = std::make_shared<game::Object>(game::Object());
	//		create movable component
	MAKE_SHARED_ALIGN16(game::shared_MovableComponent_ptr pMovableComponent3, game::MovableComponent, () );
	pMovableComponent3->m_vCurrentPosition = DirectX::XMVectorSetY(pMovableComponent3->m_vCurrentPosition, -225.0f);
	//pSpriteComponent->m_pMovableRef.m_vCurrentPosition = DirectX::XMVectorSet(-1280.0f/2.0f, -720.0f/2.0f, 0.0f, 1.0f);
	//		create sprite component
	MAKE_SHARED_ALIGN16( shared_SpriteComp_ptr pSpriteComponent3, SpriteComponent,
		(	&m_pGameWindowRef->m_device, "Contents/buttons_mainmenu.png",
		626.0f, 648.0f/6.0f, XMFLOAT4(0.0f, 2.0f/6.0f, 1.0f, 1.0f/6.0f),
		sprite::E_ALPHA_BLENDED, sprite::E_LINEAR,
		&m_pGameWindowRef->m_spriteRenderer, pMovableComponent3 )		);
	//		create button component
	shared_ButtonComponent_ptr pButtonComponent3 = 
		std::make_shared<ButtonComponent>( pSpriteComponent3, XMFLOAT4(0.0f, 3.0f/6.0f, 1.0f, 1.0f/6.0f), 20.0f, 30.0f);
	//		add components to object
	pObject3->AddComponent(pSpriteComponent3);
	pObject3->AddComponent(pButtonComponent3);
	pObject3->AddComponent(pMovableComponent3);

	// build obj 4
	//		create object
	game::shared_Object_ptr pObject4 = std::make_shared<game::Object>(game::Object());
	//		create movable component
	MAKE_SHARED_ALIGN16(game::shared_MovableComponent_ptr pMovableComponent4, game::MovableComponent, () );
	pMovableComponent4->m_vCurrentPosition = DirectX::XMVectorSetY(pMovableComponent4->m_vCurrentPosition, -300.0f);
	//pSpriteComponent->m_pMovableRef.m_vCurrentPosition = DirectX::XMVectorSet(-1280.0f/2.0f, -720.0f/2.0f, 0.0f, 1.0f);
	//		create sprite component
	MAKE_SHARED_ALIGN16( shared_SpriteComp_ptr pSpriteComponent4, SpriteComponent,
		(	&m_pGameWindowRef->m_device, "Contents/buttons_mainmenu.png",
		626.0f, 648.0f/6.0f, XMFLOAT4(0.0f, 4.0f/6.0f, 1.0f, 1.0f/6.0f),
		sprite::E_ALPHA_BLENDED, sprite::E_LINEAR,
		&m_pGameWindowRef->m_spriteRenderer, pMovableComponent4 )		);
	//		create button component
	shared_ButtonComponent_ptr pButtonComponent4 = 
		std::make_shared<ButtonComponent>( pSpriteComponent4, XMFLOAT4(0.0f, 5.0f/6.0f, 1.0f, 1.0f/6.0f), 20.0f, 30.0f);
	//		add components to object
	pObject4->AddComponent(pSpriteComponent4);
	pObject4->AddComponent(pButtonComponent4);
	pObject4->AddComponent(pMovableComponent4);

	// build obj 5
	//		create object		
	game::shared_Object_ptr pObject5 = std::make_shared<game::Object>(game::Object());
	//		create sprite component
	MAKE_SHARED_ALIGN16( shared_SpriteComp_ptr pSpriteComponent5, SpriteComponent,
		(	&m_pGameWindowRef->m_device, "Contents/bg_mainmenu.png",
		1280.0f, 720.0f, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		sprite::E_ALPHA_BLENDED, sprite::E_LINEAR,
		&m_pGameWindowRef->m_spriteRenderer, nullptr	)			);
	pSpriteComponent5->m_renderData.m_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//		add component to object
	pObject5->AddComponent(pSpriteComponent5);


	//---

	// add components to systems

	m_sprites.push_back(pSpriteComponent );
	m_sprites.push_back(pSpriteComponent2 );
	m_sprites.push_back(pSpriteComponent3);
	m_sprites.push_back(pSpriteComponent4);
	m_sprites.push_back(pSpriteComponent5);
	//
	m_buttons.push_back(pButtonComponent2);
	m_buttons.push_back(pButtonComponent3);
	m_buttons.push_back(pButtonComponent4);

	// add objects to layer

	AddObject(pObject);
	AddObject(pObject2);
	AddObject(pObject3);
	AddObject(pObject4);
	AddObject(pObject5);

	// create fader task
	m_tasker.AddTask( std::make_shared<FadeSpriteTask>(0.5f, pSpriteComponent5));
}

void TestLayer::VOnUpdate( const double dAccum_p, const double dDelta_p )
{
	XMFLOAT2 m_mousePos((float)m_pGameWindowRef->m_mouseInput.GetPos().x, (float)m_pGameWindowRef->m_mouseInput.GetPos().y);
	m_pGameWindowRef->m_mouseInput.ConvertMousePosToNormalizedScreenSpace(m_mousePos.x, m_mousePos.y, m_pGameWindowRef->m_cliRect.w,  m_pGameWindowRef->m_cliRect.h);

	bool bOneHover = false;
	for( buttonables::iterator it = m_buttons.begin(), itEnd = m_buttons.end();
		it!=itEnd;
		++it){

			if( !bOneHover && (*it)->GetPointCollision(m_mousePos) ){

				(*it)->SetHoverImage();
				bOneHover = true;					
			}
			else{

				(*it)->SetNormalImage();
			}
	}

	//

	m_pGameWindowRef->m_spriteRenderer.m_camera.Update();

	//

	m_tasker.Update( dAccum_p, dDelta_p );
}

void TestLayer::VOnDraw( const double interpolation_p )
{
	for( sprites::iterator it = m_sprites.begin(), itEnd = m_sprites.end();
		it != itEnd;
		++it ){

			(*it)->OnDraw(interpolation_p);
	}

	//m_pGameWindowRef->m_spriteRenderer.m_queue.ResetState(E_VS_CBuffer0);
	//m_pGameWindowRef->m_spriteRenderer.m_queue.ResetState(E_VS_CBuffer1);
	m_pGameWindowRef->m_spriteRenderer.Raster(m_pGameWindowRef->m_device.GetContext());
}
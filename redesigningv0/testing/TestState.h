#pragma once


// standard includes
#include <new>

// private includes
#include "../namespace game/State.h"
#include "../namespace sprite/SpriteRenderer.h"

#include "../namespace render/dx/Device.h"
#include "../namespace sprite/binders cache/binderscache.h"
#include "../namespace render/DrawablesQueue.h"
#include "../namespace render/dx/HLSLResource.h"
#include "../namespace sprite/Movable.h"
#include "../namespace sprite/DrawableCbuffer.h"

#include "../namespace win/FileLogger.h"

#include "../namespace win/MouseInput.h"
#include "../namespace game/components/MovableComponent.h"

using namespace DirectX;
using namespace sprite;
using namespace dx;

//========================================================================
// SPRITE COMPONENT
//========================================================================

class SpriteComponent:public game::Component{
public:
	friend class SpriteRenderer;

	//------------------------------------------------------------------------
	// Used to sort the draw calls
	// This level of the rendering cant worry about this more specific stuff!
	//------------------------------------------------------------------------
	union SortMask{
		struct{
			UINT layer			: 2;	// 4 layers: game, HUD, full screen effect...
			UINT viewport		: 3;	// 8 viewports: split screens, portals, mirrors...
			UINT viewportLayer	: 3;	// 8 viewport layers: skybox, world, fx, HUD...
			UINT transparency	: 2;	// 4 modes: opaque, blended, additive, subtractive...
			UINT Zdepth			: 24;	// 0 - 16 777 216 drawables depth range
			UINT shaderID		: 15;
			UINT textureID		: 15;
		} bitField;

		UINT64 intRepresentation;
	}m_sortKey;

	//------------------------------------------------------------------------
	// Use this to access specific index on the sprite pipe state
	//------------------------------------------------------------------------
	enum E_SPRITEBINDS{

		E_DRAWABLECBUFFER = 0, // world, uv
		E_TEXTURE,
		E_SAMPLER,
		E_BLEND
	};

	int m_TextureID, m_BlendModeID, m_FilterModeID, m_ShaderID;
	int m_iShaderPermutation;

	dx::State m_pipeState;	// cbuffer, texture, blend state, sampler state
	sprite::DrawableCbuffer m_renderData;
	sprite::BindVSDrawableCBuffer * m_pBindDrawableCB;


	sprite::SpriteRenderer * m_pSpriteRendererRef;

public:

	game::weak_MovableComponent_ptr m_pMovableRef;

	//------------------------------------------------------------------------
	// ctor
	//------------------------------------------------------------------------
	SpriteComponent(	dx::Device * pDevice_p,
						const char * szTexture_p, 
						float fWidth_p, float fHeight_p,
						XMFLOAT4 uvRect_p,
						E_BLENDTYPE blendType_p, E_SAMPLERTYPE sampler_p,
						SpriteRenderer * pSpriteRenderer_p,
						const game::shared_MovableComponent_ptr & pMovable_p ){

		m_pMovableRef = pMovable_p;

		m_TextureID= m_BlendModeID= m_FilterModeID= m_ShaderID = 21;
		m_pSpriteRendererRef = pSpriteRenderer_p;
		m_iShaderPermutation = 0;

		m_renderData.m_mWorld = XMMatrixIdentity();

		m_renderData.m_res.x = fWidth_p;
		m_renderData.m_res.y = fHeight_p;
		m_renderData.m_uvRect = uvRect_p;
		m_renderData.m_bUpdate = true;

		// creates a ID3DBuffer for the vs constant buffer

		dx::BufferResource::CreationParams cbufferParams;
		ZeroMemory(&cbufferParams, sizeof(dx::BufferResource::CreationParams));
		cbufferParams.desc.bufferDesc.ByteWidth = DrawableCbuffer::s_SIZE;
		cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbufferParams.desc.bufferDesc.CPUAccessFlags = 0;
		cbufferParams.desc.bufferDesc.StructureByteStride = 0;
		cbufferParams.desc.bufferDesc.MiscFlags = 0;

		ID3D11Buffer * pBuffer = NULL;
		pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, pBuffer );

		// initialize pipe state for this sprite
		m_pipeState.AddBinderCommand( std::make_shared<BindVSDrawableCBuffer>(pBuffer, shared_DrawableCbuffer_ptr(&m_renderData, &gen::NoOp<DrawableCbuffer>)) );

		m_pipeState.AddBinderCommand( dx::shared_Binder_ptr(&pSpriteRenderer_p->m_texs.Get(szTexture_p), &gen::NoOp<dx::Binder>  ));
		m_pipeState.AddBinderCommand( dx::shared_Binder_ptr(&pSpriteRenderer_p->m_samplers.GetSamplerBind(sampler_p), &gen::NoOp<dx::Binder>  ));
		m_pipeState.AddBinderCommand( dx::shared_Binder_ptr(&pSpriteRenderer_p->m_blends.GetBlendBind(blendType_p), &gen::NoOp<dx::Binder>  ));
	}
	//------------------------------------------------------------------------
	// dctor
	//------------------------------------------------------------------------
	~SpriteComponent(){
		BREAKHERE;
	}

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	virtual void VOnUpdate( double, double ){
		
		if( game::shared_MovableComponent_ptr ptr = m_pMovableRef.lock() ){
			
			ptr->Step();
		}
	}

	//------------------------------------------------------------------------
	// interpolates
	//------------------------------------------------------------------------
	void OnDraw( double dInterpolation_p ){

		// interpolate
		if( game::shared_MovableComponent_ptr pMovable = m_pMovableRef.lock() ){

			DirectX::XMMATRIX && mNewWorld = pMovable->ComputeRenderWorldMatrix( (float)dInterpolation_p );

			if( memcmp( &m_renderData.m_mWorld, &mNewWorld, sizeof(DirectX::XMMATRIX)) ){

				m_renderData.m_mWorld = mNewWorld;

				// send to GPU?
				m_renderData.m_bUpdate = true;
			}
			DBG(m_renderData.m_bUpdate = true;)
			//m_pipeState.m_binds

			//---
		}
			m_pSpriteRendererRef->Render(this);
		
	}


	ALLIGN16ONLY;
};

 typedef std::shared_ptr<SpriteComponent> shared_SpriteComp_ptr;
 
 //========================================================================
 // BUTTON COMPONENT
 //========================================================================
 class ButtonComponent:public game::Component{

	 shared_SpriteComp_ptr m_pSpriteCompoRef;
	 XMFLOAT4 m_uv_Normal, m_uv_Hover;
	 XMFLOAT2 m_borderGap;	// used to make the "collision box" smaller
 public:

	 //------------------------------------------------------------------------
	 // ctor
	 //------------------------------------------------------------------------
	 ButtonComponent( const shared_SpriteComp_ptr & pSpriteCompo_p, XMFLOAT4 uvHover_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f ): m_pSpriteCompoRef(pSpriteCompo_p){

		 assert(pSpriteCompo_p);

		 m_uv_Normal = pSpriteCompo_p->m_renderData.m_uvRect;
		 m_uv_Hover = uvHover_p;

		 m_borderGap.x = wBorder_p;
		 m_borderGap.y = hBorder_p;
	 }

	 void SetHoverImage(){

		 m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Hover;
	 }
	 void SetNormalImage(){

		 m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Normal;
	 }

	 //------------------------------------------------------------------------
	 // use for mouse collision
	 //------------------------------------------------------------------------
	 bool GetPointCollision( XMFLOAT2 mouseXY_p ){

		 float halfW = m_pSpriteCompoRef->m_renderData.m_res.x / 2.0f - m_borderGap.x;

		 float x = XMVectorGetX(m_pSpriteCompoRef->m_renderData.m_mWorld.r[3]);
		 if( mouseXY_p.x >  x - halfW 
			 && mouseXY_p.x < x + halfW ){

				 float halfH =  m_pSpriteCompoRef->m_renderData.m_res.y / 2.0f - m_borderGap.y;
				 float y = XMVectorGetY(m_pSpriteCompoRef->m_renderData.m_mWorld.r[3]);

				 if( mouseXY_p.y > y - halfH
					 && mouseXY_p.y < y + halfH ){

						 return true;
				 }
		 }

		 return false;
	 }
 };

 typedef std::shared_ptr<ButtonComponent> shared_ButtonComponent_ptr;
//========================================================================
// LAYER
//========================================================================
class TestLayer: public game::Layer{

	 dx::Device * m_pDevice;
	 SpriteRenderer * m_pSpriteRenderer;

	 win::MouseInput * m_pMouseInputRef;

	 typedef std::vector<shared_SpriteComp_ptr > renderables;
	 typedef std::vector<shared_ButtonComponent_ptr> buttonables;

	 renderables m_renderables;
	 buttonables m_buttons;

public:
	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	TestLayer( dx::Device * pDevice_p, SpriteRenderer * pSpriteRenderer_p, win::MouseInput * pMouseInput  ){

		m_pDevice = pDevice_p;
		m_pSpriteRenderer = pSpriteRenderer_p;
		m_pMouseInputRef = pMouseInput;
	}

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	void VInit(){

		// build obj 1
		//		create object		
		game::shared_Object_ptr pObject = std::make_shared<game::Object>(game::Object());
		//		create sprite component
		MAKE_SHARED_ALIGN16( shared_SpriteComp_ptr pSpriteComponent, SpriteComponent,
			(	m_pDevice, "Contents/bg_mainmenu.png",
				1280.0f, 720.0f, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
				sprite::E_ALPHA_BLENDED, sprite::E_LINEAR,
				m_pSpriteRenderer, nullptr	)			);
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
			(	m_pDevice, "Contents/buttons_mainmenu.png",
				626.0f, 648.0f/6.0f, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f/6.0f),
				sprite::E_ALPHA_BLENDED, sprite::E_LINEAR,
				m_pSpriteRenderer, pMovableComponent2 )		);
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
			(	m_pDevice, "Contents/buttons_mainmenu.png",
			626.0f, 648.0f/6.0f, XMFLOAT4(0.0f, 2.0f/6.0f, 1.0f, 1.0f/6.0f),
			sprite::E_ALPHA_BLENDED, sprite::E_LINEAR,
			m_pSpriteRenderer, pMovableComponent3 )		);
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
			(	m_pDevice, "Contents/buttons_mainmenu.png",
			626.0f, 648.0f/6.0f, XMFLOAT4(0.0f, 4.0f/6.0f, 1.0f, 1.0f/6.0f),
			sprite::E_ALPHA_BLENDED, sprite::E_LINEAR,
			m_pSpriteRenderer, pMovableComponent4 )		);
		//		create button component
		shared_ButtonComponent_ptr pButtonComponent4 = 
			std::make_shared<ButtonComponent>( pSpriteComponent4, XMFLOAT4(0.0f, 5.0f/6.0f, 1.0f, 1.0f/6.0f), 20.0f, 30.0f);
		//		add components to object
		pObject4->AddComponent(pSpriteComponent4);
		pObject4->AddComponent(pButtonComponent4);
		pObject4->AddComponent(pMovableComponent4);



		
		//---
		
		// add components to systems
		 
		m_renderables.push_back(pSpriteComponent );
		m_renderables.push_back(pSpriteComponent2 );
		m_renderables.push_back(pSpriteComponent3);
		m_renderables.push_back(pSpriteComponent4);
		m_buttons.push_back(pButtonComponent2);
		m_buttons.push_back(pButtonComponent3);
		m_buttons.push_back(pButtonComponent4);
		
		// add objects to layer

		AddObject(pObject);
		AddObject(pObject2);
		AddObject(pObject3);
		AddObject(pObject4);
	}

	void VUpdate( const double , const double ){

		XMFLOAT2 m_mousePos((float)m_pMouseInputRef->GetPos().x, (float)m_pMouseInputRef->GetPos().y);
		m_pMouseInputRef->ConvertMousePosToNormalizedScreenSpace(m_mousePos.x, m_mousePos.y, 1280, 720);

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

	}

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	void VDraw( const double interpolation_p ){

		m_pSpriteRenderer->m_camera.Update();

		for( renderables::iterator it = m_renderables.begin(), itEnd = m_renderables.end();
			it != itEnd;
			++it ){

				(*it)->OnDraw(interpolation_p);
		}

		m_pSpriteRenderer->m_queue.ResetState(E_GS_CBuffer0);
		m_pSpriteRenderer->m_queue.ResetState(E_GS_CBuffer1);
		m_pSpriteRenderer->Raster( m_pDevice->GetContext());

	}

};
//========================================================================
// STATE
//========================================================================
class TestState: public game::State{

	TestLayer myLayer;
	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	void VInit(){

		AddLayer( game::shared_Layer_ptr(&myLayer, &gen::NoOp<game::Layer>));
	}

public:
	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	TestState( dx::Device * pDevice_p, SpriteRenderer * pSpriteRenderer_p, win::MouseInput * pMouseInput ):myLayer(pDevice_p, pSpriteRenderer_p, pMouseInput){};
	~TestState(){};
};
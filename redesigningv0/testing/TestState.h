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
#include "../namespace game/TaskMachine.h"



using namespace DirectX;
using namespace sprite;
using namespace dx;

//========================================================================
// SPRITE COMPONENT
//========================================================================

class SpriteComponent:public game::Component{

	friend class SpriteRenderer;

public:
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
		m_renderData.m_color.x = m_renderData.m_color.y = m_renderData.m_color.z = m_renderData.m_color.w = 1.0f;

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
			//DBG(m_renderData.m_bUpdate = true;)
			//m_pipeState.m_binds

			//---
		}
		//m_renderData.m_bUpdate = true;
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
class TestGameWindow;
class TestLayer: public game::Layer{

	typedef std::vector<shared_SpriteComp_ptr > sprites;
	typedef std::vector<shared_ButtonComponent_ptr> buttonables;

	//

	 TestGameWindow * m_pGameWindowRef;
	 sprites m_sprites;
	 buttonables m_buttons;

	game::TaskMachine m_tasker;

public:
	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	TestLayer( TestGameWindow * pGameWindowRef_p  ): m_pGameWindowRef(pGameWindowRef_p){}

	//------------------------------------------------------------------------
	// build the game objects
	//------------------------------------------------------------------------
	void VOnInit();

	//------------------------------------------------------------------------
	// currently just check the buttons for hover state
	//------------------------------------------------------------------------
	void VOnUpdate( const double , const double );

	//------------------------------------------------------------------------
	// draw the sprite components
	//------------------------------------------------------------------------
	void VOnDraw( const double interpolation_p );

};
//========================================================================
// STATE
//========================================================================
class TestState: public game::State{

	TestLayer myLayer;

	//------------------------------------------------------------------------
	// just add a layer
	//------------------------------------------------------------------------
	void VOnInit(){AddLayer( game::shared_Layer_ptr(&myLayer, &gen::NoOp<game::Layer>));}

public:

	//------------------------------------------------------------------------
	// ctor/dctor
	//------------------------------------------------------------------------
	TestState( TestGameWindow * pGameWnd_p ):myLayer(pGameWnd_p){};
	~TestState(){};
};
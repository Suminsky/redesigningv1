#pragma once


// standard includes

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

using namespace DirectX;
using namespace sprite;
using namespace dx;

//========================================================================
// OBJECT
//========================================================================
class SpriteComponent:public game::AComponent{
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

	Movable m_trafo;

	//------------------------------------------------------------------------
	// ctor
	//------------------------------------------------------------------------
	SpriteComponent(	dx::Device * pDevice_p,
						const char * szTexture_p, 
						float fWidth_p, float fHeight_p,
						float fUVOffset_U_p, float fUVOffset_V_p,
						E_BLENDTYPE blendType_p, E_SAMPLERTYPE sampler_p,
						SpriteRenderer * pSpriteRenderer_p ){

		m_TextureID= m_BlendModeID= m_FilterModeID= m_ShaderID = 21;
		m_pSpriteRendererRef = pSpriteRenderer_p;
		m_iShaderPermutation = 0;

		m_renderData.m_mWorld = XMMatrixIdentity();

		m_renderData.m_res.x = fWidth_p;
		m_renderData.m_res.y = fHeight_p;
		m_renderData.m_uvOffset.x = fUVOffset_U_p;
		m_renderData.m_uvOffset.y = fUVOffset_V_p;
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

	}

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	virtual void VOnUpdate( double, double ){

		m_trafo.Step();
	}

	//------------------------------------------------------------------------
	// interpolates
	//------------------------------------------------------------------------
	void OnDraw( double dInterpolation_p ){

		// interpolate

		DirectX::XMMATRIX && mNewWorld = m_trafo.ComputeRenderWorldMatrix( (float)dInterpolation_p );

		if( memcmp( &m_renderData.m_mWorld, &mNewWorld, sizeof(DirectX::XMMATRIX)) ){

			m_renderData.m_mWorld = mNewWorld;

			// send to GPU?
			m_renderData.m_bUpdate = true;
		}
		DBG(m_renderData.m_bUpdate = true;)
		//m_pipeState.m_binds

		//---

		m_pSpriteRendererRef->Render(this);

	}
};

 typedef std::shared_ptr<SpriteComponent> shared_SpriteComp_ptr;

//========================================================================
// LAYER
//========================================================================
class TestLayer: public game::Layer{

	 dx::Device * m_pDevice;
	 SpriteRenderer * m_pSpriteRenderer;

	 typedef std::vector<std::shared_ptr<SpriteComponent> > renderables;

	 renderables m_renderables;

public:
	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	TestLayer( dx::Device * pDevice_p, SpriteRenderer * pSpriteRenderer_p ){
		m_pDevice = pDevice_p;
		m_pSpriteRenderer = pSpriteRenderer_p;
	}

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	void VInit(){

		static game::shared_Object_ptr pObject = std::make_shared<game::Object>(game::Object());
		static game::shared_Object_ptr pObject2 = std::make_shared<game::Object>(game::Object());

#ifdef _WIN64
		static std::shared_ptr<SpriteComponent> pComponent = 
			std::make_shared<SpriteComponent>(	m_pDevice,
												"Contents/samusSetTry1.png",
												468.0f, 234.0f, 0.0f, 0.0f,
												sprite::E_ALPHA_BLENDED, sprite::E_NONE,
												m_pSpriteRenderer);
		static std::shared_ptr<SpriteComponent> pComponent2 = 
			std::make_shared<SpriteComponent>(	m_pDevice, "Contents/samusSetTry1.png",
												468.0f, 234.0f, 0.0f, 0.0f,
												sprite::E_ALPHA_BLENDED, sprite::E_NONE, m_pSpriteRenderer );
#else
		static std::shared_ptr<SpriteComponent> pComponent(new SpriteComponent(m_pDevice,
			"Contents/samusSetTry1.png",
			468.0f, 234.0f, 0.0f, 0.0f,
			sprite::E_ALPHA_BLENDED, sprite::E_NONE,
			m_pSpriteRenderer));
		static std::shared_ptr<SpriteComponent> pComponent2(new SpriteComponent(m_pDevice,
			"Contents/samusSetTry1.png",
			468.0f, 234.0f, 0.0f, 0.0f,
			sprite::E_ALPHA_BLENDED, sprite::E_NONE,
			m_pSpriteRenderer));

#endif

		pComponent2->m_trafo.m_vCurrentPosition = DirectX::XMVectorSetY(pComponent2->m_trafo.m_vCurrentPosition, 100.0f);

		m_renderables.push_back(pComponent );
		m_renderables.push_back(pComponent2 );
		pObject->AddComponent(pComponent);
		pObject2->AddComponent(pComponent2);
		AddObject(pObject);
		AddObject(pObject2);
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
	TestState( dx::Device * pDevice_p, SpriteRenderer * pSpriteRenderer_p ):myLayer(pDevice_p, pSpriteRenderer_p){};
	~TestState(){};
};
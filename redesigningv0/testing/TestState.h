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

using namespace DirectX;
using namespace sprite;

//========================================================================
// OBJECT
//========================================================================
class SpriteComponent:public game::AComponent{

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


	sprite::SpriteRenderer * m_pRenderer;

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

		m_pRenderer = pSpriteRenderer_p;
		m_iShaderPermutation = 0;

		m_renderData.m_mWorld = XMMatrixIdentity();

		m_renderData.m_res.x = fWidth_p;
		m_renderData.m_res.y = fHeight_p;
		m_renderData.m_uvOffset.x = fUVOffset_U_p;
		m_renderData.m_uvOffset.y = fUVOffset_V_p;

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

		m_pipeState.AddBinderCommand(  std::make_shared<BindVSDrawableCBuffer>(pBuffer, shared_DrawableCbuffer_ptr(&m_renderData, &gen::NoOp<DrawableCbuffer>) )  );

		m_pipeState.AddBinderCommand( dx::shared_Binder_ptr(&pSpriteRenderer_p->m_texs.Get(szTexture_p), &gen::NoOp<dx::Binder>  ));
		m_pipeState.AddBinderCommand( dx::shared_Binder_ptr(&pSpriteRenderer_p->m_samplers.GetSamplerBind(sampler_p), &gen::NoOp<dx::Binder>  ));
		m_pipeState.AddBinderCommand( dx::shared_Binder_ptr(&pSpriteRenderer_p->m_blends.GetBlendBind(blendType_p), &gen::NoOp<dx::Binder>  ));
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

		//---

		static render::Drawable drawable;
		drawable.m_PipeStatesGroup.resize(0);

		drawable.m_sortKey = m_sortKey.intRepresentation;
		// shader
		drawable.m_PipeStatesGroup.push_back(m_pRenderer->m_spriteShaderRes.m_permutations[m_iShaderPermutation].m_pPipeState);
		// vb
		drawable.m_PipeStatesGroup.push_back( dx::shared_State_ptr(&m_pRenderer->m_defaultVertexInput, &gen::NoOp<dx::State>)  );
		// camera
		drawable.m_PipeStatesGroup.push_back( dx::shared_State_ptr(&m_pRenderer->m_camera.m_pipeState, &gen::NoOp<dx::State>)  );
		// sprite
		drawable.m_PipeStatesGroup.push_back(  dx::shared_State_ptr(&m_pipeState, &gen::NoOp<dx::State>) );

		// draw call
		drawable.m_pDrawCall.reset(&m_pRenderer->m_drawIndexed, &gen::NoOp<dx::DrawCall>);

		m_pRenderer->m_queue.Submit( &drawable );
	}
};

//========================================================================
// LAYER
//========================================================================
class TestLayer: public game::Layer{


	void VDraw( const double interpolation_p ){


	}

};
//========================================================================
// STATE
//========================================================================
class TestState: public game::State{

	TestLayer myLayer;

	void VInit(){


		AddLayer( game::shared_Layer_ptr(&myLayer, &gen::NoOp<game::Layer>));
	}
};



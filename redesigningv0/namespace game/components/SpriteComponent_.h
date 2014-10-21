#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/19
	created:	19:11:2013   15:56
	file:		SpriteComponent_.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <DirectXMath.h>

// private includes
#include "../Component.h"
#include "../../namespace gen/TreeNode.h"
#include "../../namespace gen/Pool.h"
#include "../ComponentFactory.h"
#include "../Event.h"

#include "../../namespace sprite/SpriteRenderer.h"
#include "../../namespace sprite/Camera.h"

namespace game{

	struct TextureID_Binder_Pair;

	class SpriteComponent_: public Component{

		friend class sprite::SpriteRenderer;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteComponent_();
		SpriteComponent_(	dx::Device * pDevice_p,
			const char * szTexture_p, float fWidth_p, float fHeight_p, DirectX::XMFLOAT4 uvRect_p,
			sprite::E_BLENDTYPE blendType_p,  sprite::E_SAMPLERTYPE sampler_p,
			sprite::SpriteRenderer * pSpriteRenderer_p );
		~SpriteComponent_();

		void Init(dx::Device * pDevice_p,
			const char * szTexture_p, float fWidth_p, float fHeight_p, DirectX::XMFLOAT4 uvRect_p,
			sprite::E_BLENDTYPE blendType_p,  sprite::E_SAMPLERTYPE sampler_p,
			sprite::SpriteRenderer * pSpriteRenderer_p);

		void SpriteComponent_::Init( dx::Device * pDevice_p,
			game::TextureID_Binder_Pair * texture_p,
			float fWidth_p,
			float fHeight_p,
			DirectX::XMFLOAT4 uvRect_p,
			sprite::E_BLENDTYPE blendType_p,
			sprite::E_SAMPLERTYPE sampler_p,
			sprite::SpriteRenderer * pSpriteRenderer_p );

		//------------------------------------------------------------------------
		// interpolates
		//------------------------------------------------------------------------
		void OnDraw( double dInterpolation_p );
		void OnDraw( double dInterpolation_p, sprite::Camera * pCamera_p );

		void FlipHorzToogle(); void FlipHorz(); void UnFlipHorz();
		void FlipVertcToogle(); void FlipVertc(); void UnFlipVertc();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnAttach();
		void VOnDetach();

		//------------------------------------------------------------------------
		// delegates
		//------------------------------------------------------------------------
		void OnColorEventDelegate( const Event<ComponentEventData> & event_p );
		void OnTransformEventDelegate( const Event<ComponentEventData> & event_p );
		void OnAnimEventDelegate( const Event<ComponentEventData> & event_p );

		int GetTextureID() const{ return m_TextureID; }

		void SetColor( DirectX::XMFLOAT4 color_p );

	public:

		sprite::SortMask m_sortKey;

		DirectX::XMFLOAT4X4 m_currentTrafo;
		DirectX::XMFLOAT4X4 m_previousTrafo;

		DirectX::XMFLOAT4 m_currentColor;
		DirectX::XMFLOAT4 m_previousColor;

		int m_TextureID, m_BlendModeID, m_FilterModeID, m_ShaderID;
		int m_iShaderPermutation;

		bool m_bHFlip, m_bVFlip;

		dx::PipeState m_pipeState;	// cbuffer, texture, blend state, sampler state
		sprite::DrawableCbuffer m_renderData;
		sprite::BindVSDrawableCBuffer m_VSDrawableCbufferBinder;

		// testing
		ID3D11Buffer * m_pBuffer;
		sprite::Camera * m_pCamera;

		ALLIGN16ONLY;
	};

	typedef gen::pool_ptr<SpriteComponent_> pool_SpriteCompo__ptr;
	typedef std::shared_ptr<SpriteComponent_> shared_SpriteComponent__ptr;
	typedef std::weak_ptr<SpriteComponent_> weak_SpriteComponent__ptr;

	//========================================================================
	// 
	//========================================================================
	class SpriteComponent_Factory: public AComponentFactory{

		friend class TransformSystem;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteComponent_Factory( unsigned int maxComponents_p, dx::Device * pDevice_p, sprite::SpriteRenderer * pSpriteRenderer_p )
			:
		m_pool(maxComponents_p),
		m_pDeviceRef_p(pDevice_p), m_pRendererRef(pSpriteRenderer_p)
		{}

		gen::Pool<SpriteComponent_> m_pool;
		dx::Device * m_pDeviceRef_p;
		sprite::SpriteRenderer * m_pRendererRef;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static DirectX::XMFLOAT4 GetXYWH( text::GfigElementA * pGFig_p );
		static sprite::E_BLENDTYPE GetBlendType( text::GfigElementA * pGFig_p );
		static sprite::E_SAMPLERTYPE GetSamplerType( text::GfigElementA * pGFig_p );

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent(){

			/*pool_Component_ptr test;
			gen::pool_ptr<SpriteComponent_> spriteP( &m_pool );
			test = spriteP;*/

			 return pool_Component_ptr(m_pool);
		}
		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );
		static void CreateSprite( SpriteComponent_ & sprite_p, text::GfigElementA * pGFig_p, dx::Device * pDevice_p, sprite::SpriteRenderer * pRendererRef );
		static void LoadInstanceData( sprite::spriteInstance & inst_p, text::GfigElementA * pGFig_p );

		virtual pool_Component_ptr VCloneComponent( const Component * pCompo_p );

	};

	typedef std::shared_ptr<SpriteComponent_Factory> shared_SpriteComponent_Factory_ptr;
	typedef std::weak_ptr<SpriteComponent_Factory> weak_SpriteComponent_Factory_ptr;
}
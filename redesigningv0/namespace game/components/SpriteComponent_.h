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

		void Initialize(dx::Device * pDevice_p,
			const char * szTexture_p, float fWidth_p, float fHeight_p, DirectX::XMFLOAT4 uvRect_p,
			sprite::E_BLENDTYPE blendType_p,  sprite::E_SAMPLERTYPE sampler_p,
			sprite::SpriteRenderer * pSpriteRenderer_p);

		void SpriteComponent_::Initialize( dx::Device * pDevice_p,
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
		//void OnDraw( double dInterpolation_p );
		void InterpolateWorld( double dInterp_p );
		void InterpolateColor( double dInterp );

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
		void OnAnim_EventDelegate( const Event<ComponentEventData> & event_p );

		int GetTextureID() const{ return m_TextureID; }

		void SetColor( DirectX::XMFLOAT4 color_p );


	public:

		sprite::SortMask m_sortKey;
		sprite::DrawableCbuffer m_renderData;

		DirectX::XMFLOAT4X4 m_currentTrafo;
		DirectX::XMFLOAT4X4 m_previousTrafo;

		DirectX::XMFLOAT4 m_currentColor;
		DirectX::XMFLOAT4 m_previousColor;



		unsigned int m_TextureID, m_BlendModeID, m_SamplerModeID, m_ShaderID;
		int m_iShaderPermutation;

		bool m_bHFlip, m_bVFlip;

		dx::PipeState m_pipeState;	// cbuffer, texture, blend state, sampler state
		sprite::BindVSDrawableCBuffer m_VSDrawableCbufferBinder;

		// testing
		ID3D11Buffer * m_pBuffer;
		sprite::Camera * m_pCamera;

		//ALLIGN16ONLY;
	};

	typedef gen::pool_ptr<SpriteComponent_> pool_SpriteCompo__ptr;

	//========================================================================
	// 
	//========================================================================
	class SpriteComponent_Factory: public IComponentFactory{

		friend class TransformSystem;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteComponent_Factory( unsigned int maxComponents_p, dx::Device * pDevice_p, sprite::SpriteRenderer * pSpriteRenderer_p );

		SpriteComponent_ m_defaults;

		gen::Pool<SpriteComponent_> m_pool;
		dx::Device * m_pDeviceRef_p;
		sprite::SpriteRenderer * m_pRendererRef;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static DirectX::XMFLOAT4 GetXYWH( text::GfigElementA * pGFig_p );
		static sprite::E_BLENDTYPE GetBlendType( text::GfigElementA * pGFig_p );
		static sprite::E_SAMPLERTYPE GetSamplerType( text::GfigElementA * pGFig_p );

		static void UpdateXYWHFromGfig( DirectX::XMFLOAT4 & xywh_p, text::GfigElementA * pGFig_p );

		static void CreateSprite( SpriteComponent_ & sprite_p, text::GfigElementA * pGFig_p, dx::Device * pDevice_p, sprite::SpriteRenderer * pRendererRef );
		static void LoadInstanceData( sprite::spriteInstance & inst_p, text::GfigElementA * pGFig_p );


		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent(){ return pool_Component_ptr(m_pool);}
		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );
		virtual pool_Component_ptr VCloneComponent( const Component * pCompo_p );
		virtual void VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p );
		virtual void VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p );
		virtual void VSerialize( const Component * pCompo_p, const Component * pDefaults_p, text::GfigElementA * pGFig_p );
		virtual Component * VGetDefaultCompo();

		//------------------------------------------------------------------------
		// those 2 must receive an already named gfig
		//------------------------------------------------------------------------
		static void SerializeXYZW( const DirectX::XMFLOAT4 & xyzw_p, const DirectX::XMFLOAT4 & defs_p, text::GfigElementA & gFig_p );
		static bool SerializeBlendType( const sprite::E_BLENDTYPE eBlend_p, const sprite::E_BLENDTYPE def_p, text::GfigElementA & gFig_p );
		static bool SerializeSamplerType( const sprite::E_SAMPLERTYPE eSampler_p, const sprite::E_SAMPLERTYPE def_p, text::GfigElementA & gFig_p );
	};

	typedef std::shared_ptr<SpriteComponent_Factory> shared_SpriteComponent_Factory_ptr;
	typedef std::weak_ptr<SpriteComponent_Factory> weak_SpriteComponent_Factory_ptr;
}
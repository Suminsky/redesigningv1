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

		//union SortMask{
		//	struct{
		//		unsigned __int64 textureID		: 15;
		//		unsigned __int64 shaderID		: 15;
		//		unsigned __int64 transparency	: 2;	// 4 modes: opaque, blended, additive, subtractive...
		//		unsigned __int64 Zdepth			: 24;	// 0 - 16 777 216 drawables depth range
		//		unsigned __int64 viewportLayer	: 3;	// 8 viewport layers: skybox, world, fx, HUD...
		//		unsigned __int64 viewport		: 3;	// 8 viewports: split screens, portals, mirrors...
		//		unsigned __int64 layer			: 2;	// 4 layers: game, HUD, full screen effect...
		//	}bitfield;
		//	__int64 intRepresentation;
		//}
		sprite::SortMask m_sortKey;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteComponent_();
		SpriteComponent_(	dx::Device * pDevice_p,
			const char * szTexture_p, float fWidth_p, float fHeight_p, DirectX::XMFLOAT4 uvRect_p,
			sprite::E_BLENDTYPE blendType_p,  sprite::E_SAMPLERTYPE sampler_p,
			sprite::SpriteRenderer * pSpriteRenderer_p );

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

		void FlipHorz();
		void FlipVertc();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnAttach();

		//------------------------------------------------------------------------
		// delegates
		//------------------------------------------------------------------------
		void OnColorEventDelegate( const Event<ComponentEventData> & event_p );
		void OnTransformEventDelegate( const Event<ComponentEventData> & event_p );
		void OnAnimEventDelegate( const Event<ComponentEventData> & event_p );

	public:

		void SetColor( DirectX::XMFLOAT4 color_p );

		DirectX::XMFLOAT4X4 m_currentTrafo;
		DirectX::XMFLOAT4X4 m_previousTrafo;

		int m_TextureID, m_BlendModeID, m_FilterModeID, m_ShaderID;
		int m_iShaderPermutation;

		bool m_bHFlip, m_bVFlip;

		DirectX::XMFLOAT4 m_currentColor;
		DirectX::XMFLOAT4 m_previousColor;

		dx::PipeState m_pipeState;	// cbuffer, texture, blend state, sampler state
		sprite::DrawableCbuffer m_renderData;
		sprite::BindVSDrawableCBuffer m_VSDrawableCbufferBinder;

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

	};

	typedef std::shared_ptr<SpriteComponent_Factory> shared_SpriteComponent_Factory_ptr;
	typedef std::weak_ptr<SpriteComponent_Factory> weak_SpriteComponent_Factory_ptr;
}
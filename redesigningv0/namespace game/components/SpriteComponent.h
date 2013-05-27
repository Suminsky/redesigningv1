#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/25
	created:	25:5:2013   21:01
	file:		SpriteComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "MovableComponent.h"
#include "../../namespace sprite/SpriteRenderer.h"

namespace dx{
	class Device;
}


namespace game{

	class SpriteComponent: public Component{

		friend class sprite::SpriteRenderer;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteComponent(	dx::Device * pDevice_p,
							const char * szTexture_p, float fWidth_p, float fHeight_p, DirectX::XMFLOAT4 uvRect_p,
							sprite::E_BLENDTYPE blendType_p,  sprite::E_SAMPLERTYPE sampler_p,
							sprite::SpriteRenderer * pSpriteRenderer_p,
							const game::shared_MovableComponent_ptr & pMovable_p );

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
		void OnDraw( double dInterpolation_p );

	public:
		//------------------------------------------------------------------------
		// Used to sort the draw calls
		// This level of the rendering cant worry about this more specific stuff!
		//------------------------------------------------------------------------
		union SortMask{
			struct{
				unsigned int layer			: 2;	// 4 layers: game, HUD, full screen effect...
				unsigned int viewport		: 3;	// 8 viewports: split screens, portals, mirrors...
				unsigned int viewportLayer	: 3;	// 8 viewport layers: skybox, world, fx, HUD...
				unsigned int transparency	: 2;	// 4 modes: opaque, blended, additive, subtractive...
				unsigned int Zdepth			: 24;	// 0 - 16 777 216 drawables depth range
				unsigned int shaderID		: 15;
				unsigned int textureID		: 15;
			} bitField;

			unsigned __int64 intRepresentation;
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

		weak_MovableComponent_ptr m_pMovableRef;

		int m_TextureID, m_BlendModeID, m_FilterModeID, m_ShaderID;
		int m_iShaderPermutation;

		dx::State m_pipeState;	// cbuffer, texture, blend state, sampler state
		sprite::DrawableCbuffer m_renderData;
		sprite::BindVSDrawableCBuffer * m_pBindDrawableCB;

		sprite::SpriteRenderer * m_pSpriteRendererRef;

		ALLIGN16ONLY;
	};

	typedef std::shared_ptr<SpriteComponent> shared_SpriteComponent_ptr;
}
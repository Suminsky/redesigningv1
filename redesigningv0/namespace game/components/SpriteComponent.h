#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/25
	created:	25:5:2013   21:01
	file:		SpriteComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	testing

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "MovableComponent.h"
#include "../../namespace sprite/SpriteRenderer.h"
#include "../../namespace sprite/Camera.h"
#include "../Event.h"

namespace dx{
	class Device;
}

namespace game{

	class SpriteComponent: public Component{

		friend class sprite::SpriteRenderer;

	public:

		//------------------------------------------------------------------------
		// Used to sort the draw calls
		// This level of the rendering cant worry about this more specific stuff!
		// Figured out that byte order for the bitfields, just like on a union,
		// is different from struct and arrays:
		// struct{ m0, m1, m2} == array[3];
		// union{ char a, b, c, d } == integer{ d<<24 | c << 16 | b << 8 | a << 0 }
		// And the bitfields are like
		// struct{ :1, :3, :4, :1 } == xxxxxxxx1, xxxxx111x, x1111xxxx, 1xxxxxxxx
		// I dont know why, and its not portable either.
		//------------------------------------------------------------------------
		union SortMask{
			struct{
				unsigned __int64 textureID		: 15;
				unsigned __int64 shaderID		: 15;
				unsigned __int64 transparency	: 2;	// 4 modes: opaque, blended, additive, subtractive...
				unsigned __int64 Zdepth			: 24;	// 0 - 16 777 216 drawables depth range
				unsigned __int64 viewportLayer	: 3;	// 8 viewport layers: skybox, world, fx, HUD...
				unsigned __int64 viewport		: 3;	// 8 viewports: split screens, portals, mirrors...
				unsigned __int64 layer			: 2;	// 4 layers: game, HUD, full screen effect...
			}bitfield;
			__int64 intRepresentation;
		}m_sortKey;

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
		void UpdatePreviousData(){

			if( game::shared_MovableComponent_ptr ptr = m_pMovableRef.lock() ){

				ptr->Step();
			}

			m_previousColor = m_currentColor;
		}

		//------------------------------------------------------------------------
		// interpolates
		//------------------------------------------------------------------------
		void OnDraw( double dInterpolation_p );
		void OnDraw( double dInterpolation_p, sprite::Camera * pCamera_p );

	public:

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
		DirectX::XMFLOAT4X4 m_currentTrafo;
		DirectX::XMFLOAT4X4 m_previousTrafo;

		int m_TextureID, m_BlendModeID, m_FilterModeID, m_ShaderID;
		int m_iShaderPermutation;

		DirectX::XMFLOAT4 m_currentColor;
		DirectX::XMFLOAT4 m_previousColor;

		dx::State m_pipeState;	// cbuffer, texture, blend state, sampler state
		sprite::DrawableCbuffer m_renderData;
		sprite::BindVSDrawableCBuffer m_VSDrawableCbufferBinder;

		sprite::SpriteRenderer * m_pSpriteRendererRef;

		ALLIGN16ONLY;
	};

	typedef std::shared_ptr<SpriteComponent> shared_SpriteComponent_ptr;
}
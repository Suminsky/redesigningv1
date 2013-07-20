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
#include "../../namespace sprite/Camera.h"

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
		struct SortMask{
			
				unsigned short textureID	: 15;
				unsigned short shaderID		: 15;
				unsigned int Zdepth			: 24;	// 0 - 16 777 216 drawables depth range
				unsigned char transparency	: 2;	// 4 modes: opaque, blended, additive, subtractive...
				unsigned char viewportLayer	: 3;	// 8 viewport layers: skybox, world, fx, HUD...
				unsigned char viewport		: 3;	// 8 viewports: split screens, portals, mirrors...
				unsigned char layer			: 2;	// 4 layers: game, HUD, full screen effect...

				__int64 intRepresentation(){

					return   __int64(layer			& 0x03)	<< (64 -2) 
							| __int64(viewport		& 0x07)	<< (64 -2 -3)
							| __int64(viewportLayer & 0x07)	<< (64 -2 -3 -3)
							| __int64(transparency  & 0x03)	<< (64 -2 -3 -3 -2)
							| __int64(Zdepth		& 0x00ffffff)	<< (64 -2 -3 -3 -2 -24)
							| __int64(shaderID		& 0x7fff)	<< (64 -2 -3 -3 -2 -24 -15)
							| __int64(textureID		& 0x7fff)	<< (64 -2 -3 -3 -2 -24 -15 -15);
				}
				void FromInt( __int64 int_p ){
					// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0011
					// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0111
					// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0111
					// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0011
					// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 1111
					// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0111 1111 1111 1111
					// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0111 1111 1111 1111
					// 
					// 1100 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
					// 0011 1000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
					// 0000 0111 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
					// 0000 0000 1100 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
					// 0000 0000 0011 1111 1111 1111 1111 1111 1100 0000 0000 0000 0000 0000 0000 0000
					// 0000 0000 0000 0000 0000 0000 0000 0000 0011 1111 1111 1111 1000 0000 0000 0000
					// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0111 1111 1111 1111
					// 
					// C000000000000000
					// 3800000000000000
					// 700000000000000
					// C0000000000000
					// 3FFFFFC0000000
					// 3FFF8000
					// 7FFF
					// 
					layer			= int_p & 0xC000000000000000;
					viewport		= int_p & 0x3800000000000000;
					viewportLayer	= int_p & 0x0700000000000000;
					transparency	= int_p & 0x00C0000000000000;
					Zdepth			= int_p & 0x003FFFFFC0000000;
					shaderID		= int_p & 0x000000003FFF8000;
					textureID		= int_p & 0x0000000000007FFF;
				}

				void Zero(){
					memset( this, 0, sizeof(SortMask));
				}
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
		virtual void VOnUpdate( double, double ){

			if( game::shared_MovableComponent_ptr ptr = m_pMovableRef.lock() ){

				ptr->Step();
			}
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

		int m_TextureID, m_BlendModeID, m_FilterModeID, m_ShaderID;
		int m_iShaderPermutation;

		dx::State m_pipeState;	// cbuffer, texture, blend state, sampler state
		sprite::DrawableCbuffer m_renderData;
		sprite::BindVSDrawableCBuffer m_VSDrawableCbufferBinder;

		sprite::SpriteRenderer * m_pSpriteRendererRef;

		ALLIGN16ONLY;
	};

	typedef std::shared_ptr<SpriteComponent> shared_SpriteComponent_ptr;
}
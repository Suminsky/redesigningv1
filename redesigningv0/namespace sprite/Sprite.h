//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/03/31
	created:	31:3:2013   21:03
	filename: 	c:\Users\Gateway\Documents\Visual Studio 2010\Projects\redesigningv1\redesigningv0\Sprite.h
	file path:	c:\Users\Gateway\Documents\Visual Studio 2010\Projects\redesigningv1\redesigningv0
	file base:	Sprite
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// system/standard headers

//#include <boost/smart_ptr.hpp>

// private headers
#include "../namespace gen/gen_data.h"
#include "../namespace win/win_macros.h"

#include "../namespace render/dx/Device.h"
#include "binders cache/binderscache.h"
#include "../namespace render/DrawablesQueue.h"
#include "../namespace render/dx/HLSLResource.h"
#include "Movable.h"
#include "DrawableCbuffer.h"

namespace sprite{


	//------------------------------------------------------------------------
	// Used to sort the draw calls
	// This level of the rendering cant worry about this more specific stuff!
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

			return   __int64(layer		& 0x03)	<< (64 -2) 
				| __int64(viewport		& 0x07)	<< (64 -2 -3)
				| __int64(viewportLayer & 0x07)	<< (64 -2 -3 -3)
				| __int64(transparency  & 0x03)	<< (64 -2 -3 -3 -2)
				| __int64(Zdepth		& 0x00ffffff)	<< (64 -2 -3 -3 -2 -24)
				| __int64(shaderID		& 0x7fff)	<< (64 -2 -3 -3 -2 -24 -15)
				| __int64(textureID		& 0x7fff)	<< (64 -2 -3 -3 -2 -24 -15 -15);
		}
		void FromInt( __int64 int_p ){

			layer			= ((int_p & 0xC000000000000000) >> (64 -2) );
			viewport		= ((int_p & 0x3800000000000000) >> (64 -2 -3) );
			viewportLayer	= ((int_p & 0x0700000000000000) >> (64 -2 -3 -3) );
			transparency	= ((int_p & 0x00C0000000000000) >> (64 -2 -3 -3 -2) );
			Zdepth			= ((int_p & 0x003FFFFFC0000000) >> (64 -2 -3 -3 -2 -24) );
			shaderID		= ((int_p & 0x000000003FFF8000) >> (64 -2 -3 -3 -2 -24 -15) );
			textureID		= ((int_p & 0x0000000000007FFF) >> (64 -2 -3 -3 -2 -24 -15 -15) );
		}

		void Zero(){
			memset( this, 0, sizeof(SortMask));
		}
	};

	class SpriteRenderer;

	//========================================================================
	// 
	//========================================================================
	class Sprite{

		friend class SpriteRenderer;

	public:

		Movable m_trafo;

		//------------------------------------------------------------------------
		// Use this to access specific index on the sprite pipe state
		//------------------------------------------------------------------------
		enum E_SPRITEBINDS{

			E_DRAWABLECBUFFER = 0,
			E_TEXTURE,
			E_SAMPLER,
			E_BLEND
		};

		//------------------------------------------------------------------------
		// ctor
		// builds the sprite pipe state based on the given parameters
		//------------------------------------------------------------------------
		Sprite( dx::Device * pDevice_p,
				const char * szTexture_p, 
				float fWidth_p, float fHeight_p,
				float fUVOffset_U_p, float fUVOffset_V_p,
				E_BLENDTYPE blendType_p, E_SAMPLERTYPE sampler_p,
				SpriteRenderer * pSpriteRenderer_p );

		//------------------------------------------------------------------------
		// check if needs to update buffer (if data is different), than it
		// interpolates the render data, and then send to buffer interface on the gpu
		// NOTE: interpolating linearly,position only
		// TODO:
		// create a function pointer proxy for this method, sprites that rotate will
		// have a different interpolation (accounting for rotation matrix)
		//------------------------------------------------------------------------
		void Update( double dInterpolation_p );

	private:

		dx::State m_pipeState;	// cbuffer, texture, blend state, sampler state
		SortMask m_renderSortKey; // used to sort on the render queue

		DrawableCbuffer m_renderData;
		BindVSDrawableCBuffer m_VSDrawableCbufferBinder;

		UINT64 m_currentPermutation;// used to pick the right permutation
		UINT m_iCurrentPermutationIndex;

	};
}
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

#include <boost/smart_ptr.hpp>

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
	};

	class SpriteRenderer;

	//========================================================================
	// 
	//========================================================================
	class Sprite{

		friend class SpriteRenderer;

	public:

		//------------------------------------------------------------------------
		// Use this to access specific index on the sprite pipe state
		//------------------------------------------------------------------------
		enum E_SPRITEBINDS{

			E_DRAWABLECBUFFER = 0,
			E_TEXTURE,
			E_SAMPLER,
			E_BLEND
		};

		Movable m_trafo;

	private:

		dx::State m_pipeState;	// cbuffer, texture, blend state, sampler state
		SortMask m_renderSortKey; // used to sort on the render queue

		DrawableCbuffer m_renderData;

		UINT64 m_currentPermutation;// used to pick the right permutation
		UINT m_iCurrentPermutationIndex;

	public:

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
	};
}
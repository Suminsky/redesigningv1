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

#include "../../namespace gen/gen_data.h"
#include "../../namespace win/win_macros.h"

#include "../../namespace render/dx/Device.h"
#include "../../namespace render/dx/pipeline/Binder.h"

namespace sprite{

	enum E_BLENDTYPE{

		E_ALPHA_BLENDED = 0,
		E_ADDITIVE,
		E_NONE_DEFAULT,
		E_MAX_BLEND
	};

	//========================================================================
	// cache blend state binder commands
	// all blends defined with :
	//		params.desc.blendDesc.AlphaToCoverageEnable = false;
	//		params.desc.blendDesc.IndependentBlendEnable = false;
	//		params.desc.blendDesc.RenderTarget[0].BlendEnable = true;
	//	the difference is only the blend operation
	//========================================================================
	class BlendStateBinders{

		dx::BindOMBlendState m_cache[E_MAX_BLEND];

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		BlendStateBinders(){}
		BlendStateBinders( dx::Device * pDevice_p ){

			Init(pDevice_p);
		}

		//------------------------------------------------------------------------
		// "delayed" ctor
		//------------------------------------------------------------------------
		void Init( dx::Device * pDevice_p );

		//------------------------------------------------------------------------
		// core method that returns the cached blend state binder command
		//------------------------------------------------------------------------
		dx::BindOMBlendState & GetBlendBind( E_BLENDTYPE eBlend_p ){

			return m_cache[eBlend_p];
		}	
	};
}
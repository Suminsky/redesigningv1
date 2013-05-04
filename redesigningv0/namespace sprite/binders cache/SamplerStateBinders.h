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

// system/standard headers

//#include <boost/smart_ptr.hpp>

// private headers

#include "../../namespace gen/gen_data.h"
#include "../../namespace win/win_macros.h"

#include "../../namespace render/dx/Device.h"
#include "../../namespace render/dx/pipeline/Binder.h"

namespace sprite{

	enum E_SAMPLERTYPE{

		E_NONE = 0,
		E_LINEAR,
		E_MAX_SAMPLER
	};

	//========================================================================
	// cache samplers state commands : # slot 0
	//========================================================================
	class SamplerStateBinders{

		dx::BindPSSampler m_cache[E_MAX_SAMPLER];

	public:
		
		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SamplerStateBinders(){}
		SamplerStateBinders( dx::Device * pDevice_p ){

			Init(pDevice_p);
		}
		
		//------------------------------------------------------------------------
		// "delayed" ctor
		//------------------------------------------------------------------------
		void Init( dx::Device * pDevice_p );

		//------------------------------------------------------------------------
		// core method that returns the cached sampler state binder command
		//------------------------------------------------------------------------
		dx::BindPSSampler & GetSamplerBind( E_SAMPLERTYPE eBlend_p ){

			return m_cache[eBlend_p];
		}	
	};
}
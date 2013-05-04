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

#include <vector>
//#include <boost/smart_ptr.hpp>

// private headers

#include "../../namespace gen/gen_data.h"
#include "../../namespace win/win_macros.h"

#include "../../namespace render/dx/Device.h"
#include "../../namespace render/dx/pipeline/Binder.h"

namespace sprite{

	//========================================================================
	// cache srv state commands : # slot 0
	//========================================================================
	class TextureBinders{

		std::vector<dx::BindPSShaderResourceView> m_cache;
		std::vector< std::string > m_names;

		dx::Device * m_pDevice;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		TextureBinders(){}
		TextureBinders( dx::Device * pDevice_p ):m_pDevice(pDevice_p){}
	
		//------------------------------------------------------------------------
		// "delayed" ctor
		//------------------------------------------------------------------------
		void Init( dx::Device * pDevice_p ){

			m_pDevice = pDevice_p;
		}

		//------------------------------------------------------------------------
		// add new texture to the cache, given the texture filepath name
		//------------------------------------------------------------------------
		int Add( const char* szTexture_p );

		//------------------------------------------------------------------------
		// Get the texture by name, if name is not found, it adds a new texture
		// to the cache
		//------------------------------------------------------------------------
		dx::BindPSShaderResourceView & Get( const char* szTexture_p );

		//------------------------------------------------------------------------
		// get texture by index
		//------------------------------------------------------------------------
		dx::BindPSShaderResourceView & Get( UINT iTexture_p ){

			assert( iTexture_p < m_cache.size());

			return m_cache[iTexture_p];
		}

	};
}
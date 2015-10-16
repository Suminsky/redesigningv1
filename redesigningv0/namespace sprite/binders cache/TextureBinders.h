//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/03/31
	created:	31:3:2013   21:03
	filename: 	c:\Users\Gateway\Documents\Visual Studio 2010\Projects\redesigningv1\redesigningv0\Sprite.h
	file path:	c:\Users\Gateway\Documents\Visual Studio 2010\Projects\redesigningv1\redesigningv0
	file base:	Sprite
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	TODO: create a definitive cache class

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// system/standard headers

#include <vector>

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

	public:

		struct Resolution{ uint32_t w, h;};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		TextureBinders(){}
		TextureBinders( dx::Device * pDevice_p ):m_pDevice(pDevice_p){}
	
		//------------------------------------------------------------------------
		// "delayed" ctor
		//------------------------------------------------------------------------
		void Init( dx::Device * pDevice_p, int size_p = 100 ){
			m_cache.reserve(size_p);
			m_names.reserve(size_p);
			m_resolutions.reserve(size_p);
			m_size = size_p;

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
		dx::BindPSShaderResourceView & Get( const char* szTexture_p, int * pIndexID_p = nullptr );

		//------------------------------------------------------------------------
		// get texture by index
		//------------------------------------------------------------------------
		dx::BindPSShaderResourceView & Get( UINT iTexture_p ){

			assert( iTexture_p < m_cache.size());

			return m_cache[iTexture_p];
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		const std::string & GetTextureName( UINT iTexture_p ) const{

			assert( iTexture_p < m_cache.size());

			return m_names[iTexture_p];
		}
		const Resolution & GetTextureRes( UINT iTexture_p ) const{

			assert( iTexture_p < m_cache.size());

			return m_resolutions[iTexture_p];
		}

	private:

		std::vector<dx::BindPSShaderResourceView> m_cache;
		std::vector< std::string > m_names;
		std::vector< Resolution > m_resolutions;
		unsigned int m_size;

		dx::Device * m_pDevice;

	};
}
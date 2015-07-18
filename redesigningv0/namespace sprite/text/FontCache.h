#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2015/07/05
	created:	5:7:2015   23:31
	file:		FontCache.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../../namespace gen/gen_data.h"
#include "BmpFont_.h"

namespace sprite{

	class FontCache{

		enum{

			E_MAX_FONTS = 4,
			E_MAXGLYPHS = 2048
		};

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		FontCache( TextureBinders * pTextureCacheRef_p )
		:
		m_pTextureCacheRef(pTextureCacheRef_p){}

		//------------------------------------------------------------------------
		// returns ID
		//------------------------------------------------------------------------
		int Add( BmpFont_ & font_p, const char * szFont_p ){

			*m_names.StackAlloc() = szFont_p;
			*m_fonts.StackAlloc() = font_p;

			return m_fonts.Size() - 1;
		}

		//------------------------------------------------------------------------
		// Get font by name, if name is not found, it adds a new font
		// to the cache
		//------------------------------------------------------------------------
		BmpFont_ & Get( const char* szFont_p, uint32_t * pIndexID_p = nullptr );

		//------------------------------------------------------------------------
		// get texture by index
		//------------------------------------------------------------------------
		BmpFont_ & Get( uint32_t iFont_p ) {

			assert( iFont_p < m_fonts.Size());

			return m_fonts[iFont_p];
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		std::string & GetFontName( uint32_t iFont_p ){

			assert( iFont_p < m_names.Size());

			return m_names[iFont_p];
		}

	private:

		TextureBinders * m_pTextureCacheRef;

		gen::Stack<std::string, E_MAX_FONTS> m_names;
		gen::Stack<BmpFont_, E_MAX_FONTS> m_fonts;
		gen::Stack<GlyphIDUV,E_MAXGLYPHS> m_fontGlyphs;
	};
}
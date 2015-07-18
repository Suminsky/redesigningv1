#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2015/07/05
	created:	5:7:2015   11:31
	file:		BmpFont_.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../../namespace gen/gen_data.h"
#include "../../namespace text/CreateFontTexture.h"
#include "../binders cache/TextureBinders.h"

namespace sprite{
	
	struct GlyphUV{

		int x, y, w, h;
	};

	struct GlyphIDUV{

		wchar_t szChar;
		GlyphUV uv;
	};

	class BmpFont_{

	public:

		struct Formating{

			float fSpaceWidth;
			float fNewLineHeight;

			int iTabSpacing;
		};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		BmpFont_(){}
		BmpFont_( text::BmpFontDesc & fontDesc_p, GlyphIDUV * pGlyphMemBlock_p, sprite::TextureBinders & textureCache_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Initialize( text::BmpFontDesc & fontDesc_p, GlyphIDUV * pGlyphMemBlock_p, sprite::TextureBinders & textureCache_p );

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		const GlyphUV * GetGlyphUV( const wchar_t id ) const;

		float GetSpaceWidth() const { return m_formating.fSpaceWidth; }
		float GetMinNewLineHeight() const { return m_formating.fNewLineHeight; }
		int GetTabSpacing() const { return m_formating.iTabSpacing; }

		int GetTextureWidth()  const { return m_iTextureW; }
		int GetTextureHeight() const { return m_iTextureH; }
		int GetTextureID() const {return m_iTextureID;}
		dx::BindPSShaderResourceView * GetTextureBinder() const { return m_pTextureSRVBinder; }

	private:

		Formating m_formating;
		GlyphUV m_notSupportedGlyph;
		
		int m_iTextureID;
		int m_iTextureW, m_iTextureH;
		dx::BindPSShaderResourceView * m_pTextureSRVBinder;

		GlyphIDUV * m_pGlyphsMemBlock;
		uint32_t m_nChars;
	};
}
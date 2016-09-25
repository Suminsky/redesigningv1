#include "PCHF.h"
#include "BmpFont_.h"

#include <algorithm>

using namespace sprite;
using namespace text;


sprite::BmpFont_::BmpFont_( BmpFontDesc & fontDesc_p, GlyphIDUV * pGlyphMemBlock_p, sprite::TextureBinders & textureCache_p )
{

	Initialize(fontDesc_p, pGlyphMemBlock_p, textureCache_p);
}



void sprite::BmpFont_::Initialize( BmpFontDesc & fontDesc_p, GlyphIDUV * pGlyphMemBlock_p, sprite::TextureBinders & textureCache_p )
{
	// formatting

	m_formating.fSpaceWidth = fontDesc_p.fSpaceWidth;
	m_formating.fNewLineHeight = fontDesc_p.fNewLineMinHeight;
	m_formating.iTabSpacing = 32;

	// TODO:
	m_notSupportedGlyph.w = m_iTextureW;
	m_notSupportedGlyph.h = m_iTextureH;
	m_notSupportedGlyph.x = 0;
	m_notSupportedGlyph.y = 0;

	// UVs

	m_nChars = fontDesc_p.nCharacteres;
	m_pGlyphsMemBlock = pGlyphMemBlock_p;
	for( int it = 0; it < fontDesc_p.nCharacteres; ++it ){

		m_pGlyphsMemBlock[it].szChar = fontDesc_p.characteresIDs[it];
		m_pGlyphsMemBlock[it].uv = *((GlyphUV*)&fontDesc_p.charUVRects[it]);
	}

	// sort vector based on wchar value
	auto comp = []( const GlyphIDUV & a, const GlyphIDUV & b  ){

		return a.szChar < b.szChar;
	};
	std::sort( m_pGlyphsMemBlock, &m_pGlyphsMemBlock[m_nChars-1], comp );

	// texture

	m_iTextureW = fontDesc_p.iTextureWidth;
	m_iTextureH = fontDesc_p.iTextureHeight;

	m_pTextureSRVBinder = &textureCache_p.Get(fontDesc_p.szTextureFilename, &m_iTextureID);
	//m_pipeState.AddBinderCommand( m_pTextureSRVBinder );
}

const sprite::GlyphUV * sprite::BmpFont_::GetGlyphUV( const wchar_t id ) const
{
	auto comp = []( const GlyphIDUV & a,  const wchar_t b  ){

		return   a.szChar < b;
	};
	
	GlyphIDUV * pGlyph = std::lower_bound( m_pGlyphsMemBlock, &m_pGlyphsMemBlock[m_nChars-1], id, comp );

	if( pGlyph->szChar == id ){

		return &pGlyph->uv;
	}
	else{

		// return not supported char glyph

		return &m_notSupportedGlyph;
	}
}

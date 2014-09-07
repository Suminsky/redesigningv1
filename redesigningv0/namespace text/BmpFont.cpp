#include "BmpFont.h"
#include <algorithm>

using namespace text;

void text::BmpFont::InitFromDesc( BmpFontDesc & fontDesc_p, sprite::TextureBinders & textureCache_p )
{
	m_fSpaceWidth = fontDesc_p.fSpaceWidth;
	m_fNewLineMinHeight = fontDesc_p.fNewLineMinHeight;
	m_iTextureW = fontDesc_p.iTextureWidth;
	m_iTextureH = fontDesc_p.iTextureHeight;

	m_tabMultiple = 32; // TODO, read from file

	// TODO:
	m_notSupportedGlyph.Width = (float)m_iTextureW;
	m_notSupportedGlyph.Height = (float)m_iTextureH;
	m_notSupportedGlyph.X = 0.0f;
	m_notSupportedGlyph.Y = 0.0f;

	m_vGlyphUVs.resize( fontDesc_p.nCharacteres );
	for( int it = 0; it < fontDesc_p.nCharacteres; ++it ){

		GlyphIDRect newGlyph = { fontDesc_p.characteresIDs[it], fontDesc_p.charUVRects[it] };
		m_vGlyphUVs.push_back( newGlyph );
	}

	// sort vector based on wchar value
	auto comp = []( const GlyphIDRect & a, const GlyphIDRect & b  ){

		return a.szChar < b.szChar;
	};
	std::sort( m_vGlyphUVs.begin(), m_vGlyphUVs.end(), comp );

	// get texture

	m_pTextureSRVBinder = &textureCache_p.Get(fontDesc_p.szTextureFilename, &m_iTextureID);
	m_pipeState.AddBinderCommand( m_pTextureSRVBinder );
}

bool text::BmpFont::InitFromFile( const char * szFontDescFilename_p, sprite::TextureBinders & textureCache_p )
{
	BmpFontDesc fontDesc;
	if( ReadFontDescFromFile( szFontDescFilename_p, fontDesc ) == true ){

		InitFromDesc(fontDesc, textureCache_p);

		delete fontDesc.characteresIDs;
		delete fontDesc.charUVRects;
		delete fontDesc.szTextureFilename;

		return true;
	}

	return false;
}

text::GlyphRect text::BmpFont::GetGlyphUV( const wchar_t id ) const
{
	auto comp = []( const GlyphIDRect & a,  const wchar_t b  ){

		return   a.szChar < b;
	};

	std::vector<GlyphIDRect>::const_iterator itEnd = m_vGlyphUVs.end();
	std::vector<GlyphIDRect>::const_iterator it =
		std::lower_bound( m_vGlyphUVs.begin(), itEnd, id, comp );

	if( it != itEnd ){

		return it->rect;
	}
	else{

		// return not supported char glyph
		return m_notSupportedGlyph;
	}
}

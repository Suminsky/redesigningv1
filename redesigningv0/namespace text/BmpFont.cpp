#include "BmpFont.h"

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

	for( int it = 0; it < fontDesc_p.nCharacteres; ++it ){

		m_glyphUVs[ fontDesc_p.characteresIDs[it] ] = fontDesc_p.charUVRects[it];
	}

	// get texture

	m_pTextureSRVBinder = &textureCache_p.Get(fontDesc_p.szTextureFilename, &m_iTextureID);
	m_pipeState.AddBinderCommand( m_pTextureSRVBinder );
	BREAKHERE;
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
	GlyphMap::const_iterator element = m_glyphUVs.find( id );
	if( element != m_glyphUVs.end() ){

		return element->second;
	}
	else{

		// return not supported char glyph
		return m_notSupportedGlyph;
	}
}

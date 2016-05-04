#include "SpriteTextComposer.h"
#include "../../namespace game/components/SpriteTextCompo.h"

using namespace sprite;
using namespace game;

uint32_t sprite::SpriteTextComposer::Compose( wchar_t * szText_p, game::spriteInstance * pInstances_p, uint32_t maxChars_p, uint32_t fontID_p )
{
	BmpFont_ & font = m_fonts.Get(fontID_p);

	float xPos = 0.0f, yPos = 0.0f;

	float fCharSpacing = 1.0f; // TODO, get ths from somewhere

	uint32_t itDraw = 0;
	for( uint32_t it = 0; szText_p != 0x00 && it < maxChars_p; ++it ){

		// handle spaces

		switch( szText_p[it] ){

		case ' ': xPos += font.GetSpaceWidth();											continue;
		case '\n': xPos = 0.0f; yPos -= (font.GetMinNewLineHeight() + fCharSpacing);	continue;
		case '\t': 
			float tab = (float)font.GetTabSpacing();
			xPos = ((xPos + tab) / tab) * tab;
			//xPos = tab - (xPos % tab ); 
			continue;
		}

		game::spriteInstance & charInst = pInstances_p[itDraw++];

		// copy glyph uv to instance uv rect

		*( (GlyphUV*)(&charInst.uvRect) ) =  *font.GetGlyphUV( szText_p[it] ); // dang!

		// compute glyph width from uv percentage

		charInst.res[0] = (float)((int)(charInst.uvRect[2] * (float)font.GetTextureWidth()));
		charInst.res[1] = (float)((int)(charInst.uvRect[3] * (float)font.GetTextureHeight()));

		// pos
		// need to consider current and previous char half w
		
		float halfW = charInst.res[0] * 0.5f;
		xPos += halfW;

		charInst.padding[0] = xPos;
		charInst.padding[1] = yPos;

		xPos += halfW + fCharSpacing;
	}

	return itDraw;
}

uint32_t sprite::SpriteTextComposer::Compose( wchar_t * szText_p, game::spriteInstance * pInstances_p, uint32_t maxChars_p, uint32_t fontID_p, float & width_p, float & height_p )
{
	BmpFont_ & font = m_fonts.Get(fontID_p);

	float xPos = 0.0f,
		yPos = -font.GetMinNewLineHeight()*0.5f; // text anchor at the top of the first char (top left, check bellow)
	width_p = 0.0f; height_p = 0.0f;
	float lineW = 0.0f;
	uint32_t nLines = 1;

	float fCharSpacing = 1.0f; // TODO, get this from somewhere

	uint32_t itDraw = 0;
	for( uint32_t it = 0; szText_p[it] != 0x0000 && it < maxChars_p; ++it ){

		// handle spaces

		switch( szText_p[it] ){

		case L' ': xPos += font.GetSpaceWidth();											continue;
		case L'\n':
			xPos = 0.0f; yPos -= (font.GetMinNewLineHeight() + fCharSpacing);

			if( lineW > width_p ) width_p = lineW;
			lineW = 0.0f;

			++nLines;

			continue;
		case L'\t': 
			float tab = (float)font.GetTabSpacing();
			xPos = ((xPos + tab) / tab) * tab;
			//xPos = tab - (xPos % tab ); 
			continue;
		}

		game::spriteInstance & charInst = pInstances_p[itDraw++];

		// copy glyph uv to instance uv rect

		*( (GlyphUV*)(&charInst.uvRect) ) =  *font.GetGlyphUV( szText_p[it] ); // dang!

		// compute glyph width from uv percentage

		charInst.res[0] = (float)((int)(charInst.uvRect[2] * (float)font.GetTextureWidth()));
		charInst.res[1] = (float)((int)(charInst.uvRect[3] * (float)font.GetTextureHeight()));

		// line width

		lineW = xPos + charInst.res[0];

		// pos
		// need to consider current and previous char half w

		float halfW = charInst.res[0] * 0.5f;
		xPos += halfW;

		charInst.padding[0] = xPos;
		charInst.padding[1] = yPos;

		xPos += halfW + fCharSpacing;
	}

	if( lineW > width_p ) width_p = lineW;

	height_p = font.GetMinNewLineHeight() * nLines;

	return itDraw;
}

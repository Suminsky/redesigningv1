#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/09
	created:	9:7:2013   10:57
	file:		BmpFont.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	TODO compare map vs sorted vector performance

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <map>
#include "../namespace render/dx/Device.h"

// private includes
#include "CreateFontTexture.h"
#include "../namespace sprite/binders cache/TextureBinders.h"

namespace text{


	class BmpFont{

		typedef std::map<WCHAR, GlyphRect> GlyphMap;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		BmpFont(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void InitFromDesc( BmpFontDesc & fontDesc_p, sprite::TextureBinders & textureCache_p ){

			m_fSpaceWidth = fontDesc_p.fSpaceWidth;
			m_fNewLineMinHeight = fontDesc_p.fNewLineMinHeight;
			//m_iTextureW = 
			//m_iTextureH = 

			for( int it = 0; it < fontDesc_p.nCharacteres; ++it ){

				m_glyphUVs[ fontDesc_p.characteresIDs[it] ] = fontDesc_p.charUVRects[it];
			}

			// get texture

			m_textureSRVBinder.reset( &textureCache_p.Get(fontDesc_p.szTextureFilename), &gen::NoOp<dx::Binder>  );
		}
		bool InitFromFile( const char * szFontDescFilename_p, sprite::TextureBinders & textureCache_p ){

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

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		float GetSpaceWidth()const{ return m_fSpaceWidth; }
		float GetMinNewLineHeight()const{ return m_fNewLineMinHeight; }
		GlyphRect GetGlyphUV( WCHAR id )const{

			 GlyphMap::const_iterator element = m_glyphUVs.find( id );
			 if( element != m_glyphUVs.end() ){

				 return element->second;
			 }
			 else{

				 // return not supported char glyph
				return m_notSupportedGlyph;
			 }
		}
		dx::shared_Binder_ptr & GetTextureBinder(){ return m_textureSRVBinder; }


	private:

		float m_fSpaceWidth;
		float m_fNewLineMinHeight;
		int m_iTextureW, m_iTextureH;
		GlyphRect m_notSupportedGlyph;

		dx::shared_Binder_ptr m_textureSRVBinder;
		GlyphMap m_glyphUVs;
	};

	typedef std::shared_ptr<BmpFont> shared_BmpFont_ptr;
	typedef std::weak_ptr<BmpFont> weak_BmpFont_ptr;
}
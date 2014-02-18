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

		typedef std::map<wchar_t, GlyphRect> GlyphMap;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		BmpFont(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void InitFromDesc( BmpFontDesc & fontDesc_p, sprite::TextureBinders & textureCache_p );
		bool InitFromFile( const char * szFontDescFilename_p, sprite::TextureBinders & textureCache_p );

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		GlyphRect GetGlyphUV( const wchar_t id )const;
		float GetSpaceWidth()const{ return m_fSpaceWidth; }
		float GetMinNewLineHeight()const{ return m_fNewLineMinHeight; }
		int GetTabMultiple() const { return m_tabMultiple; }
		dx::BindPSShaderResourceView *& GetTextureBinder(){ return m_pTextureSRVBinder; }
		dx::PipeState & GetPipeState(){ return m_pipeState; }
		int GetTextureWidth()  const{ return m_iTextureW; }
		int GetTextureHeight() const{ return m_iTextureH; }
		int GetTextureID() const {return m_iTextureID;}


	private:

		float m_fSpaceWidth;
		float m_fNewLineMinHeight;
		int m_tabMultiple; // todo, save on file
		int m_iTextureW, m_iTextureH;
		GlyphRect m_notSupportedGlyph;

		dx::BindPSShaderResourceView * m_pTextureSRVBinder;
		dx::PipeState	m_pipeState;
		int m_iTextureID;

		GlyphMap m_glyphUVs;
	};

	typedef std::shared_ptr<BmpFont> shared_BmpFont_ptr;
	typedef std::weak_ptr<BmpFont> weak_BmpFont_ptr;
}
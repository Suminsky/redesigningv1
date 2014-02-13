#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/09
	created:	9:7:2013   12:19
	file:		TextRenderer.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	� Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "BmpFont.h"
#include "../namespace sprite/SpriteRenderer.h"

namespace text{

	struct Box{

		float x, y, w, h;
		float carretPos;
	};

	//------------------------------------------------------------------------
	// holds data common to all glyphs
	//------------------------------------------------------------------------
	struct GlyphRenderData{
		
		sprite::DrawableCbuffer cbufferData;
		sprite::BindVSDrawableCBuffer bindDrawableCbuffer;
		
		dx::PipeState state;
	};
	struct DrawableGlyph{

		GlyphRenderData renderData; // data used by the drawable
		render::Drawable drawable;

		void Clear(){
			renderData.state.Reset();
			drawable.Clear();
		}
	};

	typedef std::vector<DrawableGlyph> DrawableText;

	//========================================================================
	// 
	//========================================================================
	class TextRenderer{

		enum{
			E_MAXGLYPHS = 1024
		};

		typedef std::vector<BmpFont> Fonts;

		struct GlyphRenderBuffer{ // due a bug on std, vector wont align data, this is fixed on vs2012

			int m_currentIndex;
			GlyphRenderData m_renderDatas[E_MAXGLYPHS];
			

			//------------------------------------------------------------------------
			// 
			//------------------------------------------------------------------------
			void clear(){

				m_currentIndex = 0;
			}

			//------------------------------------------------------------------------
			// 
			//------------------------------------------------------------------------
			GlyphRenderData & operator[]( unsigned int index_p ){

				assert( index_p < E_MAXGLYPHS );
				return m_renderDatas[index_p];
			}
			void operator++( int ){// the int param means postfix

				++m_currentIndex;
				if( m_currentIndex == E_MAXGLYPHS )
					m_currentIndex = 0;
			}
		};

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		TextRenderer( dx::Device * pDevice_p, sprite::SpriteRenderer & spriteRenderer_p )
			:
		m_pSpriteRendererRef(&spriteRenderer_p)
		{
			Initialize( pDevice_p, spriteRenderer_p );
		}

		TextRenderer(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Initialize( dx::Device * pDevice_p, sprite::SpriteRenderer & spriteRenderer_p );

		//------------------------------------------------------------------------
		// returns font id, the only way to specify the font
		//------------------------------------------------------------------------
		int AddFont( BmpFont & font_p ){

			m_fonts.emplace_back( font_p );

			return (int)(m_fonts.size()-1);
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void RenderText( DrawableGlyph pDrawables_p[], int nDrawables_p ){

			for( int it = 0; it < nDrawables_p; ++it ){

				m_pSpriteRendererRef->m_queue.Submit( pDrawables_p[it].drawable );
			}
		}

		//------------------------------------------------------------------------
		// SLOW, this creates the drawables and submit then to the drawablesqueue.
		// MAX of E_MAXGLYPHS, as render data is buffered in this object.
		//------------------------------------------------------------------------
		void RenderText( const wchar_t szText_p[], DirectX::XMFLOAT4 pos_p, sprite::Camera & camera_p,UINT iFontID_p = 0 );


		//------------------------------------------------------------------------
		// Returns an array of drawable text that can be used to render text.
		// nDrawables have both max glyphs to create, and return number of glyphs
		// used.
		//------------------------------------------------------------------------
		void Draw_Text(  const wchar_t szText_p[], DirectX::XMFLOAT4 pos_p, UINT iFontID_p,
						DrawableGlyph * pDrawableText_p, int & nDrawables_p,
						sprite::Camera & camera_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		int ComputeNumberOfGlyphs( const wchar_t szText_p[] );

	private:

		Fonts m_fonts;

		GlyphRenderBuffer m_glyphsRenderData;

		render::vStatePtrs m_textRenderStates;
		dx::DrawCall * m_pTextDrawCall;
		dx::PipeState m_textureState;

		ID3D11Buffer *m_pGlyphBufferInterface;

		sprite::SpriteRenderer * m_pSpriteRendererRef;	// pointer so that it can change to which "sink" to flush

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static UINT CountWString( const wchar_t * szString_p )
		{
			int counter = 0;

			while( szString_p[counter] != 0x0000 ){

				++counter;
				assert( counter < 2048 );
			}

			return counter;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void InitializeGlyphs(){

			m_glyphsRenderData.clear();

			for( int it = 0; it < E_MAXGLYPHS; ++it ){
				
				// initialize bind cbuffer command

				m_glyphsRenderData.m_renderDatas[it].bindDrawableCbuffer.
				Initialize( m_pGlyphBufferInterface, &m_glyphsRenderData.m_renderDatas[it].cbufferData );

				// initialize state with binder command

				m_glyphsRenderData.m_renderDatas[it].state.AddBinderCommand( &m_glyphsRenderData.m_renderDatas[it].bindDrawableCbuffer );
			}
		}
	};

	typedef std::shared_ptr<TextRenderer> shared_TextRenderer_ptr;
	typedef std::weak_ptr<TextRenderer> weak_TextRenderer_ptr;
}
#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/09
	created:	9:7:2013   12:19
	file:		TextRenderer.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
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
	// holds data commom to all glyphs
	//------------------------------------------------------------------------
	//struct TextRenderingState{

	//	dx::shared_State_ptr pVertexRelatedBinds; // vertices, input layout

	//	dx::shared_Binder_ptr pShaderBind;

	//	dx::shared_Binder_ptr pAlphaBlendBind;

	//	dx::shared_Binder_ptr pLinearSamplingBind;
	//	dx::shared_Binder_ptr pNoSamplingBind;
	//};
	//struct RenderableGlyph{
	//	dx::State m_pipeState; // cbuffer only
	//	sprite::DrawableCbuffer m_renderData;
	//};
	struct GlyphRenderData{
		sprite::BindVSDrawableCBuffer bindDrawableCbuffer;
		sprite::DrawableCbuffer cbufferData;
		dx::State state;
	};

	class TextRenderer{

		typedef std::vector<BmpFont> Fonts;
		typedef std::vector<GlyphRenderData> GlyphsRenderData;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		TextRenderer( dx::Device * pDevice_p, sprite::SpriteRenderer & spriteRenderer_p)
			:
		m_pSpriteRendererRef(&spriteRenderer_p)
		{
			// creates a ID3DBuffer for the vs constant buffer

			dx::BufferResource::CreationParams cbufferParams;
			ZeroMemory(&cbufferParams, sizeof(dx::BufferResource::CreationParams));
			cbufferParams.desc.bufferDesc.ByteWidth = sprite::DrawableCbuffer::s_SIZE;
			cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbufferParams.desc.bufferDesc.CPUAccessFlags = 0;
			cbufferParams.desc.bufferDesc.StructureByteStride = 0;
			cbufferParams.desc.bufferDesc.MiscFlags = 0;

			m_pGlyphBufferInterface = NULL;
			pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, m_pGlyphBufferInterface );

			// creates the render state holding binds commom to all glyphs
			// caralho d referencias, nao posso mudar essas porra, pq sao usadas em otros lugares
			// TODO: TEM q t um cache pra todos os binders usados no jogo
			
			m_textRenderStates.push_back( dx::shared_State_ptr(&m_pSpriteRendererRef->m_defaultVertexInput, &gen::NoOp<dx::State>) );
			m_textRenderStates.push_back( m_pSpriteRendererRef->m_spriteShaderRes.m_permutations[0].m_pPipeState );
			m_textRenderStates.push_back( dx::shared_State_ptr(&m_pSpriteRendererRef->m_camera.m_pipeState, &gen::NoOp<dx::State>) );

			m_textDrawCall.reset( &m_pSpriteRendererRef->m_drawIndexed, &gen::NoOp<dx::DrawCall> );
		}

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
		void Render(){

			/*for( int it = 0, size = (int)m_drawableGlyphs.size(); it < size; ++it ){

				m_pSpriteRendererRef->m_queue.Submit( &m_drawableGlyphs[it] );
			}*/
		}

		void DrawText( const WCHAR * szText_p, DirectX::XMFLOAT4 pos_p, UINT iFontID = 0 ){

			//m_drawableGlyphs.clear();
			m_glyphsRenderData.clear();
			m_textureState.m_binds.clear();m_textureState.m_stateMask = 0;

			// NOTE:  everything is preserved, binders of texture, shaders, etc., those came from
			// other objects that will only release on destruction.
			// The only thing created here is for glyph data, those need to be preserved till rendering
			// is done, so thats why theres an array of glyphrenderdata, this array stores drawable(cbuffer)
			// data and its binder command.
			
			int nGlyphs = CountWString(szText_p);
			//m_drawableGlyphs.reserve(nGlyphs);
			m_glyphsRenderData.reserve(nGlyphs);
			

			// create a drawable for all glyphs

			render::Drawable drawable;
			sprite::SortMask sorMask = {0};
			sorMask.bitField.layer = 3;
			drawable.m_sortKey = sorMask.intRepresentation;//0LL;//drawable.m_sortKey = 1;
			

			
			drawable.m_pipeStatesGroup = m_textRenderStates; // common data
			drawable.m_pDrawCall = m_textDrawCall;

			// render data

			m_textureState.AddBinderCommand( m_fonts[iFontID].GetTextureBinder() ); // texture used for that text
			m_textureState.AddBinderCommand(
				dx::shared_Binder_ptr( &m_pSpriteRendererRef->m_samplers.GetSamplerBind(sprite::E_NONE), &gen::NoOp<dx::Binder>)
				);
			m_textureState.AddBinderCommand(
				dx::shared_Binder_ptr( &m_pSpriteRendererRef->m_blends.GetBlendBind(sprite::E_ALPHA_BLENDED), &gen::NoOp<dx::Binder>));

			// add to drawable

			drawable.m_pipeStatesGroup.push_back( dx::shared_State_ptr( &m_textureState, &gen::NoOp<dx::State>) );
			
			//

			GlyphRenderData glyphRenderData;

			// color (common to all glyphs, but per drawable too)
			
			glyphRenderData.cbufferData.m_color.x = //0.5f;
			glyphRenderData.cbufferData.m_color.y = 
			glyphRenderData.cbufferData.m_color.z = 
			glyphRenderData.cbufferData.m_color.w = 1.0f;

			
			UINT itChar = 0;
			float fPosOffsetX = 0.0f;
			float fPosOffsetY = 0.0f;
			while( szText_p[itChar] != 0x0000 ){

				// handle spaces

				if( szText_p[itChar] == L' ' ){

					fPosOffsetX += m_fonts[iFontID].GetSpaceWidth();
					++itChar;
					continue;
				}
				else if(szText_p[itChar] == L'\n' ){

					fPosOffsetY -= m_fonts[iFontID].GetMinNewLineHeight() + 1.0f;
					fPosOffsetX = 0.0f;
					++itChar;
					continue;
				}


				// set data specific to each glyph

				GlyphRect uv = m_fonts[iFontID].GetGlyphUV( szText_p[itChar] );

				//res
				glyphRenderData.cbufferData.m_res.x = uv.Width * 1024;
				glyphRenderData.cbufferData.m_res.y = uv.Height * 74.0f;
				float halfWidth = glyphRenderData.cbufferData.m_res.x * 0.5f;
				// pos
				DirectX::XMFLOAT4 pos = pos_p;
				fPosOffsetX += halfWidth;
				pos.x += fPosOffsetX;
				pos.y += fPosOffsetY;
				//pos_p.x += fPosOffsetX;
				glyphRenderData.cbufferData.m_mWorld = DirectX::XMMatrixIdentity();
				glyphRenderData.cbufferData.m_mWorld.r[3] =  DirectX::XMLoadFloat4( &pos );
				fPosOffsetX += halfWidth + 1.0f;

				// uv
				glyphRenderData.cbufferData.m_uvRect = DirectX::XMFLOAT4( uv.X, uv.Y, uv.Width, uv.Height );

				// bind cbuffer command

				glyphRenderData.bindDrawableCbuffer.m_pConstantBuffer = m_pGlyphBufferInterface;

				m_glyphsRenderData.push_back( glyphRenderData ); // save glyph render data

				m_glyphsRenderData[m_glyphsRenderData.size()-1].bindDrawableCbuffer.m_pConstantBufferData.reset( &m_glyphsRenderData[m_glyphsRenderData.size()-1].cbufferData, &gen::NoOp<sprite::DrawableCbuffer> );
				
				// create render state for this glyph

				m_glyphsRenderData[m_glyphsRenderData.size()-1].state.AddBinderCommand( dx::shared_Binder_ptr( &m_glyphsRenderData[m_glyphsRenderData.size()-1].bindDrawableCbuffer, &gen::NoOp<dx::Binder> ) );
				
				drawable.m_pipeStatesGroup.push_back( dx::shared_State_ptr(&m_glyphsRenderData[m_glyphsRenderData.size()-1].state, &gen::NoOp<dx::State> ) );

				//m_drawableGlyphs.push_back( drawable ); // add to list of glyphs to render
				m_pSpriteRendererRef->m_queue.Submit( &drawable );

				++itChar;

				// the state used here have data specific to the current glyph, remove it:
				drawable.m_pipeStatesGroup.pop_back();
			}
		}
		

	private:

		Fonts m_fonts;

		GlyphsRenderData m_glyphsRenderData;
		//std::vector<render::Drawable> m_drawableGlyphs;

		dx::StateGroup m_textRenderStates;
		dx::shared_DrawCall_ptr m_textDrawCall;
		dx::State m_textureState;

		ID3D11Buffer *m_pGlyphBufferInterface;

		sprite::SpriteRenderer * m_pSpriteRendererRef;	// pointer so that it can change to which "sink" to flush

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static UINT CountWString( const WCHAR * szString_p )
		{
			int counter = 0;

			while( szString_p[counter] != 0x0000 ){

				++counter;
				assert( counter < 2048 );
			}

			return counter;
		}
	};

	typedef std::shared_ptr<TextRenderer> shared_TextRenderer_ptr;
	typedef std::weak_ptr<TextRenderer> weak_TextRenderer_ptr;
}
#include "TextRenderer.h"

using namespace text;

void text::TextRenderer::Initialize( dx::Device * pDevice_p, sprite::SpriteRenderer & spriteRenderer_p )
{
	m_pSpriteRendererRef = &spriteRenderer_p;

	// creates a ID3DBuffer for the vs constant buffer

	dx::BufferResource::CreationParams cbufferParams;
	ZeroMemory(&cbufferParams, sizeof(dx::BufferResource::CreationParams));
	cbufferParams.desc.bufferDesc.ByteWidth = sprite::DrawableCbuffer::s_SIZE;
	cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT; //
	cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferParams.desc.bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //0; //
	cbufferParams.desc.bufferDesc.StructureByteStride = 0;
	cbufferParams.desc.bufferDesc.MiscFlags = 0;

	m_pGlyphBufferInterface = NULL;
	pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, m_pGlyphBufferInterface );
	//pDevice_p->GetDevice()->CreateBuffer( &cbufferParams.desc.bufferDesc, nullptr, &m_pGlyphBufferInterface );
	
	// !!!!!!!!!!!!! TODO NOW !!!!!!!!!!!!!!!!!!!!
	// using another buffer interface is the problem, not the buffer config, but another interface ptr

	// creates the render state holding binds commom to all glyphs
	// caralho d referencias, nao posso mudar essas porra, pq sao usadas em otros lugares
	// TODO: TEM q t um cache pra todos os binders usados no jogo

	m_textRenderStates.push_back( &m_pSpriteRendererRef->m_defaultVertexInput );
	m_textRenderStates.push_back( &m_pSpriteRendererRef->m_spriteShaderRes.m_permutations[0].m_pipeState );
	m_textRenderStates.push_back( &m_pSpriteRendererRef->m_camera.m_pipeState );

	m_textureState.AddBinderCommand( &m_pSpriteRendererRef->m_samplers.GetSamplerBind(sprite::E_SAMPLER_NONE) );
	m_textureState.AddBinderCommand( &m_pSpriteRendererRef->m_blends.GetBlendBind(sprite::E_BLEND_ALPHA_BLENDED) );

	m_pTextDrawCall = &m_pSpriteRendererRef->m_drawIndexed;

	InitializeGlyphs();
}


void text::TextRenderer::RenderText( const WCHAR szText_p[], DirectX::XMFLOAT4 pos_p, UINT iFontID_p /*= 0 */ )
{
	if( szText_p[0] == 0 ) return;
	//

	sprite::DrawableCbuffer glyphCBuffer;

	// color (common to all glyphs, but per drawable too)

	glyphCBuffer.m_color.x = //0.5f;
	glyphCBuffer.m_color.y = 
	glyphCBuffer.m_color.z = 
	glyphCBuffer.m_color.w = 1.0f;

	glyphCBuffer.m_mWorld = DirectX::XMMatrixIdentity();

	UINT itChar = 0;
	float fPosOffsetX = 0.0f;
	float fPosOffsetY = 0.0f;

	render::Drawable drawable;
	sprite::SortMask sorMask = {0};
	sorMask.bitfield.layer = 3;
	sorMask.bitfield.textureID = m_fonts[iFontID_p].GetTextureID();
	sorMask.bitfield.transparency = sprite::E_BLEND_ALPHA_BLENDED;

	drawable.SetSortKey( sorMask.intRepresentation );
	drawable.SetDrawCall( m_pTextDrawCall );

	drawable.SetPipelineStateGroup( m_textRenderStates ); // common data (vertex, IA, etc)
	drawable.AddPipelineState( &m_textureState );
	drawable.AddPipelineState( &m_fonts[iFontID_p].GetPipeState() );

	while( szText_p[itChar] != 0x0000 ){

		// handle spaces

		if( szText_p[itChar] == L' ' ){

			fPosOffsetX += m_fonts[iFontID_p].GetSpaceWidth();
			++itChar;
			continue;
		}
		else if(szText_p[itChar] == L'\n' ){

			fPosOffsetY -= m_fonts[iFontID_p].GetMinNewLineHeight() + 1.0f;
			fPosOffsetX = 0.0f;
			++itChar;
			continue;
		}

		// set data specific to each glyph

		GlyphRect uv = m_fonts[iFontID_p].GetGlyphUV( szText_p[itChar] );

		//res
		glyphCBuffer.m_res.x = uv.Width * (float)m_fonts[iFontID_p].GetTextureWidth();
		glyphCBuffer.m_res.y = uv.Height * (float)m_fonts[iFontID_p].GetTextureHeight();

		float halfWidth = glyphCBuffer.m_res.x * 0.5f;

		// pos
		DirectX::XMFLOAT4 pos = pos_p;

		fPosOffsetX += halfWidth;

		pos.x += fPosOffsetX;
		pos.y += fPosOffsetY;

		glyphCBuffer.m_mWorld.r[3] = DirectX::XMLoadFloat4( &pos );

		fPosOffsetX += halfWidth + 1.0f;

		// uv
		glyphCBuffer.m_uvRect = DirectX::XMFLOAT4( uv.X, uv.Y, uv.Width, uv.Height );

		// glyph state ( bind cbuffer and texture ) should remain, so add to buffer here

		m_glyphsRenderData[m_glyphsRenderData.m_currentIndex].cbufferData = glyphCBuffer;

		drawable.AddPipelineState( &m_glyphsRenderData[m_glyphsRenderData.m_currentIndex].state );

		//m_pSpriteRendererRef->m_queue.Submit( std::move(drawable) );
		m_pSpriteRendererRef->m_queue.Submit( drawable );

		drawable.PopLastPipelineState();

		++itChar;
		m_glyphsRenderData++;
	}
}

void text::TextRenderer::DrawText( const WCHAR szText_p[], DirectX::XMFLOAT4 pos_p, UINT iFontID_p,
									DrawableGlyph * pDrawableText_p, int & nDrawables_p )
{
	if( szText_p[0] == 0 ) return;

	UINT itChar = 0;
	float fPosOffsetX = 0.0f;
	float fPosOffsetY = 0.0f;

	sprite::SortMask sorMask = {0};
	sorMask.bitfield.layer = 3;
	sorMask.bitfield.textureID = m_fonts[iFontID_p].GetTextureID();
	sorMask.bitfield.transparency = sprite::E_BLEND_ALPHA_BLENDED;
	//__int64 llsortKey = sorMask.intRepresentation;

	int itGlyph = 0;
	while( szText_p[itChar] != 0x0000 ){

		assert( itGlyph != nDrawables_p );

		// handle spaces

		if( szText_p[itChar] == L' ' ){

			fPosOffsetX += m_fonts[iFontID_p].GetSpaceWidth();
			++itChar;
			continue;
		}
		else if(szText_p[itChar] == L'\n' ){

			fPosOffsetY -= m_fonts[iFontID_p].GetMinNewLineHeight() + 1.0f;
			fPosOffsetX = 0.0f;
			++itChar;
			continue;
		}

		pDrawableText_p[itGlyph].renderData.cbufferData.m_color.x = //0.5f;
		pDrawableText_p[itGlyph].renderData.cbufferData.m_color.y = 
		pDrawableText_p[itGlyph].renderData.cbufferData.m_color.z = 
		pDrawableText_p[itGlyph].renderData.cbufferData.m_color.w = 1.0f;
		pDrawableText_p[itGlyph].renderData.cbufferData.m_mWorld = DirectX::XMMatrixIdentity();

		// set data specific to each glyph

		GlyphRect uv = m_fonts[iFontID_p].GetGlyphUV( szText_p[itChar] );

		//res
		pDrawableText_p[itGlyph].renderData.cbufferData.m_res.x = uv.Width * (float)m_fonts[iFontID_p].GetTextureWidth();
		pDrawableText_p[itGlyph].renderData.cbufferData.m_res.y = uv.Height * (float)m_fonts[iFontID_p].GetTextureHeight();

		float halfWidth = pDrawableText_p[itGlyph].renderData.cbufferData.m_res.x * 0.5f;

		// pos
		DirectX::XMFLOAT4 pos = pos_p;

		fPosOffsetX += halfWidth;

		pos.x += fPosOffsetX;
		pos.y += fPosOffsetY;

		pDrawableText_p[itGlyph].renderData.cbufferData.m_mWorld.r[3] = DirectX::XMLoadFloat4( &pos );

		fPosOffsetX += halfWidth + 1.0f;

		// uv
		pDrawableText_p[itGlyph].renderData.cbufferData.m_uvRect = DirectX::XMFLOAT4( uv.X, uv.Y, uv.Width, uv.Height );

		//
		pDrawableText_p[itGlyph].renderData.bindDrawableCbuffer.Initialize( m_pGlyphBufferInterface, &pDrawableText_p[itGlyph].renderData.cbufferData );
		pDrawableText_p[itGlyph].renderData.state.AddBinderCommand( &pDrawableText_p[itGlyph].renderData.bindDrawableCbuffer );


		pDrawableText_p[itGlyph].drawable.SetSortKey( sorMask.intRepresentation );
		pDrawableText_p[itGlyph].drawable.SetDrawCall( m_pTextDrawCall );

		pDrawableText_p[itGlyph].drawable.SetPipelineStateGroup( m_textRenderStates ); // common data (vertex, IA, etc)
		pDrawableText_p[itGlyph].drawable.AddPipelineState( &m_textureState );
		pDrawableText_p[itGlyph].drawable.AddPipelineState( &m_fonts[iFontID_p].GetPipeState() );
		pDrawableText_p[itGlyph].drawable.AddPipelineState( &pDrawableText_p[itGlyph].renderData.state );

		++itChar;
		++itGlyph;
	}

	nDrawables_p = itGlyph;
}


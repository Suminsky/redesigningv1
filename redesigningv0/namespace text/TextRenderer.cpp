#include "TextRenderer.h"

using namespace text;
using namespace DirectX;

void text::TextRenderer::Initialize( dx::Device * pDevice_p, sprite::SpriteRenderer & spriteRenderer_p )
{
	m_glyphsRenderData.Initialize( E_MAXGLYPHS );

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
	//pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, m_pGlyphBufferInterface );
	//pDevice_p->GetDevice()->CreateBuffer( &cbufferParams.desc.bufferDesc, nullptr, &m_pGlyphBufferInterface );
	m_pGlyphBufferInterface = dx::BufferResource::Create( pDevice_p->GetDevice(), cbufferParams );
	
	// !!!!!!!!!!!!! TODO NOW !!!!!!!!!!!!!!!!!!!!
	// using another buffer interface is the problem, not the buffer config, but another interface ptr

	// creates the render pipeState holding binds commom to all glyphs
	// caralho d referencias, nao posso mudar essas porra, pq sao usadas em otros lugares
	// TODO: TEM q t um cache pra todos os binders usados no jogo

	m_textRenderStates.push_back( &m_pSpriteRendererRef->m_defaultVertexInput );
	m_textRenderStates.push_back( &m_pSpriteRendererRef->m_spriteShaderRes.m_permutations[0].m_pipeState );
	//m_textRenderStates.push_back( &m_pSpriteRendererRef->m_camera.m_pipeState );

		// NOTE: since camera is being set here, you cant add another camera later, cause the later ones are
		// discarded on the drawables queue
		// If u remove it, it will use last one bound(from another drawable?), thats where hodgmans cleaning
		// states comes in..although I think it may be quite usefull, it obviously avoid hardcore bugs due
		// misuse

	m_textureState.AddBinderCommand( &m_pSpriteRendererRef->m_samplers_cache.GetSamplerBind(sprite::E_SAMPLER_NONE) );
	m_textureState.AddBinderCommand( &m_pSpriteRendererRef->m_blends_cache.GetBlendBind(sprite::E_BLEND_ALPHA_BLENDED) );

	m_pTextDrawCall = &m_pSpriteRendererRef->m_drawIndexed;

	InitializeGlyphs();
}


void text::TextRenderer::RenderText( const wchar_t szText_p[], DirectX::XMFLOAT4 pos_p, sprite::Camera & camera_p, UINT iFontID_p /*= 0 */  )
{
	if( szText_p[0] == 0 ) return;
	//

	sprite::DrawableCbuffer glyphCBuffer;

	// color (common to all glyphs, but per drawable too)

	glyphCBuffer.m_color.x = //0.5f;
	glyphCBuffer.m_color.y = 
	glyphCBuffer.m_color.z = 
	glyphCBuffer.m_color.w = 1.0f;

	glyphCBuffer.m_mWorld = XMMatrixIdentity();

	UINT itChar = 0;
	float fPosOffsetX = 0.0f;
	float fPosOffsetY = 0.0f;

	render::Drawable drawable;
	sprite::SortMask sorMask = {0};
	sorMask.bitfield.layer = 2;
	sorMask.bitfield.viewportLayer = 3;
	sorMask.bitfield.textureID = m_fonts[iFontID_p].GetTextureID();
	sorMask.bitfield.transparency = sprite::E_BLEND_ALPHA_BLENDED;

	drawable.SetSortKey( sorMask.intRepresentation );
	drawable.SetDrawCall( m_pTextDrawCall );
	drawable.SetPipelineStateGroup( m_textRenderStates ); // common data (vertex, IA, etc)
	drawable.AddPipelineState( &m_textureState );
	drawable.AddPipelineState( &m_fonts[iFontID_p].GetPipeState() );
	drawable.AddPipelineState( &camera_p.m_pipeState );

// 	m_pSpriteRendererRef->BeginInstancing(  m_fonts[iFontID_p].GetTextureBinder(),
// 		&m_pSpriteRendererRef->m_blends_cache.GetBlendBind(sprite::E_BLEND_ALPHA_BLENDED),
// 		&m_pSpriteRendererRef->m_samplers_cache.GetSamplerBind(sprite::E_SAMPLER_NONE) );

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
		else if( szText_p[itChar] == L'\t' ){

			fPosOffsetX += gen::RoundUpToNextMultiple_POT( int(fPosOffsetX+0.5f), m_fonts[iFontID_p].GetTabMultiple());
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

		glyphCBuffer.m_mWorld.r[3] = XMLoadFloat4( &pos );

		fPosOffsetX += halfWidth + 1.0f;

		// uv
		glyphCBuffer.m_uvRect = XMFLOAT4( uv.X, uv.Y, uv.Width, uv.Height );

		// glyph pipeState ( bind cbuffer and texture ) should remain, so add to buffer here

		m_glyphsRenderData().cbufferData = glyphCBuffer;
		drawable.AddPipelineState( &m_glyphsRenderData().pipeState );

		m_pSpriteRendererRef->m_queue.Submit( drawable );
// 		sprite::SpriteRenderer::spriteInstance instanceData;
// 		memcpy( (void*)instanceData.color, (void*)&glyphCBuffer.m_color, sizeof(float)*4 );
// 		instanceData.padding[0] = instanceData.padding[1] = 0.0f;
// 		memcpy( (void*)instanceData.res, (void*)&glyphCBuffer.m_res, sizeof(float)*2 );
// 		memcpy( (void*)instanceData.mWorld, (void*)&glyphCBuffer.m_mWorld, sizeof(float)*16 );
// 		memcpy( (void*)instanceData.uvRect, (void*)&glyphCBuffer.m_uvRect, sizeof(float)*4 );

		//m_pSpriteRendererRef->RenderInstance( instanceData, sorMask.intRepresentation );

		drawable.PopLastPipelineState();

		++itChar;
		++m_glyphsRenderData;
	}

	//m_pSpriteRendererRef->FinishInstancingRender( pDevice_p, sorMask.intRepresentation );
}

void text::TextRenderer::Draw_Text( const wchar_t szText_p[], DirectX::XMFLOAT4 pos_p, UINT iFontID_p,
									DrawableGlyph * pDrawableText_p, int & nDrawables_p, sprite::Camera & camera_p )
{
	if( szText_p[0] == 0x0000 ) return;

	sprite::SortMask sorMask = {0};
	sorMask.bitfield.layer = 2;
	sorMask.bitfield.viewportLayer = 3;
	//sorMask.bitfield.Zdepth = 10;
	sorMask.bitfield.textureID = m_fonts[iFontID_p].GetTextureID();
	sorMask.bitfield.transparency = sprite::E_BLEND_ALPHA_BLENDED;

	float fPosOffsetX = 0.0f;
	float fPosOffsetY = 0.0f;

	int itChar = 0;
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
		else if(szText_p[itChar] == L'\r' ){

			++itChar;
			continue;
		}
		else if( szText_p[itChar] == L'\t' ){
			
			fPosOffsetX += (m_fonts[iFontID_p].GetSpaceWidth()*4.0f);
			++itChar;
			continue;
		}

		pDrawableText_p[itGlyph].renderData.cbufferData.m_color.x = //0.5f;
		pDrawableText_p[itGlyph].renderData.cbufferData.m_color.y = 
		pDrawableText_p[itGlyph].renderData.cbufferData.m_color.z = 
		pDrawableText_p[itGlyph].renderData.cbufferData.m_color.w = 1.0f;
		pDrawableText_p[itGlyph].renderData.cbufferData.m_mWorld = XMMatrixIdentity();

		// set data specific to each glyph

		GlyphRect uv = m_fonts[iFontID_p].GetGlyphUV( szText_p[itChar] );

		//res
		pDrawableText_p[itGlyph].renderData.cbufferData.m_res.x = uv.Width * (float)m_fonts[iFontID_p].GetTextureWidth();
		pDrawableText_p[itGlyph].renderData.cbufferData.m_res.y = uv.Height * (float)m_fonts[iFontID_p].GetTextureHeight();

		float halfWidth = pDrawableText_p[itGlyph].renderData.cbufferData.m_res.x * 0.5f;

		// pos
		XMFLOAT4 pos = pos_p;

		fPosOffsetX += halfWidth;

		pos.x += fPosOffsetX;
		pos.y += fPosOffsetY;

		pDrawableText_p[itGlyph].renderData.cbufferData.m_mWorld.r[3] = XMLoadFloat4( &pos );

		fPosOffsetX += halfWidth + 1.0f;

		// uv
		pDrawableText_p[itGlyph].renderData.cbufferData.m_uvRect = XMFLOAT4( uv.X, uv.Y, uv.Width, uv.Height );

		//
		pDrawableText_p[itGlyph].renderData.bindDrawableCbuffer.Initialize( m_pGlyphBufferInterface, &pDrawableText_p[itGlyph].renderData.cbufferData );
		pDrawableText_p[itGlyph].renderData.pipeState.AddBinderCommand( &pDrawableText_p[itGlyph].renderData.bindDrawableCbuffer );

		pDrawableText_p[itGlyph].drawable.SetSortKey( sorMask.intRepresentation );
		pDrawableText_p[itGlyph].drawable.SetDrawCall( m_pTextDrawCall );

		pDrawableText_p[itGlyph].drawable.SetPipelineStateGroup( m_textRenderStates ); // common data (vertex, IA, etc)
		pDrawableText_p[itGlyph].drawable.AddPipelineState( &m_textureState );
		pDrawableText_p[itGlyph].drawable.AddPipelineState( &m_fonts[iFontID_p].GetPipeState() );
		pDrawableText_p[itGlyph].drawable.AddPipelineState( &pDrawableText_p[itGlyph].renderData.pipeState );
		pDrawableText_p[itGlyph].drawable.AddPipelineState( &camera_p.m_pipeState );

		++itChar;
		++itGlyph;
	}

	nDrawables_p = itGlyph;
}

int text::TextRenderer::ComputeNumberOfGlyphs( const wchar_t szText_p[] )
{
	int itChar = 0;
	int itGlyph = 0;

	while( szText_p[itChar] != 0x0000 ){


		// handle spaces

		if( szText_p[itChar] == L' ' ){

			++itChar;
			continue;
		}
		else if(szText_p[itChar] == L'\n' ){

			++itChar;
			continue;
		}
		else if(szText_p[itChar] == L'\r' ){

			++itChar;
			continue;
		}
		else if( szText_p[itChar] == L'\t' ){

			++itChar;
			continue;
		}

		++itChar;
		++itGlyph;
	}

	return itGlyph;
}
//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================
void text::TextRenderer::RenderText(
	const wchar_t szText_p[], DirectX::XMFLOAT4 pos_p, sprite::Camera & camera_p, ID3D11DeviceContext * pDContext_p,
	sprite::SortMask sortKey_p, UINT iFontID_p,
	sprite::E_BLENDTYPE eBlendType_p, sprite::E_SAMPLERTYPE eSamplerType_p )
{
	if( szText_p[0] == 0 ) return;

	// cache instance common data (color and world rotation)
	sprite::spriteInstance perGlyph = {0};
	(*((XMFLOAT4*)perGlyph.color)) = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	perGlyph.mWorld[0] = perGlyph.mWorld[5] = perGlyph.mWorld[10] = perGlyph.mWorld[15] = 1.0f;

	// prepare for instancing
	// TODO:blend & sampler by param
	m_perFrameText.Initialize(	*m_fonts[iFontID_p].GetTextureBinder(),
								m_pSpriteRendererRef->m_blends_cache.GetBlendBind(eBlendType_p),
								m_pSpriteRendererRef->m_samplers_cache.GetSamplerBind(eSamplerType_p),
								m_pSpriteRendererRef->m_dynamic_tmp_instancesVB	);
	m_perFrameText.StartBufferingInstances();

	// cached
	float fPosOffsetX = 0.0f;
	float fPosOffsetY = 0.0f;

	for( int itChar = 0; szText_p[itChar] != 0x0000; ++ itChar ){

		// handle spaces
		{
		if( szText_p[itChar] == L' ' ){

			fPosOffsetX += m_fonts[iFontID_p].GetSpaceWidth();
			continue;
		}
		else if(szText_p[itChar] == L'\n' ){

			fPosOffsetY -= m_fonts[iFontID_p].GetMinNewLineHeight() + 1.0f;
			fPosOffsetX = 0.0f;
			continue;
		}
		else if( szText_p[itChar] == L'\t' ){

			fPosOffsetX += gen::RoundUpToNextMultiple_POT( int(fPosOffsetX+0.5f), m_fonts[iFontID_p].GetTabMultiple());
			continue;
		}
		}

		// set data specific to each glyph

		// uv
		(*((GlyphRect*)perGlyph.uvRect)) = m_fonts[iFontID_p].GetGlyphUV( szText_p[itChar] );

		// res
		perGlyph.res[0] = perGlyph.uvRect[2] * (float)m_fonts[iFontID_p].GetTextureWidth(); // uv width * tex res
		perGlyph.res[1] = perGlyph.uvRect[3] * (float)m_fonts[iFontID_p].GetTextureHeight();

		float halfWidth = perGlyph.res[0] * 0.5f; // cache
		fPosOffsetX += halfWidth;

		// pos
		//(*((XMFLOAT4*)perGlyph.mWorld[gen::indexFromXY(0, 3, 4)])) = pos_p;
		perGlyph.mWorld[gen::indexFromXY(0, 3, 4)] = pos_p.x + fPosOffsetX;
		perGlyph.mWorld[gen::indexFromXY(1, 3, 4)] = pos_p.y + fPosOffsetY;
		perGlyph.mWorld[gen::indexFromXY(2, 3, 4)] = pos_p.z;

		fPosOffsetX += halfWidth + 1.0f; // a pixel of space between glyphs

		m_perFrameText.AddInstance( perGlyph );
	}

	sortKey_p.bitfield.textureID = m_fonts[iFontID_p].GetTextureID();
	sortKey_p.bitfield.transparency = eBlendType_p;
	m_perFrameText.ConcludeInstancesBuffering_NoSort( pDContext_p, sortKey_p.intRepresentation );

	m_pSpriteRendererRef->Render( &m_perFrameText, &camera_p );
}

void text::TextRenderer::Draw_Text( sprite::InstancedSprites & instancedSprites_p, const wchar_t szText_p[], DirectX::XMFLOAT4 pos_p, ID3D11DeviceContext * pDContext_p, sprite::SortMask sortKey_p, UINT iFontID_p /*= 0*/, sprite::E_BLENDTYPE eBlendType_p /*= sprite::E_BLEND_ALPHA_BLENDED*/, sprite::E_SAMPLERTYPE eSamplerType_p /*= sprite::E_SAMPLER_NONE */ )
{
	if( szText_p[0] == 0 ) return;

	// cache instance common data (color and world rotation)
	sprite::spriteInstance perGlyph = {0};
	(*((XMFLOAT4*)perGlyph.color)) = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	perGlyph.mWorld[0] = perGlyph.mWorld[5] = perGlyph.mWorld[10] = perGlyph.mWorld[15] = 1.0f;

	// prepare for instancing
	// TODO:blend & sampler by param
	instancedSprites_p.Initialize(
		*m_fonts[iFontID_p].GetTextureBinder(),
		m_pSpriteRendererRef->m_blends_cache.GetBlendBind(eBlendType_p),
		m_pSpriteRendererRef->m_samplers_cache.GetSamplerBind(eSamplerType_p),
		m_pSpriteRendererRef->m_dynamic_tmp_instancesVB	);
	instancedSprites_p.StartBufferingInstances();

	// cached
	float fPosOffsetX = 0.0f;
	float fPosOffsetY = 0.0f;

	for( int itChar = 0; szText_p[itChar] != 0x0000; ++ itChar ){

		// handle spaces
		{
			if( szText_p[itChar] == L' ' ){

				fPosOffsetX += m_fonts[iFontID_p].GetSpaceWidth();
				continue;
			}
			else if(szText_p[itChar] == L'\n' ){

				fPosOffsetY -= m_fonts[iFontID_p].GetMinNewLineHeight() + 1.0f;
				fPosOffsetX = 0.0f;
				continue;
			}
			else if( szText_p[itChar] == L'\t' ){

				fPosOffsetX += gen::RoundUpToNextMultiple_POT( int(fPosOffsetX+0.5f), m_fonts[iFontID_p].GetTabMultiple());
				continue;
			}
		}

		// set data specific to each glyph

		// uv
		(*((GlyphRect*)perGlyph.uvRect)) = m_fonts[iFontID_p].GetGlyphUV( szText_p[itChar] );

		// res
		perGlyph.res[0] = perGlyph.uvRect[2] * (float)m_fonts[iFontID_p].GetTextureWidth(); // uv width * tex res
		perGlyph.res[1] = perGlyph.uvRect[3] * (float)m_fonts[iFontID_p].GetTextureHeight();

		float halfWidth = perGlyph.res[0] * 0.5f; // cache
		fPosOffsetX += halfWidth;

		// pos
		//(*((XMFLOAT4*)perGlyph.mWorld[gen::indexFromXY(0, 3, 4)])) = pos_p;
		perGlyph.mWorld[gen::indexFromXY(0, 3, 4)] = pos_p.x + fPosOffsetX;
		perGlyph.mWorld[gen::indexFromXY(1, 3, 4)] = pos_p.y + fPosOffsetY;
		perGlyph.mWorld[gen::indexFromXY(2, 3, 4)] = pos_p.z;

		fPosOffsetX += halfWidth + 1.0f; // a pixel of space between glyphs

		instancedSprites_p.AddInstance( perGlyph );
	}

	sortKey_p.bitfield.textureID = m_fonts[iFontID_p].GetTextureID();
	sortKey_p.bitfield.transparency = eBlendType_p;
	instancedSprites_p.ConcludeInstancesBuffering_NoSort( pDContext_p, sortKey_p.intRepresentation );
}

void text::TextRenderer::Draw_Text( sprite::InstancedSprites & instancedSprites_p, unsigned int iIndexOnVB_p, const wchar_t szText_p[],
	DirectX::XMFLOAT4 pos_p, ID3D11DeviceContext * pDContext_p, sprite::SortMask sortKey_p, UINT iFontID_p /*= 0*/ )
{
	if( szText_p[0] == 0 ) return;

	// cache instance common data (color and world rotation)
	sprite::spriteInstance perGlyph = {0};
	(*((XMFLOAT4*)perGlyph.color)) = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	perGlyph.mWorld[0] = perGlyph.mWorld[5] = perGlyph.mWorld[10] = perGlyph.mWorld[15] = 1.0f;

	// prepare for instancing

	instancedSprites_p.StartBufferingInstancesAtVBIndex( iIndexOnVB_p );

	// cached
	float fPosOffsetX = 0.0f;
	float fPosOffsetY = 0.0f;

	for( int itChar = 0; szText_p[itChar] != 0x0000; ++ itChar ){

		// handle spaces
		{
			if( szText_p[itChar] == L' ' ){

				fPosOffsetX += m_fonts[iFontID_p].GetSpaceWidth();
				continue;
			}
			else if(szText_p[itChar] == L'\n' ){

				fPosOffsetY -= m_fonts[iFontID_p].GetMinNewLineHeight() + 1.0f;
				fPosOffsetX = 0.0f;
				continue;
			}
			else if( szText_p[itChar] == L'\t' ){

				fPosOffsetX += gen::RoundUpToNextMultiple_POT( int(fPosOffsetX+0.5f), m_fonts[iFontID_p].GetTabMultiple());
				continue;
			}
		}

		// set data specific to each glyph

		// uv
		(*((GlyphRect*)perGlyph.uvRect)) = m_fonts[iFontID_p].GetGlyphUV( szText_p[itChar] );

		// res
		perGlyph.res[0] = perGlyph.uvRect[2] * (float)m_fonts[iFontID_p].GetTextureWidth(); // uv width * tex res
		perGlyph.res[1] = perGlyph.uvRect[3] * (float)m_fonts[iFontID_p].GetTextureHeight();

		float halfWidth = perGlyph.res[0] * 0.5f; // cache
		fPosOffsetX += halfWidth;

		// pos
		//(*((XMFLOAT4*)perGlyph.mWorld[gen::indexFromXY(0, 3, 4)])) = pos_p;
		perGlyph.mWorld[gen::indexFromXY(0, 3, 4)] = pos_p.x + fPosOffsetX;
		perGlyph.mWorld[gen::indexFromXY(1, 3, 4)] = pos_p.y + fPosOffsetY;
		perGlyph.mWorld[gen::indexFromXY(2, 3, 4)] = pos_p.z;

		fPosOffsetX += halfWidth + 1.0f; // a pixel of space between glyphs

		instancedSprites_p.AddInstance( perGlyph );
	}

	//sortKey_p.bitfield.textureID = m_fonts[iFontID_p].GetTextureID();
	//sortKey_p.bitfield.transparency = eBlendType_p;
	instancedSprites_p.ConcludeInstancesBuffering_NoSort( pDContext_p, sortKey_p.intRepresentation );
}

void text::TextRenderer::InitializeSpriteInstances( sprite::InstancedSprites & instancedSprites_p, sprite::InstancesVertexBuffer & instVB_p, sprite::SortMask & sortMask_p, UINT iFontID_p /*= 0*/, sprite::E_BLENDTYPE eBlendType_p /*= sprite::E_BLEND_ALPHA_BLENDED*/, sprite::E_SAMPLERTYPE eSamplerType_p /*= sprite::E_SAMPLER_NONE */ )
{
	instancedSprites_p.Initialize(
		*m_fonts[iFontID_p].GetTextureBinder(),
		m_pSpriteRendererRef->m_blends_cache.GetBlendBind(eBlendType_p),
		m_pSpriteRendererRef->m_samplers_cache.GetSamplerBind(eSamplerType_p),
		instVB_p	);

	sortMask_p.bitfield.textureID = m_fonts[iFontID_p].GetTextureID();
	sortMask_p.bitfield.transparency = eBlendType_p;
}

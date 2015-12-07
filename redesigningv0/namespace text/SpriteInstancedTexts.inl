
template<int NTEXTS>
void text::SpriteInstancedTexts<NTEXTS>::CreateVertexBuffer( unsigned int vNMaxChars_p[], dx::Device * pDevice_p, unsigned int iVBSlot_p /*= 1 */ )
{
	
	
	m_nMaxChars[0] = vNMaxChars_p[0];
	m_iOffsetsOnVB[0] = 0; 
	unsigned int totalChars = m_nMaxChars[0];

	for( int it = 1; it < NTEXTS; ++it ){

		m_nMaxChars[it] = vNMaxChars_p[it];
		totalChars += m_nMaxChars[it];
		m_iOffsetsOnVB[it] = m_nMaxChars[it-1];
	}

	BufferResource::CreationParams cbufferParams;
	ZeroMemory(&cbufferParams, sizeof(BufferResource::CreationParams));
	cbufferParams.desc.bufferDesc.ByteWidth =		sizeof(spriteInstance)*totalChars;
	cbufferParams.desc.bufferDesc.Usage =			D3D11_USAGE_DYNAMIC;
	cbufferParams.desc.bufferDesc.BindFlags =		D3D11_BIND_VERTEX_BUFFER;
	cbufferParams.desc.bufferDesc.CPUAccessFlags =  D3D11_CPU_ACCESS_WRITE;
	cbufferParams.desc.bufferDesc.StructureByteStride = 0;
	cbufferParams.desc.bufferDesc.MiscFlags = 0;

	if( m_pIVB ){
		m_pIVB->Release();
	}

	m_pIVB = dx::BufferResource::Create( pDevice_p->GetDevice(), cbufferParams );

	m_instsVB.Initialize( totalChars, iVBSlot_p, m_pIVB );
}



template<int NTEXTS>
void text::SpriteInstancedTexts<NTEXTS>::InitializeInstancedSprites( TextRenderer * pTextRenderer_p,
	UINT iFontID_p /*= 0*/, sprite::SortMask sortMask_p /*= 0*/,
	sprite::E_BLENDTYPE eBlendType_p /*= sprite::E_BLEND_ALPHA_BLENDED*/,
	sprite::E_SAMPLERTYPE eSamplerType_p /*= sprite::E_SAMPLER_NONE*/ )
{
	m_fontID = iFontID_p;
	m_sortKey = sortMask_p;
	for( int it = 0; it < NTEXTS; it++ ){

		pTextRenderer_p->InitializeSpriteInstances( m_instSprites[it], m_instsVB, m_sortKey, iFontID_p, eBlendType_p, eSamplerType_p );
	}
}

template<int NTEXTS>
void text::SpriteInstancedTexts<NTEXTS>::RenderTexts( sprite::SpriteRenderer * pSpriteRenderer_p )
{
	for( int it = 0; it < NTEXTS; it++ ){

		pSpriteRenderer_p->Render( &m_instSprites[it], &pSpriteRenderer_p->m_camera );
	}
}


template<int NTEXTS>
void text::SpriteInstancedTexts<NTEXTS>::DrawTextAt( unsigned int itText_p, const wchar_t * szText_p, const DirectX::XMFLOAT4 & pos_p, TextRenderer * pTextRenderer_p, dx::Device * pDevice_p )
{
	m_textPos[itText_p] = pos_p;

	pTextRenderer_p->Draw_Text(
		m_instSprites[itText_p],
		m_iOffsetsOnVB[itText_p],
		szText_p,
		m_textPos[itText_p],
		pDevice_p->GetContext(),
		m_sortKey, m_fontID );
}

template<int NTEXTS>
void text::SpriteInstancedTexts<NTEXTS>::DrawText( unsigned int itText_p, const wchar_t * szText_p, TextRenderer * pTextRenderer_p, dx::Device * pDevice_p )
{
	pTextRenderer_p->Draw_Text(
		m_instSprites[itText_p],
		m_iOffsetsOnVB[itText_p],
		szText_p,
		m_textPos[itText_p],
		pDevice_p->GetContext(),
		m_sortKey, m_fontID );
}



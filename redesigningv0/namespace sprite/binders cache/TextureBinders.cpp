#include "TextureBinders.h"

int sprite::TextureBinders::Add( const char* szTexture_p )
{
	// create texture

	dx::Texture2DResource::CreationParams params;
	params.desc.szFilename = szTexture_p;
	params.pInitialData = NULL;

	ID3D11Texture2D * pTex2D;
	m_pDevice->m_pCacheTex2D->Acquire(params, pTex2D);

	// get texture desc to help on the srv ceation

	D3D11_TEXTURE2D_DESC desc = {0};
	pTex2D->GetDesc(&desc);

	// create srv

	dx::ShaderResourceView::CreationParams paramsSRV;
	paramsSRV.desc.pResourceData = pTex2D;
	paramsSRV.desc.srvDesc.Format = desc.Format;
	paramsSRV.desc.srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	paramsSRV.desc.srvDesc.Texture2D.MipLevels = desc.MipLevels;
	paramsSRV.desc.srvDesc.Texture2D.MostDetailedMip = 0;


	ID3D11ShaderResourceView * pSRV;
	m_pDevice->m_pCacheSRV->Acquire(paramsSRV, pSRV);

	// add bind command

	m_cache.push_back( dx::BindPSShaderResourceView(0, pSRV) );

	// add name

	m_names.push_back( szTexture_p );

	// returns the index

	return (int)(m_cache.size()-1);
}

dx::BindPSShaderResourceView & sprite::TextureBinders::Get( const char* szTexture_p )
{
	for( int it = 0, size = (int)m_cache.size();	it < size;		++it)
	{
		if( m_names[it] == szTexture_p )
		{
			return m_cache[it];
		}
	}

	// cache miss:

	int index = Add( szTexture_p );

	return m_cache[index];
}
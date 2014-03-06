#include "SpriteAtlas.h"

using namespace sprite;


sprite::SpriteAtlas::SpriteAtlas( dx::BindPSShaderResourceView * pTextureBinder_p, uint32_t size_p )
{
	Initialize( pTextureBinder_p, size_p );
}

void sprite::SpriteAtlas::Initialize( dx::BindPSShaderResourceView * pTextureBinder_p, uint32_t size_p )
{
	m_pTextureBinder = pTextureBinder_p;
	m_vSprites.resize( size_p );
}

void sprite::SpriteAtlas::SetSpriteAt( uint32_t index, SpriteInfo & spriteInfo_p )
{
	assert(index < (uint32_t)m_vSprites.size() );

	m_vSprites[index] = spriteInfo_p;
}

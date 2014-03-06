#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/02/24
	created:	24:2:2014   1:35
	file:		SpriteAtlas.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	-utility to store info of sprites stored on one texture, its on sprites and not
				on render because the UVs stored are rects.
				- padding offset is stored here because you will mostly need that info for trimmed
				sprites stored into an atlas

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <vector>
#include <assert.h>

#include <DirectXMath.h>

// private includes


// frwd declarations
namespace dx{

	class BindPSShaderResourceView;
}

namespace sprite{

	class SpriteAtlas{

	public:

		struct SpriteInfo{

			DirectX::XMFLOAT4 uvRect;
			DirectX::XMFLOAT2 res;
			DirectX::XMFLOAT2 paddingOffset;
		};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteAtlas();
		explicit SpriteAtlas( dx::BindPSShaderResourceView * pTextureBinder_p, uint32_t size_p );
		void Initialize( dx::BindPSShaderResourceView * pTextureBinder_p, uint32_t size_p );

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		SpriteInfo GetSpriteAt( uint32_t index_p ) const { return m_vSprites[index_p]; }
		dx::BindPSShaderResourceView * GetTextureBinder() const { return m_pTextureBinder; }

		void SetSpriteAt( uint32_t index, SpriteInfo & spriteInfo_p );

	private:

		dx::BindPSShaderResourceView * m_pTextureBinder;
		std::vector<SpriteInfo> m_vSprites;
	};
}
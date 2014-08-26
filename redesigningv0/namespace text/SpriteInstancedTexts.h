#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/06/07
	created:	7:6:2014   2:13
	file:		SpriteInstancedText.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../namespace sprite/SpriteRenderer.h"
#include "TextRenderer.h"

namespace text{

	template<int NTEXTS>
	class SpriteInstancedTexts{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		SpriteInstancedTexts(): m_pIVB(nullptr){}
		~SpriteInstancedTexts(){if( m_pIVB ) m_pIVB->Release();	}
		void CreateVertexBuffer( unsigned int vNMaxChars_p[], dx::Device * pDevice_p, unsigned int iVBSlot_p = 1 );


		void InitializeInstancedSprites( TextRenderer * pTextRenderer_p,
			UINT iFontID_p = 0, sprite::SortMask sortMask_p = sprite::SortMask(),
										 sprite::E_BLENDTYPE eBlendType_p = sprite::E_BLEND_ALPHA_BLENDED,
										 sprite::E_SAMPLERTYPE eSamplerType_p = sprite::E_SAMPLER_NONE );
		void DrawTextAt(	unsigned int itText_p, const wchar_t * szText_p, const DirectX::XMFLOAT4 & pos_p,
							TextRenderer * pTextRenderer_p, dx::Device * pDevice_p );
		void DrawText(	unsigned int itText_p, const wchar_t * szText_p, TextRenderer * pTextRenderer_p, dx::Device * pDevice_p );


		void RenderTexts( sprite::SpriteRenderer * pSpriteRenderer_p );

	private:

		sprite::InstancedSprites		m_instSprites[NTEXTS];
		sprite::InstancesVertexBuffer	m_instsVB;
		ID3D11Buffer					* m_pIVB;

		sprite::SortMask	m_sortKey;
		UINT m_fontID;

		DirectX::XMFLOAT4	m_textPos[NTEXTS];
		unsigned int		m_nMaxChars[NTEXTS], m_iOffsetsOnVB[NTEXTS];
	};

#include "SpriteInstancedTexts.inl"
}
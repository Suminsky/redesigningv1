#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/25
	created:	25:5:2013   21:58
	file:		ButtonComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	� Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "SpriteComponent.h"

namespace game{

	class ButtonComponent: public Component{

		shared_SpriteComponent_ptr m_pSpriteCompoRef;
		DirectX::XMFLOAT4 m_uv_Normal, m_uv_Hover, m_uv_Pressed;
		DirectX::XMFLOAT2 m_borderGap;	// used to make the "collision box" smaller

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ButtonComponent( const shared_SpriteComponent_ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f );
		ButtonComponent( const shared_SpriteComponent_ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, DirectX::XMFLOAT4 uvPressed_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f );

		void SetHoverImage(){

			m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Hover;
		}
		void SetNormalImage(){

			m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Normal;
		}
		void SetPressedImage(){

			m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Pressed;
		}

		//------------------------------------------------------------------------
		// use for mouse collision
		//------------------------------------------------------------------------
		bool GetPointCollision( DirectX::XMFLOAT2 mouseXY_p );	
	};

	typedef std::shared_ptr<ButtonComponent> shared_ButtonComponent_ptr;
}
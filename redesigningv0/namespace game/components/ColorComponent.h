#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/29
	created:	29:10:2013   14:23
	file:		ColorComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

#include <DirectXMath.h>

// private includes
#include "../Component.h"

namespace game{

	class ColorSystem;

	class ColorComponent: public Component{

		friend class ColorSystem;

	public:

		DirectX::XMFLOAT4 m_offsetColor;	// color that affects only the owner object
		DirectX::XMFLOAT4 m_currentColor;	// color that affects the owner and its childs

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		DirectX::XMFLOAT4 GetLocalColor() const{ return m_localColor; }
		DirectX::XMFLOAT4 GetWorldColor() const{ return m_worldColor; }

	private:

		//------------------------------------------------------------------------
		// stores offset * current
		//------------------------------------------------------------------------
		void UpdateLocalColor(){

			DirectX::XMStoreFloat4( &m_localColor,
									DirectX::XMVectorMultiply( XMLoadFloat4(&m_offsetColor), XMLoadFloat4(&m_currentColor) )
									);
		}

		//------------------------------------------------------------------------
		// stores local * parent world
		// NOTE! should be called after UpdateLocalColor, otherwise it will return
		// a possibly outdated color
		//------------------------------------------------------------------------
		void UpdateWorldColor( const DirectX::XMFLOAT4 & parentWorldColor_p ){

			m_lastWorldColor = m_worldColor;

			DirectX::XMStoreFloat4( &m_worldColor,
									DirectX::XMVectorMultiply( XMLoadFloat4(&m_localColor), XMLoadFloat4(&parentWorldColor_p) )
									);
		}

		DirectX::XMFLOAT4 m_localColor; // offset * current
		DirectX::XMFLOAT4 m_worldColor; // offset * current * parent world
		DirectX::XMFLOAT4 m_lastWorldColor;
	};

	typedef std::shared_ptr<ColorComponent> shared_ColorComponent_ptr;
	typedef std::weak_ptr<ColorComponent> weak_ColorComponent_ptr;
}
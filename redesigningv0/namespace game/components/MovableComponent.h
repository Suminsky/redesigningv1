#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/19
	created:	19:5:2013   15:52
	file:		MovableComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <new>
#include <DirectXMath.h>

// private includes
#include "../Component.h"
#include "../../namespace gen/gen_macros.h"


namespace game{

#pragma warning(push)
#pragma warning( disable : 4324) // padding due forced align

	class MovableComponent: public Component{

		DirectX::XMVECTOR	m_vPreviousPosition,
							m_vPreviousScale,
							m_vPreviousOrientation;
	public:
		DirectX::XMVECTOR	m_vCurrentPosition,
							m_vCurrentScale,
							m_vCurrentOrientation;

		//------------------------------------------------------------------------
		// ctor
		// initializes to "none"/"identity"
		//------------------------------------------------------------------------
		MovableComponent(){

			m_vPreviousPosition = m_vCurrentPosition = 
				m_vPreviousOrientation =  m_vCurrentOrientation = DirectX::g_XMIdentityR3;	// 0 0 0 1

			m_vPreviousScale = m_vCurrentScale = DirectX::g_XMOne;	// 1 1 1 1
		}

		//------------------------------------------------------------------------
		// Call this function each step of the simulation to save current state
		// to the previous state
		//------------------------------------------------------------------------
		void Step(){

			m_vPreviousPosition		= m_vCurrentPosition;
			m_vPreviousScale		= m_vPreviousScale;
			m_vPreviousOrientation	= m_vCurrentOrientation;
		}

		//------------------------------------------------------------------------
		// use this to get the render mWorld
		//------------------------------------------------------------------------
		DirectX::XMMATRIX ComputeRenderWorldMatrix( float fInterpolationFactor_p ){

			DirectX::XMVECTOR vFactor = DirectX::XMVectorReplicate(fInterpolationFactor_p); // do only once

			DirectX::XMVECTOR vPos =  DirectX::XMVectorLerpV( m_vPreviousPosition, m_vCurrentPosition, vFactor );
			DirectX::XMVECTOR vScale = DirectX::XMVectorLerpV( m_vPreviousScale, m_vCurrentScale, vFactor );
			DirectX::XMVECTOR vOrient = DirectX::XMQuaternionSlerpV( m_vPreviousOrientation, m_vCurrentOrientation, vFactor );

			return  DirectX::XMMatrixAffineTransformation(vScale, DirectX::g_XMZero, vOrient, vPos );
		}
		DirectX::XMMATRIX ComputeRenderWorldMatrixNoRotation( float fInterpolationFactor_p ){

			DirectX::XMVECTOR vFactor = DirectX::XMVectorReplicate(fInterpolationFactor_p); // do only once

			DirectX::XMVECTOR vPos =  DirectX::XMVectorLerpV( m_vPreviousPosition, m_vCurrentPosition, vFactor );
			DirectX::XMVECTOR vScale = DirectX::XMVectorLerpV( m_vPreviousScale, m_vCurrentScale, vFactor );
			//DirectX::XMVECTOR vOrient = DirectX::XMQuaternionSlerpV( m_vPreviousOrientation, m_vCurrentOrientation, vFactor );

			return  DirectX::XMMatrixAffineTransformation2D(vScale, DirectX::g_XMZero, 0.0f, vPos );
		}

		//------------------------------------------------------------------------
		// XMVECTOR need to be aligned by 16
		//------------------------------------------------------------------------
		ALLIGN16ONLY;
	};

#pragma warning(pop)

	typedef std::shared_ptr<MovableComponent> shared_MovableComponent_ptr;
	typedef std::weak_ptr<MovableComponent> weak_MovableComponent_ptr;
}
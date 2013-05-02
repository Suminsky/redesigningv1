//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/07
	created:	7:4:2013   23:21
	file base:	Movable
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

// system/standard headers

#include <DirectXMath.h>

// private headers

//========================================================================
// Inspired on "component model", its a movable component, focused on render
// data (methods for interpolating between prev and current position)
//========================================================================
class Movable{

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
	Movable(){

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
};
#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/30
	created:	30:10:2013   0:24
	file:		TransformComponent.h
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

	//========================================================================
	// 
	//========================================================================
	struct Trafo{

		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 qRotation;
		DirectX::XMFLOAT4 scale;

		DirectX::XMFLOAT4X4 DeriveMatrix(){

			DirectX::XMFLOAT4X4 mTrafo;

			DirectX::XMStoreFloat4x4( &mTrafo,
				DirectX::XMMatrixAffineTransformation(
				DirectX::XMLoadFloat4(&scale),
				DirectX::g_XMZero,
				DirectX::XMLoadFloat4(&qRotation),
				DirectX::XMLoadFloat4(&position)
				)
			);

			return mTrafo;
		}
	};

	//========================================================================
	// 
	//========================================================================
	class TransformComponent: public Component{

	public:

		Trafo m_offset;
		Trafo m_current;

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		DirectX::XMFLOAT4X4 GetLocalTransformation() const { return m_local; }
		DirectX::XMFLOAT4X4 GetWorldTransformation() const { return m_world; }

	private:

		//------------------------------------------------------------------------
		// stores offset * current
		//------------------------------------------------------------------------
		void UpdateLocalTransformation(){

			DirectX::XMFLOAT4X4 offsetDerived = m_offset.DeriveMatrix();
			DirectX::XMMATRIX mOffset = DirectX::XMLoadFloat4x4( &offsetDerived );

			DirectX::XMFLOAT4X4 currentDerived = m_current.DeriveMatrix();
			DirectX::XMMATRIX mCurrent = DirectX::XMLoadFloat4x4( &currentDerived );

			DirectX::XMStoreFloat4x4(
				&m_local,
				DirectX::XMMatrixMultiply( mOffset, mCurrent )
				);	
		}

		//------------------------------------------------------------------------
		// stores local * parent world
		// NOTE! should be called after UpdateLocalTransformation, otherwise it will
		// return a possibly outdated trafo
		//------------------------------------------------------------------------
		void UpdateWorldTransformation( const DirectX::XMFLOAT4X4 & mParentWorldTrafo_p ){

			m_previousWorld = m_world;

			//

			DirectX::XMMATRIX mLocal = DirectX::XMLoadFloat4x4( &m_local );
			DirectX::XMMATRIX mParentWorld = DirectX::XMLoadFloat4x4( &mParentWorldTrafo_p );

			DirectX::XMStoreFloat4x4(
				&m_world,
				DirectX::XMMatrixMultiply( mLocal, mParentWorld )
				);	
		}

		DirectX::XMFLOAT4X4 m_local; // offser * current
		DirectX::XMFLOAT4X4 m_world; // local * parent world
		DirectX::XMFLOAT4X4 m_previousWorld;

	};

	typedef std::shared_ptr<TransformComponent> shared_TransformComponent_ptr;
	typedef std::weak_ptr<TransformComponent> weak_TransformComponent_ptr;
}
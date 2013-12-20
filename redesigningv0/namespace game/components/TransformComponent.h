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
#include "../../namespace gen/TreeNode.h"
#include "../../namespace gen/Pool.h"
#include "../ComponentFactory.h"

namespace gen{
	template<typename T, unsigned int SIZE>	class Pool;
}

namespace game{

	class TransformSystem;
	class TransformComponentFactory;

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

		void FromMatrix( const DirectX::XMFLOAT4X4 & mTransform_p ){

			DirectX::XMMATRIX mTransform = DirectX::XMLoadFloat4x4( &mTransform_p );

			DirectX::XMVECTOR vPos, qRot, vScale;

			assert( DirectX::XMMatrixDecompose( &vScale, &qRot, &vPos, mTransform ) );

			DirectX::XMStoreFloat4( &position, vPos );
			DirectX::XMStoreFloat4( &qRotation, qRot );
			DirectX::XMStoreFloat4( &scale, vScale );
		}

		void FromMatrix( const DirectX::XMMATRIX & mTransform_p ){

			DirectX::XMVECTOR vPos, qRot, vScale;

			assert( DirectX::XMMatrixDecompose( &vScale, &qRot, &vPos, mTransform_p ) );

			DirectX::XMStoreFloat4( &position, vPos );
			DirectX::XMStoreFloat4( &qRotation, qRot );
			DirectX::XMStoreFloat4( &scale, vScale );
		}
	};

	//========================================================================
	// 
	//========================================================================
	class TransformComponent : public Component{

		//DCL_POOLELEMENT();

		friend class TransformSystem;
		friend class TransformComponentFactory;
		//template<typename T, unsigned int SIZE> friend class gen::Pool;
		DCL_POOLELEMENT();

	public:

		Trafo m_offset;
		Trafo m_local;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		TransformComponent();

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		DirectX::XMFLOAT4X4 GetWorld() const{ return m_world; }
		DirectX::XMFLOAT4X4 GetFinal() const{ return m_final; }
		DirectX::XMFLOAT4X4 GetPreviousFinal() const{ return m_previousFinal; }

		//------------------------------------------------------------------------
		// parenting stuff
		//------------------------------------------------------------------------
		void AddChild( TransformComponent * pTrafo_p );
		void RemoveChild( TransformComponent * pTrafo_p );
		void Snap(){

			m_bSnap = true;
		}
		bool GonnaSnap()const{return m_bSnap;}

	private:

		//------------------------------------------------------------------------
		// stores local * parent world and offset * world
		//------------------------------------------------------------------------
		void UpdateWorldAndFinalTransformation( const DirectX::XMFLOAT4X4 & mParentWorldTrafo_p );

		DirectX::XMFLOAT4X4 m_world; // local * parent world
		DirectX::XMFLOAT4X4 m_previousFinal;
		DirectX::XMFLOAT4X4 m_final;  // world * offset

		bool m_bSnap; // hack

		//unsigned int m_iCurrentRosterIndex;

		gen::TreeNode<TransformComponent*> m_node;
	};

	typedef std::shared_ptr<TransformComponent> shared_TransformComponent_ptr;
	typedef std::weak_ptr<TransformComponent> weak_TransformComponent_ptr;

	//========================================================================
	// 
	//========================================================================
	class TransformComponentFactory: public AComponentFactory{

		friend class TransformSystem;

	public:

		gen::Pool<TransformComponent, 256> m_pool;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		Trafo GetTrafoFromGfig( text::GfigElementA * pGFig_p );
		DirectX::XMFLOAT4 GetXYZWFromGfig( text::GfigElementA * pGFig_p );

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual shared_Component_ptr VCreateComponent();
		virtual shared_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );

	};

	typedef std::shared_ptr<TransformComponentFactory> shared_TransformComponentFactory_ptr;
	typedef std::weak_ptr<TransformComponentFactory> weak_TransformComponentFactory_ptr;
}
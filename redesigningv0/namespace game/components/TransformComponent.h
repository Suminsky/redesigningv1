#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/30
	created:	30:10:2013   0:24
	file:		TransformComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	� Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
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
	template<typename T>	class Pool;
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

		inline DirectX::XMFLOAT4X4 DeriveMatrix_f4x4(){

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
		inline DirectX::XMMATRIX DeriveMatrix(){

			return

			DirectX::XMMatrixAffineTransformation(	DirectX::XMLoadFloat4(&scale),
													DirectX::g_XMZero,
													DirectX::XMLoadFloat4(&qRotation),
													DirectX::XMLoadFloat4(&position)
													);
		}

		inline void FromMatrix( const DirectX::XMFLOAT4X4 & mTransform_p ){

			DirectX::XMMATRIX mTransform = DirectX::XMLoadFloat4x4( &mTransform_p );

			DirectX::XMVECTOR vPos, qRot, vScale;

			keepAssert( DirectX::XMMatrixDecompose( &vScale, &qRot, &vPos, mTransform ) );

			DirectX::XMStoreFloat4( &position, vPos );
			DirectX::XMStoreFloat4( &qRotation, qRot );
			DirectX::XMStoreFloat4( &scale, vScale );
		}
		inline void FromMatrix( const DirectX::XMMATRIX & mTransform_p ){

			DirectX::XMVECTOR vPos, qRot, vScale;

			keepAssert( DirectX::XMMatrixDecompose( &vScale, &qRot, &vPos, mTransform_p ) );

			DirectX::XMStoreFloat4( &position, vPos );
			DirectX::XMStoreFloat4( &qRotation, qRot );
			DirectX::XMStoreFloat4( &scale, vScale );
		}

		void RotateAround(const DirectX::XMVECTOR & pivot_p, const DirectX::XMVECTOR & qRot_p);

		void Rotate( const DirectX::XMVECTOR & normalAxis,  const float rad ) {

			DirectX::XMVECTOR qRot = DirectX::XMQuaternionRotationNormal(normalAxis, rad);
			DirectX::XMVECTOR qQ = DirectX::XMLoadFloat4(&qRotation);
			DirectX::XMStoreFloat4(&qRotation, DirectX::XMQuaternionMultiply(qQ, qRot));
		}
	};

	//========================================================================
	// 
	//========================================================================
	class TransformComponent : public Component{

		static const int E_MAXCHILDREN = 3;

		friend class TransformSystem;
		friend class TransformComponentFactory;

	public:

		Trafo m_offset;
		Trafo m_local;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		TransformComponent();
		~TransformComponent();

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		const DirectX::XMFLOAT4X4 & GetWorld() const{ return m_world; }
		const DirectX::XMFLOAT4X4 & GetFinal() const{ return m_final; }
		const DirectX::XMFLOAT4X4 & GetPreviousFinal() const{ return m_previousFinal; }

		void Snap(){ m_bSnap = true; }
		bool GonnaSnap()const{ return m_bSnap; }

		//------------------------------------------------------------------------
		// parenting stuff
		//------------------------------------------------------------------------
		void AddChild( TransformComponent * pTrafo_p );
		void RemoveChild( TransformComponent * pTrafo_p );
		TransformComponent * GetParent()const;
		TransformComponent * FindChildByObjectName( const char * szObjName_p );
		TransformComponent * GetChild( uint32_t it)const;
		bool HaveParent() const;
		bool HaveChild( uint32_t it )const;

		//------------------------------------------------------------------------
		// stores local * parent world and offset * world
		//------------------------------------------------------------------------
		void UpdateWorldAndFinalTransformation( const DirectX::XMFLOAT4X4 & mParentWorldTrafo_p );
		bool BUpdateWorldAndFinalTransformation( const DirectX::XMFLOAT4X4 & mParentWorldTrafo_p );
		void UpdateWorldAndFinalTransformation( const DirectX::XMMATRIX & mParentWorldTrafo_p );
		bool BUpdateWorldAndFinalTransformation( const DirectX::XMMATRIX & mParentWorldTrafo_p );
		void UpdateWorldAndFinalTransformation_NoPrevious( const DirectX::XMMATRIX & mParentWorldTrafo_p );
		void UpdateWorldAndFinalTransformation_NoPrevious( const DirectX::XMFLOAT4X4 & mParentWorldTrafo_p );

		//------------------------------------------------------------------------
		// for the orfans ;D
		//------------------------------------------------------------------------
		void UpdateWorldAndFinalTransformation();
		bool BUpdateWorldAndFinalTransformation();
		void UpdateWorldAndFinalTransformation_NoPrevious();

	private:

		DirectX::XMFLOAT4X4 m_world; // local * parent world
		DirectX::XMFLOAT4X4 m_previousFinal;
		DirectX::XMFLOAT4X4 m_final;  // world * offset

		bool m_bSnap; // hack

		gen::TreeNode_MC<TransformComponent*, E_MAXCHILDREN> m_node; // TODO, memory block on the factory for everyone? needs sibling linked list?
	};

	typedef gen::pool_ptr<TransformComponent> pool_TransformCompo_ptr;

	//========================================================================
	// 
	//========================================================================
	class TransformComponentFactory: public IComponentFactory{

		friend class TransformSystem;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		TransformComponentFactory(unsigned int maxComponents_p);

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static Trafo GetTrafoFromGfig( text::GfigElementA * pGFig_p );
		static DirectX::XMFLOAT4 GetXYZWFromGfig( text::GfigElementA * pGFig_p );

		//------------------------------------------------------------------------
		// updaters
		//------------------------------------------------------------------------
		static void UpdateDataFromGfig( TransformComponent & trafoCompo_p, text::GfigElementA * pGFig_p );
		static void UpdateTrafoFromGfig( Trafo & trafo_p, text::GfigElementA * pGFig_p );
		static void UpdateXYZWFromGfig( DirectX::XMFLOAT4 & XYZW_p, text::GfigElementA * pGFig_p );

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent();
		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );
		virtual pool_Component_ptr VCloneComponent( const Component * pCompo_p );
		virtual void VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p );
		virtual void VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p );
		virtual void VSerialize( const Component * pCompo_p, const Component * pDefault_p, text::GfigElementA * pGFig_p );
		virtual Component* VGetDefaultCompo();

		//------------------------------------------------------------------------
		// those 2 must receive an already named gfig
		//------------------------------------------------------------------------
		static void SerializeTrafo( const Trafo & trafo_p, const Trafo & default_p, text::GfigElementA & gFig_p );
		static void SerializeXYZW( const DirectX::XMFLOAT4 & xyzw_p, text::GfigElementA & gFig_p, const DirectX::XMFLOAT4 & fDefault_p );

	private:

		TransformComponent m_defaults;
		gen::Pool<TransformComponent> m_pool;
	};

	typedef std::shared_ptr<TransformComponentFactory> shared_TransformComponentFactory_ptr;
	typedef std::weak_ptr<TransformComponentFactory> weak_TransformComponentFactory_ptr;
}
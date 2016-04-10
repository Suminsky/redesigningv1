#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/17
	created:	17:11:2013   17:30
	file:		TransformSystem.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../System.h"
#include "../../namespace gen/Pool.h"
#include "../components/TransformComponent.h"

namespace game{

	class TransformSystem: public System{

		static const uint32_t E_MAX_NONROOTTRAFOS = 1024;

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		TransformSystem()
			:
		m_poolAccess(nullptr, nullptr){}

	private:

		gen::Pool<TransformComponent>::PoolAccessor m_poolAccess;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnInit();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnUpdate_oldRecursive( double , double );
		void VOnUpdate( double, double);

		inline void UpdateWorldAndFinalAndDispatchEveIfNeeded(TransformComponent & trafo_p);
		void UpdateWorldAndFinalAndDispatchEveIfNeeded(TransformComponent & trafo_p, DirectX::XMFLOAT4X4 & parentWorld_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void RecursiveUpdate( TransformComponent & currentParent_p );


	};

	typedef std::shared_ptr<TransformSystem> shared_TransformSystem_ptr;
	typedef std::weak_ptr<TransformSystem> weak_TransformSystem_ptr;
}
#include "TransformSystem.h"
#include "../Layer.h"

using namespace game;
using namespace DirectX;

void game::TransformSystem::VOnInit()
{
	shared_TransformComponentFactory_ptr pTrafoFactory = std::static_pointer_cast<TransformComponentFactory>(
		m_pSysMachineOwner->GetLayer()->m_componentFactories.GetComponentFactory<TransformComponent>()
		);

	m_poolAccess = pTrafoFactory->m_pool.GetAccess();
}

void game::TransformSystem::VOnUpdate( double , double )
{
	for( int itColor = 0, iSize = m_poolAccess.GetNAllocated();
		itColor < iSize;
		++itColor ){

			TransformComponent & transformCompo = (*m_poolAccess.GetAllocated( itColor ));

			// ignore if not root trafo

			if( transformCompo.m_node.GetParent() ) continue;

			// compute final trafo
			
			XMFLOAT4X4 mIdentity;
			XMStoreFloat4x4( &mIdentity, XMMatrixIdentity() );
			transformCompo.UpdateWorldAndFinalTransformation( mIdentity );

			if( transformCompo.IsAttached() )
				transformCompo.GetObjectOwner()->DispatchComponentEventImmediately( COMPONENT_TYPE(TransformComponent), &transformCompo );

			transformCompo.m_bSnap = false;

			// recurse on childes

			RecursiveUpdate( transformCompo );
	}
}

void game::TransformSystem::RecursiveUpdate( TransformComponent & currentParent_p )
{
	for( int itChild = 0, nChilds = currentParent_p.m_node.GetNumberOfChilds();
		itChild < nChilds;
		++itChild ){

			TransformComponent & childTransform = (*currentParent_p.m_node.GetChild(itChild)->GetData());

			// compute final color

			childTransform.UpdateWorldAndFinalTransformation(currentParent_p.GetWorld());

			if( childTransform.IsAttached() )
				childTransform.GetObjectOwner()->DispatchComponentEventImmediately( COMPONENT_TYPE(TransformComponent), &childTransform );

			RecursiveUpdate( childTransform );
	}
}
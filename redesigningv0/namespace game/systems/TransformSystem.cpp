#include "TransformSystem.h"
#include "../Layer.h"

using namespace game;
using namespace DirectX;

void game::TransformSystem::VOnInit()
{
	shared_TransformComponentFactory_ptr pTrafoFactory = std::static_pointer_cast<TransformComponentFactory>(
		m_pSysMachineOwner->GetLayer()->m_componentFactory.GetComponentFactory<TransformComponent>()
		);

	m_poolAccess = pTrafoFactory->m_pool.GetAccess();
}

void game::TransformSystem::VOnUpdate( double , double )
{
	// TODO depth first traversal?
	// put every root on a vector
	// for each node on the vector
	//	update final/world
	//	add every child on the vector
	// 

	for( unsigned int itTrafo = 0, iSize = m_poolAccess.GetNAllocated();
		itTrafo < iSize;
		++itTrafo ){

			TransformComponent & transformCompo = (*m_poolAccess.GetAllocated( itTrafo )); //(*m_poolAccess.GetAllocatedLinearly(itTrafo));//
			if( !transformCompo.GetObjectOwner()->IsAttached() ) continue;

			// ignore if not root trafo

			if( transformCompo.m_node.GetParent() ) continue;

			// compute final trafo
			
			//transformCompo.UpdateWorldAndFinalTransformation();
			if( transformCompo.BUpdateWorldAndFinalTransformation() || transformCompo.m_bSnap ){

			// TODO shouldnt dispatch if unchanged..(profile)-> actually, sprite need to receive the last previous for
			// proper interpolation
			
			transformCompo.GetObjectOwner()->DispatchComponentEventImmediately( COMPONENT_TYPE(TransformComponent), &transformCompo );

			transformCompo.m_bSnap = false;

			}
			// recurse on childs

			RecursiveUpdate( transformCompo );
			
	}
}

void game::TransformSystem::RecursiveUpdate( TransformComponent & currentParent_p )
{
	for( unsigned int itChild = 0, nChilds = currentParent_p.m_node.GetNumberOfChilds();
		itChild < nChilds;
		++itChild ){

			TransformComponent & childTransform = (*currentParent_p.m_node.GetChild(itChild)->GetData());
			if( !childTransform.GetObjectOwner()->IsAttached() ) continue;
			// compute final trafo

			//childTransform.UpdateWorldAndFinalTransformation(currentParent_p.GetWorld());
			if( childTransform.BUpdateWorldAndFinalTransformation(currentParent_p.GetWorld()) || childTransform.m_bSnap ){

				//if( childTransform.IsAttached() ) why this?
				childTransform.GetObjectOwner()->DispatchComponentEventImmediately( COMPONENT_TYPE(TransformComponent), &childTransform );

				childTransform.m_bSnap = false;

			}

			// recurse on childes

			RecursiveUpdate( childTransform );
	}
}
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


void game::TransformSystem::VOnUpdate_oldRecursive( double , double )
{
	// recursive version

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
			
				transformCompo.GetObjectOwner()
					->DispatchComponentEventImmediately( COMPONENT_TYPE(TransformComponent), &transformCompo );

				transformCompo.m_bSnap = false;

			}
			// recurse on childs

			RecursiveUpdate( transformCompo );
	}
}

void game::TransformSystem::VOnUpdate( double, double )
{
	// dfs version
	
	//	for all root trafos
	//		update every world final
	//		add every children to a vector
	//	for all trafos on the vector (non root trafos)
	//		update every trafo, using its parent world
	//		add its children to the vector
	//	Due the order, all parents will have been updated before its children are
	
	gen::Stack<TransformComponent*, E_MAX_NONROOTTRAFOS> nonRootTrafos;

	for( unsigned int itTrafo = 0, iSize = m_poolAccess.GetNAllocated();
		itTrafo < iSize;
		++itTrafo ){

			TransformComponent & transformCompo = (*m_poolAccess.GetAllocated( itTrafo )); //(*m_poolAccess.GetAllocatedLinearly(itTrafo));//
			if( !transformCompo.GetObjectOwner()->IsAttached() ) continue;

			// ignore if not root trafo

			if( transformCompo.m_node.GetParent() ) continue;

			// update and add children to vector

			UpdateWorldAndFinalAndDispatchEveIfNeeded(transformCompo);

			for( unsigned int itChild = 0;
				itChild < transformCompo.m_node.GetNumberOfChilds();
				++itChild ){

					TransformComponent & childTransform = (*transformCompo.m_node.GetChild(itChild)->GetData());
					if( !childTransform.GetObjectOwner()->IsAttached() ) continue;

					nonRootTrafos.PushBack(&childTransform);
			}
	}

	// in order, update each trafo world/final and add its children at the end of the vector

	for( uint32_t it = 0; it < nonRootTrafos.Size(); ++it){

		TransformComponent & transformCompo = *nonRootTrafos[it];

		UpdateWorldAndFinalAndDispatchEveIfNeeded(transformCompo, transformCompo.GetParent()->m_world);
		
		for( unsigned int itChild = 0;
			itChild < transformCompo.m_node.GetNumberOfChilds();
			++itChild ){

				TransformComponent & childTransform = (*transformCompo.m_node.GetChild(itChild)->GetData());
				if( !childTransform.GetObjectOwner()->IsAttached() ) continue;

				nonRootTrafos.PushBack(&childTransform);
		}
	}
}


void game::TransformSystem::RecursiveUpdate( TransformComponent & currentParent_p )
{
	for( unsigned int itChild = 0, nChilds = currentParent_p.m_node.GetNumberOfChilds();
		itChild < nChilds;
		++itChild ){

			TransformComponent & childTransform = (*currentParent_p.m_node.GetChild(itChild)->GetData());
			if( !childTransform.GetObjectOwner()->IsAttached() ) continue;

			UpdateWorldAndFinalAndDispatchEveIfNeeded(childTransform, currentParent_p.m_world);

			// recurse on childes

			RecursiveUpdate( childTransform );
	}
}


void game::TransformSystem::UpdateWorldAndFinalAndDispatchEveIfNeeded(TransformComponent & trafo_p)
{
	if( trafo_p.BUpdateWorldAndFinalTransformation()
		||
		trafo_p.m_bSnap ){

			trafo_p.GetObjectOwner()
				->DispatchComponentEventImmediately( COMPONENT_TYPE(TransformComponent), &trafo_p );

			trafo_p.m_bSnap = false;
	}
}

void game::TransformSystem::UpdateWorldAndFinalAndDispatchEveIfNeeded( TransformComponent & trafo_p, DirectX::XMFLOAT4X4 & parentWorld_p )
{
	if( trafo_p.BUpdateWorldAndFinalTransformation(parentWorld_p)
		||
		trafo_p.m_bSnap ){

			trafo_p.GetObjectOwner()
				->DispatchComponentEventImmediately( COMPONENT_TYPE(TransformComponent), &trafo_p );

			trafo_p.m_bSnap = false;
	}
}

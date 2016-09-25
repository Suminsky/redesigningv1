#include "PCHF.h"
#include "ColorSystem.h"
#include "../Layer.h"

using namespace game;

void game::ColorSystem::VOnInit()
{
	shared_ColorComponentFactory_ptr pColorFactory = std::static_pointer_cast<ColorComponentFactory>(
		m_pSysMachineOwner->GetLayer()->m_componentFactory.GetComponentFactory<ColorComponent>()
		);

	m_poolAccess = pColorFactory->m_pool.GetAccess();
}

void game::ColorSystem::VOnUpdate( double /*dAccum_p*/, double /*dDelta_p*/ )
{

	// traverse colors

	for( int itColor = 0, iSize = m_poolAccess.GetNAllocated();
		itColor < iSize;
		++itColor ){

			ColorComponent & colorCompo = (*m_poolAccess.GetAllocated( itColor ));
			if( !colorCompo.GetObjectOwner()->IsAttached() ) continue;

			// ignore if not root color

			if( colorCompo.m_node.GetParent() ) continue;

			// compute final color

			colorCompo.UpdateWorldAndFinalColor();

			//if( colorCompo.IsAttached() )
			colorCompo.GetObjectOwner()->DispatchComponentEventImmediately( COMPONENT_TYPE(ColorComponent), &colorCompo );

			colorCompo.m_bSnap = false;

			// recurse on childes

			RecursiveUpdate( colorCompo );
	}
}

void game::ColorSystem::RecursiveUpdate( ColorComponent & currentParent_p )
{
	for( int itChild = 0, nChilds = currentParent_p.m_node.GetNumberOfChilds();
		itChild < nChilds;
		++itChild ){

			ColorComponent & childColor = (*currentParent_p.m_node.GetChild(itChild)->GetData());

			// compute final color

			childColor.UpdateWorldAndFinalColor(currentParent_p.GetWorldColor());

			if( childColor.IsAttached() )
				childColor.GetObjectOwner()->DispatchComponentEventImmediately( COMPONENT_TYPE(ColorComponent), &childColor );

			RecursiveUpdate( childColor );
	}
}

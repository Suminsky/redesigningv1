#include "ComponentFactory.h"

using namespace game;

template< typename DerivedComponent >
void game::ComponentFactory::RegisterFactory( const shared_IComponentFactory_ptr & pFactory_p )
{
	RegisteredFactory newFactory = {COMPONENT_TYPE(DerivedComponent), pFactory_p, COMPONENT_NAME(DerivedComponent) };
	NDBG(
	m_registry.push_back(newFactory);
	);
	DBG(
	assert( GetComponentFactory<DerivedComponent>() == nullptr );
	m_registry.push_back(newFactory);
	);
	// insert returns a pair of <it, bool> indicating if the insertion took place or there
	// was already an element
}

template< typename DerivedComponent >
gen::pool_ptr<DerivedComponent> ComponentFactory::CreateComponent()
{
	IComponentFactory * pFactory = GetComponentFactory<DerivedComponent>().get();

	assert( pFactory );

	return pFactory->VCreateComponent();
}

template< typename DerivedComponent >
gen::pool_ptr<DerivedComponent> ComponentFactory::CreateComponent( text::GfigElementA * pGFig_p )
{
	IComponentFactory * pFactory = GetComponentFactory<DerivedComponent>().get();

	assert( pFactory );

	return pFactory->VCreateComponent( pGFig_p );
}


template< typename DerivedComponent >
gen::pool_ptr<DerivedComponent> ComponentFactory::CloneComponent( const DerivedComponent * pCompo_p )
{
	IComponentFactory * pFactory = GetComponentFactory<DerivedComponent>().get();

	assert( pFactory );

	return pFactory->VCloneComponent( pCompo_p );
}


template< typename DerivedComponent >
shared_IComponentFactory_ptr ComponentFactory::GetComponentFactory()
{
	for( uint32_t it = 0, size = (uint32_t)m_registry.size();
		it < size;
		++ it ){

			if( m_registry[it].compoType == COMPONENT_TYPE(DerivedComponent) ){

				return m_registry[it].pCompoFactory;
			}
	}

	return nullptr;
}

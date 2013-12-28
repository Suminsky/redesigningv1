#include "ComponentFactory.h"

using namespace game;

template< typename DerivedComponent >
void game::ComponentFactory::RegisterFactory( const shared_AComponentFactory_ptr & pFactory_p )
{
	assert( m_registry.emplace( std::make_pair( COMPONENT_TYPE(DerivedComponent), pFactory_p) ).second );
	assert( m_nameToType.emplace( std::make_pair( COMPONENT_NAME(DerivedComponent), COMPONENT_TYPE(DerivedComponent) ) ).second );

	// insert returns a pair of <it, bool> indicating if the insertion took place or there
	// was already an element
}

template< typename DerivedComponent >
gen::pool_ptr<DerivedComponent> game::ComponentFactory::CreateComponent()
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( COMPONENT_TYPE(DerivedComponent) );

	assert( itFound != m_registry.end() );

	shared_AComponentFactory_ptr & pFactory = itFound->second;

	return  pFactory->VCreateComponent();
}

template< typename DerivedComponent >
gen::pool_ptr<DerivedComponent> game::ComponentFactory::CreateComponent( text::GfigElementA * pGFig_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( COMPONENT_TYPE(DerivedComponent) );

	assert( itFound != m_registry.end() );

	shared_AComponentFactory_ptr & pFactory = itFound->second;

	return pFactory->VCreateComponent( pGFig_p );
}

template< typename DerivedComponent >
shared_AComponentFactory_ptr game::ComponentFactory::GetComponentFactory()
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( COMPONENT_TYPE(DerivedComponent) );

	assert( itFound != m_registry.end() );

	return itFound->second;
}

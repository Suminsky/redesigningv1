#include "ComponentFactory.h"

using namespace game;

//========================================================================
// 
//========================================================================

shared_Component_ptr ComponentFactories::CreateComponent( int iType_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find(iType_p);

	assert( itFound != m_registry.end() );

	shared_AComponentFactory_ptr & pFactory = itFound->second;

	return pFactory->VCreateComponent();
}

game::shared_Component_ptr game::ComponentFactories::CreateComponent( const char * szComponent_p )
{
	ComponentNameToTypeMap::iterator itFound = m_nameToType.find(szComponent_p);

	if( itFound != m_nameToType.end() ){
		
		return CreateComponent( itFound->second );
	}
	else{

		return shared_Component_ptr();
	}
}

shared_Component_ptr game::ComponentFactories::CreateComponent( int iType_p, text::GfigElementA * pGFig_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( iType_p );

	assert( itFound != m_registry.end() );

	shared_AComponentFactory_ptr & pFactory = itFound->second;

	return pFactory->VCreateComponent( pGFig_p );
}

game::shared_Component_ptr game::ComponentFactories::CreateComponent( const char * szComponent_p, text::GfigElementA * pGFig_p )
{
	ComponentNameToTypeMap::iterator itFound = m_nameToType.find(szComponent_p);

	if( itFound != m_nameToType.end() ){
		
		return CreateComponent( itFound->second, pGFig_p );
	}
	else{

		return shared_Component_ptr();
	}
}

shared_AComponentFactory_ptr game::ComponentFactories::GetComponentFactory( int iType_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( iType_p );

	assert( itFound != m_registry.end() );

	return itFound->second;
}

#include "ComponentFactory.h"
#include "Component.h"

using namespace game;

//========================================================================
// 
//========================================================================

pool_Component_ptr ComponentFactory::CreateComponent( int iType_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find(iType_p);

	assert( itFound != m_registry.end() );

	shared_IComponentFactory_ptr & pFactory = itFound->second;

	return pFactory->VCreateComponent();
}

game::pool_Component_ptr game::ComponentFactory::CreateComponent( const char * szComponent_p )
{
	ComponentNameToTypeMap::iterator itFound = m_nameToType.find(szComponent_p);

	if( itFound != m_nameToType.end() ){
		
		return CreateComponent( itFound->second );
	}
	else{

		return pool_Component_ptr();
	}
}

pool_Component_ptr game::ComponentFactory::CreateComponent( int iType_p, text::GfigElementA * pGFig_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( iType_p );

	assert( itFound != m_registry.end() );

	shared_IComponentFactory_ptr & pFactory = itFound->second;

	return pFactory->VCreateComponent( pGFig_p );
}

game::pool_Component_ptr game::ComponentFactory::CreateComponent( const char * szComponent_p, text::GfigElementA * pGFig_p )
{
	ComponentNameToTypeMap::iterator itFound = m_nameToType.find(szComponent_p);

	if( itFound != m_nameToType.end() ){
		
		return CreateComponent( itFound->second, pGFig_p );
	}
	else{

		return pool_Component_ptr();
	}
}

shared_IComponentFactory_ptr game::ComponentFactory::GetComponentFactory( int iType_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( iType_p );

	assert( itFound != m_registry.end() );

	return itFound->second;
}

game::pool_Component_ptr game::ComponentFactory::CloneComponent( const Component * pComponent_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( pComponent_p->GetType() );

	assert( itFound != m_registry.end() );

	shared_IComponentFactory_ptr & pFactory = itFound->second;

	return pFactory->VCloneComponent( pComponent_p );
}

void game::ComponentFactory::UpdateComponent( Component * pComponent_p, text::GfigElementA * pGFig_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( pComponent_p->GetType() );

	assert( itFound != m_registry.end() );

	shared_IComponentFactory_ptr & pFactory = itFound->second;

	return pFactory->VUpdateComponent( pComponent_p, pGFig_p );
}

unsigned int game::ComponentFactory::GetCompoTypeFromName( const std::string & szCompoName_p )
{
	ComponentNameToTypeMap::iterator itFound = m_nameToType.find(szCompoName_p);

	assert( itFound != m_nameToType.end() );

	return itFound->second;
}

void game::ComponentFactory::Serialize( const Component * pComponent_p, text::GfigElementA * pGFig_p )
{
	ComponentFactoryRegistry::iterator itFound = m_registry.find( pComponent_p->GetType() );

	assert( itFound != m_registry.end() );

	shared_IComponentFactory_ptr & pFactory = itFound->second;

	pFactory->VSerialize(pComponent_p, pGFig_p);
}

#include "PCHF.h"
#include "ComponentFactory.h"
#include "Component.h"

using namespace game;

//========================================================================
// 
//========================================================================

pool_Component_ptr ComponentFactory::CreateComponent( unsigned int iType_p )
{
	IComponentFactory * pFactory = GetComponentFactory(iType_p).get();

	assert( pFactory );

	return pFactory->VCreateComponent();
}

game::pool_Component_ptr game::ComponentFactory::CreateComponent( const char * szComponent_p )
{
	unsigned int compotype = GetCompoTypeFromName(szComponent_p);
	assert( compotype != INVALID_COMPONENTTYPE);

	return CreateComponent( compotype );
}

pool_Component_ptr game::ComponentFactory::CreateComponent( unsigned int iType_p, text::GfigElementA * pGFig_p )
{
	IComponentFactory * pFactory = GetComponentFactory(iType_p).get();

	assert( pFactory );

	return pFactory->VCreateComponent(pGFig_p);
}

game::pool_Component_ptr game::ComponentFactory::CreateComponent( const char * szComponent_p, text::GfigElementA * pGFig_p )
{
	unsigned int compotype = GetCompoTypeFromName(szComponent_p);
	assert( compotype != INVALID_COMPONENTTYPE);

	return CreateComponent( compotype, pGFig_p );
}


shared_IComponentFactory_ptr game::ComponentFactory::GetComponentFactory( unsigned int iType_p )
{
	for( uint32_t it = 0, size = (uint32_t)m_registry.size();
		it < size;
		++ it ){

			if( m_registry[it].compoType == iType_p ){

				return m_registry[it].pCompoFactory;
			}
	}

	return nullptr;
}


game::pool_Component_ptr game::ComponentFactory::CloneComponent( const Component * pComponent_p )
{
	IComponentFactory * pFactory = GetComponentFactory(pComponent_p->GetType()).get();

	assert( pFactory );

	return pFactory->VCloneComponent( pComponent_p );
}

void game::ComponentFactory::UpdateComponent( Component * pComponent_p, text::GfigElementA * pGFig_p )
{
	IComponentFactory * pFactory = GetComponentFactory(pComponent_p->GetType()).get();

	assert( pFactory );

	return pFactory->VUpdateComponent(  pComponent_p, pGFig_p );
}

unsigned int game::ComponentFactory::GetCompoTypeFromName( const std::string & szCompoName_p )
{
	for( uint32_t it = 0, size = (uint32_t)m_registry.size();
		it < size;
		++ it ){

			if( szCompoName_p == m_registry[it].szCompoName ){

				return m_registry[it].compoType;
			}
	}

	return INVALID_COMPONENTTYPE;
}

void game::ComponentFactory::Serialize( const Component * pComponent_p, text::GfigElementA * pGFig_p )
{
	IComponentFactory * pFactory = GetComponentFactory(pComponent_p->GetType()).get();

	assert( pFactory );

	return pFactory->VSerialize(pComponent_p, pGFig_p);
}
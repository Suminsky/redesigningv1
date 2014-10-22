#include "ObjectFactory.h"
#include "Object.h"
#include "Layer.h"
#include "../namespace text/TextParser.h"

using namespace game;

shared_Object_ptr AObjectFactory::VCreateObject(){

	return std::make_shared<Object>();
}

shared_Object_ptr AObjectFactory::VCreateObject( text::GfigElementA * pGfig_p ){

	shared_Object_ptr pObj = std::make_shared<Object>();

	for( int it = 0, iSize = (int)pGfig_p->m_subElements.size();
		it<iSize;
		++it ){

		text::GfigElementA & gfigComponent = pGfig_p->m_subElements[it];

		pool_Component_ptr pCompo = m_pLayerOwner->m_componentFactory.CreateComponent( gfigComponent.m_name.c_str(), &gfigComponent );
		if( pCompo ){

			pObj->AttachComponent( std::move(pCompo) );
		}
		else
			BREAKHERE;
	}

	return pObj;
}

game::shared_Object_ptr game::AObjectFactory::VCloneObject( const Object * pObj_p )
{
	shared_Object_ptr pObj = std::make_shared<Object>();

	for( int itCompo = 0, nCompos = (int)pObj_p->m_components.size(); itCompo < nCompos; ++itCompo ){

		pool_Component_ptr pCompo = m_pLayerOwner->m_componentFactory.CloneComponent( pObj_p->m_components[itCompo].Get() );
		if( pCompo ){

			pObj->AttachComponent( std::move(pCompo) );
		}
		else
			BREAKHERE;
	}

	return pObj;
}

void game::AObjectFactory::LoadNewPrefab( text::GfigElementA * pGfig_p )
{
	Prefab newPrefab = { pGfig_p->m_value, VCreateObject( pGfig_p ) };

	m_prefabs.push_back( std::move( newPrefab ) );
}

void game::AObjectFactory::LoadNewPrefab( const Object * pObj_p, const std::string & szName_p )
{
	Prefab newPrefab = { szName_p, VCloneObject( pObj_p ) };

	m_prefabs.push_back( std::move( newPrefab ) );
}

game::shared_Object_ptr game::AObjectFactory::CreateObjectFromPrefab( unsigned int iPrefab_p )
{
	assert( iPrefab_p < m_prefabs.size() );

	return VCloneObject( m_prefabs[iPrefab_p].pObj.get() );
}

game::shared_Object_ptr game::AObjectFactory::CreateObjectFromPrefab( const std::string & szName_p )
{
	return CreateObjectFromPrefab( PrefabIndexFromName(szName_p) );
}

game::shared_Object_ptr game::AObjectFactory::CreateObjectFromPrefab( unsigned int iPrefab_p, text::GfigElementA * pIntanceGfig_p )
{
	shared_Object_ptr pObj = CreateObjectFromPrefab(iPrefab_p);

	for( int it = 0, iSize = (int)pIntanceGfig_p->m_subElements.size();
		it<iSize;
		++it ){

			text::GfigElementA & gfigComponent = pIntanceGfig_p->m_subElements[it];
			
			unsigned int iCompoType = m_pLayerOwner->m_componentFactory.GetCompoTypeFromName( gfigComponent.m_name );
			
			pool_Component_ptr pCompo = pObj->GetFirstOfComponent( iCompoType );

			if( pCompo ){

				// update

				m_pLayerOwner->m_componentFactory.UpdateComponent( pCompo.Get(), &gfigComponent );
			}
			else{

				// create

				pCompo = m_pLayerOwner->m_componentFactory.CreateComponent( iCompoType, &gfigComponent );
				assert(pCompo );

				pObj->AttachComponent( std::move(pCompo) );
			}
	}

	return pObj;
}

game::shared_Object_ptr game::AObjectFactory::CreateObjectFromPrefab( const std::string & szName_p, text::GfigElementA * pIntanceGfig_p )
{
	return CreateObjectFromPrefab( PrefabIndexFromName(szName_p), pIntanceGfig_p );
}

unsigned int game::AObjectFactory::PrefabIndexFromName( const std::string & szName_p )
{
	for( unsigned int it = 0, nPrefabs = (unsigned)m_prefabs.size(); it < nPrefabs; ++it ){

		if( szName_p == m_prefabs[it].szName )
			return it;
	}

	assert(0);
	return (unsigned)-1;
}

#include "ObjectFactory.h"
#include "Object.h"
#include "Layer.h"
#include "../namespace text/TextParser.h"

using namespace game;
using namespace text;

pool_Object_ptr ObjectFactory::CreateObject(){

	return pool_Object_ptr(m_pool);
}

pool_Object_ptr ObjectFactory::CreateObject( text::GfigElementA * pGfig_p ){

	pool_Object_ptr pObj(m_pool);

	strcpy_s<64>( pObj->m_szName, pGfig_p->m_name.c_str() );

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

pool_Object_ptr game::ObjectFactory::CloneObject( const Object * pObj_p )
{
	pool_Object_ptr pObj(m_pool);

	strcpy_s<64>( pObj->m_szName, pObj_p->m_szName );

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

void game::ObjectFactory::LoadNewPrefab( text::GfigElementA * pGfig_p )
{
	Prefab newPrefab = { pGfig_p->m_value, CreateObject( pGfig_p ) };

	m_prefabs.push_back( std::move( newPrefab ) );
}

void game::ObjectFactory::LoadNewPrefab( const Object * pObj_p, const std::string & szName_p )
{
	Prefab newPrefab = { szName_p, CloneObject( pObj_p ) };

	m_prefabs.push_back( std::move( newPrefab ) );
}

game::pool_Object_ptr game::ObjectFactory::CreateObjectFromPrefab( unsigned int iPrefab_p )
{
	assert( iPrefab_p < m_prefabs.size() );

	return CloneObject( m_prefabs[iPrefab_p].pObj.Get() );
}

game::pool_Object_ptr game::ObjectFactory::CreateObjectFromPrefab( const std::string & szName_p )
{
	return CreateObjectFromPrefab( PrefabIndexFromName(szName_p) );
}

game::pool_Object_ptr game::ObjectFactory::CreateObjectFromPrefab( unsigned int iPrefab_p, text::GfigElementA * pIntanceGfig_p )
{
	pool_Object_ptr pObj = CreateObjectFromPrefab(iPrefab_p);

	strcpy_s<64>( pObj->m_szName, pIntanceGfig_p->m_name.c_str() );

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

game::pool_Object_ptr game::ObjectFactory::CreateObjectFromPrefab( const std::string & szName_p, text::GfigElementA * pIntanceGfig_p )
{
	return CreateObjectFromPrefab( PrefabIndexFromName(szName_p), pIntanceGfig_p );
}

unsigned int game::ObjectFactory::PrefabIndexFromName( const std::string & szName_p )
{
	for( unsigned int it = 0, nPrefabs = (unsigned)m_prefabs.size(); it < nPrefabs; ++it ){

		if( szName_p == m_prefabs[it].szName )
			return it;
	}

	assert(0);
	return (unsigned)-1;
}
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void game::ObjectFactory::Serialize( const Object * pObj_p, text::GfigElementA * pGFig_p )
{
	pGFig_p->m_subElements.emplace_back( GfigElementA(pObj_p->m_szName) );

	int nCompos = (int)pObj_p->m_components.size();

	for( int itCompo = 0 ; itCompo < nCompos; ++itCompo ){

		Component * pCompo = pObj_p->m_components[itCompo].Get();

		m_pLayerOwner->m_componentFactory.Serialize( pCompo, &pGFig_p->m_subElements.back() );
	}
}

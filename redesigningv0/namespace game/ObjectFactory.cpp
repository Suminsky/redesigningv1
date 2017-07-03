#include "PCHF.h"
#include "ObjectFactory.h"
#include "Object.h"
#include "Layer.h"
#include "../namespace text/TextParser.h"
#include "../Legalia/AABB2DColliderComponent.h"

using namespace game;
using namespace text;

pool_Object_ptr ObjectFactory::CreateObject(){

	return pool_Object_ptr(m_pool);
}

pool_Object_ptr ObjectFactory::CreateObject( text::GfigElementA * pGfig_p ){

	pool_Object_ptr pObj(m_pool);

	strcpy_s<64>( pObj->m_szName, pGfig_p->m_name.c_str() );
	pObj->m_prefab = -1;
	//pObj->m_prefab = pGfig_p->m_value.c_str() ? atoi(pGfig_p->m_value.c_str()) : -1;

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
	pObj->m_prefab = pObj_p->m_prefab;

	for( int itCompo = 0, nCompos = (int)pObj_p->m_components.Size(); itCompo < nCompos; ++itCompo ){

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
	Prefab newPrefab = { pGfig_p->m_name, CreateObject( pGfig_p ) };
	newPrefab.pObj->m_prefab = (int)m_prefabs.size();

	m_prefabs.push_back( std::move( newPrefab ) );
}

void game::ObjectFactory::LoadNewPrefab( const Object * pObj_p, const std::string & szName_p )
{
	Prefab newPrefab = { szName_p, CloneObject( pObj_p ) };
	newPrefab.pObj->m_prefab = (int)m_prefabs.size();

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
	pObj->m_prefab = (int)iPrefab_p;

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

	// NOTE:
	// saving an object that uses a prefab should optmize out data that equals the prefab.
	// currently, data discarded on the gfigs consider only the defaults..
	// so, for example:
	// default color = rgba(1,1,1,1)
	// prefab color = rgba(1,1,0.5,0.5)
	// obj color = rgba(1,1,0.5,1)
	// 
	// prefab gfig = ba(0.5,0.5)
	// obj gfig = a(1)
	// the only way is create a new factory function for components, that receives a prefab compo for reference
	// TODO

	
	int nCompos = (int)pObj_p->m_components.Size();

	if( pObj_p->m_prefab == -1 ){

		pGFig_p->m_subElements.emplace_back(
			GfigElementA(pObj_p->m_szName,  "" )
			);

		for( int itCompo = 0 ; itCompo < nCompos; ++itCompo ){

			Component * pCompo = pObj_p->m_components[itCompo].Get();

			m_pLayerOwner->m_componentFactory.Serialize( pCompo, &pGFig_p->m_subElements.back() );
		}
	}
	else{

		pGFig_p->m_subElements.emplace_back(
			GfigElementA(pObj_p->m_szName,   pObj_p->m_prefab )
			);

		Object * pPrefab = m_prefabs[pObj_p->m_prefab].pObj.Get();
		//int nPrefCompos = (int)pPrefab->m_components.Size();

		for( int itCompo = 0 ; itCompo < nCompos; ++itCompo ){

			Component * pCompo = pObj_p->m_components[itCompo].Get();
			Component * pPrefCompo = pPrefab->GetFirstOfComponent(pCompo->GetType()).Get();				

			// how to solve multiple of the same compo type? new int on compo?

			if( pCompo->GetType() == COMPONENT_TYPE(AABB2DColliderComponent)
				||
				pCompo->GetType() == COMPONENT_TYPE(TransformComponent)
				||
				pCompo->GetType() == COMPONENT_TYPE(SpriteComponent_)
				||
				pCompo->GetType() == COMPONENT_TYPE(SpriteAnimCompo_)){ // testing just with AABB2d

				if( pPrefCompo != nullptr ){

					m_pLayerOwner->m_componentFactory.Serialize( pCompo, pPrefCompo, &pGFig_p->m_subElements.back() );
				}
				else{

					m_pLayerOwner->m_componentFactory.Serialize(
						pCompo,
						m_pLayerOwner->m_componentFactory.GetDefaultCompo(pCompo->GetType()),
						&pGFig_p->m_subElements.back() );
				}
			}
			else{

				m_pLayerOwner->m_componentFactory.Serialize( pCompo, &pGFig_p->m_subElements.back() );
			}
		}
	}
}

void game::ObjectFactory::SerializePrefabs( text::GfigElementA * pGFig_p )
{
	for( uint32_t it = 0, n = (uint32_t)m_prefabs.size();
		it < n; ++ it ){

		//Serialize( m_prefabs[it].pObj.Get(), pGFig_p );

		const Object * pObj_p = m_prefabs[it].pObj.Get();

		pGFig_p->m_subElements.emplace_back(
			GfigElementA(pObj_p->m_szName, it )
			);

		int nCompos = (int)pObj_p->m_components.Size();

		for( int itCompo = 0 ; itCompo < nCompos; ++itCompo ){

			Component * pCompo = pObj_p->m_components[itCompo].Get();

			m_pLayerOwner->m_componentFactory.Serialize( pCompo, &pGFig_p->m_subElements.back() );
		}
	}
}
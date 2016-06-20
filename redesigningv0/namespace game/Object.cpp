#include "Object.h"
#include "Layer.h"

using namespace game;

void game::Object::AttachComponent( pool_Component_ptr && pComponent_p )
{
	assert( pComponent_p->m_bDettached );

	pComponent_p->m_bDettached = false;

	if( pComponent_p->m_currentIndexOnObject == INVALID_COMPONENTINDEX 
		||
		pComponent_p->m_pObjectOwner != this ){

			pComponent_p->m_currentIndexOnObject = (COMPONENTINDEX)m_components.Size();
			pComponent_p->m_pObjectOwner = this;

			pComponent_p->VOnAttach();

			m_components.PushBack( std::move(pComponent_p) );
	}
}

void game::Object::AttachComponent( const pool_Component_ptr & pComponent_p )
{
	assert( pComponent_p->m_bDettached );

	pComponent_p->m_bDettached = false;

	if( pComponent_p->m_currentIndexOnObject == INVALID_COMPONENTINDEX 
		||
		pComponent_p->m_pObjectOwner != this ){

			pComponent_p->m_currentIndexOnObject = (COMPONENTINDEX)m_components.Size();
			pComponent_p->m_pObjectOwner = this;

			pComponent_p->VOnAttach();

			m_components.PushBack( pComponent_p );

			//m_pLayerOwner->InformSystemsAboutComponentAddedToObject(pComponent_p);
	}
}

void game::Object::DettachComponent( COMPONENTINDEX componentCurrentIndex_p )
{
	DettachComponent( m_components[componentCurrentIndex_p].Get() );
}

void game::Object::DettachComponent( const pool_Component_ptr & pComponent_p )
{
	DettachComponent(pComponent_p.Get());
}

void game::Object::DettachComponent( Component * pComponent_p )
{
	assert( pComponent_p->m_currentIndexOnObject != INVALID_COMPONENTINDEX );
	assert( !pComponent_p->m_bDettached );

	pComponent_p->m_bDettached = true;
	pComponent_p->VOnDetach(); // new

	m_removedComponents.PushBack(pComponent_p);
}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void game::Object::CleanRemovedComponents()
{
	unsigned int nRemoved =	(unsigned int)m_removedComponents.Size(); // cache
	unsigned int nCompos =		(unsigned int)m_components.Size();

	int nSkipped = 0;
	for( unsigned int itR = 0, itLast = (nCompos-1); itR < nRemoved; ++itR ){

		if( !m_removedComponents[itR]->m_bDettached ){

			++nSkipped;
			continue;
		}

		if( m_removedComponents[itR]->m_currentIndexOnObject == itLast ){

			m_removedComponents[itR]->m_currentIndexOnObject = INVALID_OBJECTINDEX;
			m_components[itLast].Release();
			--itLast;
			continue; // already "swapped"
		}

		std::swap( m_components[m_removedComponents[itR]->m_currentIndexOnObject], m_components[itLast] );
		m_components[itLast].Release();
		--itLast;

		// update the index of the swapped object (not the one sent to pop)

		m_components[m_removedComponents[itR]->m_currentIndexOnObject]->m_currentIndexOnObject =
			m_removedComponents[itR]->m_currentIndexOnObject;

		// since the removed vector store pointers, theres no dirt data to update
		// but we need to invalidate the discarded, cause its used as check when adding and removing..that
		// can be discarded TODO

		m_removedComponents[itR]->m_currentIndexOnObject = INVALID_OBJECTINDEX;
		//m_removedComponents[itR]->VOnDetach();
	}

	// "trim"

	nRemoved -= nSkipped;
	m_components.Unstack(nRemoved);

	m_removedComponents.Reset();
}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void game::Object::RegisterForComponentEvent( EventMachine<ComponentEventData>::EventHandler eveHandlerDelegate_p, EventType eveType_p )
{
	m_objectEventMachine.RegisterForEvent( eveHandlerDelegate_p, eveType_p );
}

void game::Object::UnregisterForComponentEvent( EventMachine<ComponentEventData>::EventHandler eveHandlerDelegate_p, EventType eveType_p )
{
	m_objectEventMachine.UnregisterForEvent( eveHandlerDelegate_p, eveType_p );
}

void game::Object::AddComponentEvent( EventType eveType_p, ComponentEventData eveData_p )
{
	m_objectEventMachine.AddEvent( eveType_p, eveData_p );
}

void game::Object::DispatchComponentEvents()
{
	m_objectEventMachine.DispatchEvents();
}

void game::Object::DispatchComponentEventImmediately( EventType eveType_p, ComponentEventData eveData_p )
{
	m_objectEventMachine.DispatchEventImmetiately( eveType_p, eveData_p );
}

void game::Object::Dettach()
{
	assert( m_pLayerOwner );
	m_pLayerOwner->RemoveObject(this);
}

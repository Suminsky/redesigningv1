#include "Object.h"
#include "Layer.h"

using namespace game;

void game::Object::AttachComponent( pool_Component_ptr && pComponent_p )
{
	assert( pComponent_p->m_bDetached );

	pComponent_p->m_bDetached = false;

	if( pComponent_p->m_currentComponentObjectIndex == INVALID_COMPONENTINDEX 
		||
		pComponent_p->m_pObjectOwner != this ){

			pComponent_p->m_currentComponentObjectIndex = (COMPONENTINDEX)m_components.size();
			pComponent_p->m_pObjectOwner = this;

			pComponent_p->VOnAttach();

			m_components.push_back( std::move(pComponent_p) );

			//m_pLayerOwner->InformSystemsAboutComponentAddedToObject( m_components.back());
	}
}

void game::Object::AttachComponent( const pool_Component_ptr & pComponent_p )
{
	assert( pComponent_p->m_bDetached );

	pComponent_p->m_bDetached = false;

	if( pComponent_p->m_currentComponentObjectIndex == INVALID_COMPONENTINDEX 
		||
		pComponent_p->m_pObjectOwner != this ){

			pComponent_p->m_currentComponentObjectIndex = (COMPONENTINDEX)m_components.size();
			pComponent_p->m_pObjectOwner = this;

			pComponent_p->VOnAttach();

			m_components.push_back( pComponent_p );

			//m_pLayerOwner->InformSystemsAboutComponentAddedToObject(pComponent_p);
	}
}

void game::Object::DettachComponent( COMPONENTINDEX componentCurrentIndex_p )
{
	assert( componentCurrentIndex_p != INVALID_COMPONENTINDEX );
	assert( !m_components[componentCurrentIndex_p]->m_bDetached );

	m_components[componentCurrentIndex_p]->m_bDetached = true;

	m_removedComponents.push_back(componentCurrentIndex_p);
}

void game::Object::DettachComponent( const pool_Component_ptr & pComponent_p )
{
	DettachComponent(pComponent_p->m_currentComponentObjectIndex);
}

void game::Object::DettachComponent( const Component * pComponent_p )
{
	DettachComponent(pComponent_p->m_currentComponentObjectIndex);
}

void game::Object::CleanRemovedComponents()
{
	unsigned int nRemoved =	(unsigned int)m_removedComponents.size(); // cache
	unsigned int nComponents =		(unsigned int)m_components.size();

	for( unsigned int itRemoved = 0, itLast = nComponents - 1;
		itRemoved < nRemoved;
		++itRemoved, --itLast ){

			if( !m_components[m_removedComponents[itRemoved]]->m_bDetached ){

				--nRemoved;
				continue; // untested
			}

			m_components[m_removedComponents[itRemoved]]->VOnDetach();
			m_components[m_removedComponents[itRemoved]]->m_currentComponentObjectIndex = INVALID_COMPONENTINDEX;
			m_components[m_removedComponents[itRemoved]]->m_pObjectOwner = nullptr;

			if( m_removedComponents[itRemoved] == itLast ){

				continue;
			}

			std::swap( m_components[m_removedComponents[itRemoved]], m_components[itLast] );

			if( m_components[m_removedComponents[itRemoved]]->m_bDetached ){

				// find the swaped task on the list to be destroyed, update the index

				for( unsigned int itToBeDestroyed = itRemoved; itToBeDestroyed < nRemoved; ++itToBeDestroyed ){

					if( m_removedComponents[itToBeDestroyed] == m_components[m_removedComponents[itRemoved]]->m_currentComponentObjectIndex ){

						m_removedComponents[itToBeDestroyed] = m_removedComponents[itRemoved];
					}
				}
			}
			else{

				m_components[m_removedComponents[itRemoved]]->m_currentComponentObjectIndex = m_removedComponents[itRemoved];
			}
	}

	// "trim"

	m_components.resize(nComponents - nRemoved);

	m_removedComponents.clear();
}

void game::Object::RegisterForComponentEvent( EventMachine<ComponentEventData>::EventHandlerDelegate eveHandlerDelegate_p, EventType eveType_p )
{
	m_objectEventMachine.RegisterForEvent( eveHandlerDelegate_p, eveType_p );
}

void game::Object::UnregisterForComponentEvent( EventMachine<ComponentEventData>::EventHandlerDelegate eveHandlerDelegate_p, EventType eveType_p )
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
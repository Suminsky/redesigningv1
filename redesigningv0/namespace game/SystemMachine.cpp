#include "PCHF.h"
#include "SystemMachine.h"

using namespace game;

void SystemMachine::AddSystem( shared_System_ptr && pSystem_p )
{
	assert( pSystem_p->m_bDead );

	pSystem_p->m_bDead = false;

	if( pSystem_p->m_currentSystemIndex == INVALID_SYSTEMINDEX
		||
		pSystem_p->m_pSysMachineOwner != this ){

			pSystem_p->m_currentSystemIndex = (SYSTEMINDEX)m_systems.size();
			pSystem_p->m_pSysMachineOwner = this;

			pSystem_p->VOnInit();

			m_systems.push_back( std::move(pSystem_p) );
	}
}

void SystemMachine::AddSystem( const shared_System_ptr & pSystem_p )
{
	assert( pSystem_p->m_bDead );

	pSystem_p->m_bDead = false;

	if( pSystem_p->m_currentSystemIndex == INVALID_SYSTEMINDEX
		||
		pSystem_p->m_pSysMachineOwner != this ){

			pSystem_p->m_currentSystemIndex = (SYSTEMINDEX)m_systems.size();
			pSystem_p->m_pSysMachineOwner = this;

			pSystem_p->VOnInit();

			m_systems.push_back( pSystem_p );
	}
}

void SystemMachine::RemoveSystem( SYSTEMINDEX systemCurrentIndex_p )
{
	assert( systemCurrentIndex_p != INVALID_SYSTEMINDEX );
	assert( !m_systems[systemCurrentIndex_p]->m_bDead );

	m_systems[systemCurrentIndex_p]->m_bDead = true;

	m_removedSystems.push_back( systemCurrentIndex_p );
}

void SystemMachine::RemoveSystem( const shared_System_ptr & pSystem_p )
{
	RemoveSystem(pSystem_p->m_currentSystemIndex);
}

void SystemMachine::RemoveSystem( const System * pSystem_p )
{
	RemoveSystem(pSystem_p->m_currentSystemIndex);
}

void SystemMachine::CleanRemovedSystems()
{
	unsigned int nRemoved =	(unsigned int)m_removedSystems.size(); // cache
	unsigned int nSystems =	(unsigned int)m_systems.size();

	for( unsigned int itRemoved = 0, itLast = nSystems - 1;
		itRemoved < nRemoved;
		++itRemoved, --itLast ){

			if( !m_systems[m_removedSystems[itRemoved]]->m_bDead ){

				--nRemoved;
				continue; // untested
			}

			m_systems[m_removedSystems[itRemoved]]->VOnDestroy();
			m_systems[m_removedSystems[itRemoved]]->m_currentSystemIndex = INVALID_SYSTEMINDEX;
			m_systems[m_removedSystems[itRemoved]]->m_pSysMachineOwner = nullptr;

			if( m_removedSystems[itRemoved] == itLast ){

				continue;
			}

			std::swap( m_systems[m_removedSystems[itRemoved]], m_systems[itLast] );

			if( m_systems[m_removedSystems[itRemoved]]->m_bDead ){

				// find the swapped task on the list to be destroyed, update the index

				for( unsigned int itToBeDestroyed = itRemoved; itToBeDestroyed < nRemoved; ++itToBeDestroyed ){

					if( m_removedSystems[itToBeDestroyed] == m_systems[m_removedSystems[itRemoved]]->m_currentSystemIndex ){

						m_removedSystems[itToBeDestroyed] = m_removedSystems[itRemoved];
					}
				}
			}
			else{

				m_systems[m_removedSystems[itRemoved]]->m_currentSystemIndex = m_removedSystems[itRemoved];
			}
	}

	// "trim"

	m_systems.resize(nSystems - nRemoved);

	m_removedSystems.clear();
}

//========================================================================
// event stuff
//========================================================================

void SystemMachine::RegisterForSystemEvent( EventMachine<SystemEventData>::EventHandler eveHandlerDelegate_p, EventType eveType_p )
{
	m_SystemEventMachine.RegisterForEvent( eveHandlerDelegate_p, eveType_p );
}

void SystemMachine::UnregisterForSystemEvent( EventMachine<SystemEventData>::EventHandler eveHandlerDelegate_p, EventType eveType_p )
{
	m_SystemEventMachine.UnregisterForEvent( eveHandlerDelegate_p, eveType_p );
}

void SystemMachine::AddSystemEvent( EventType eveType_p, SystemEventData eveData_p )
{
	m_SystemEventMachine.AddEvent( eveType_p, eveData_p );
}

void SystemMachine::DispatchSystemEventImmediately( EventType eveType_p, SystemEventData eveData_p )
{
	m_SystemEventMachine.DispatchEventImmetiately( eveType_p, eveData_p );
}

void SystemMachine::DispatchSystemEvents()
{
	m_SystemEventMachine.DispatchEvents();
}
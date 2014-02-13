#include "ObjectMachine.h"

using namespace game;

void ObjectMachine::AddObject( shared_Object_ptr && pObject_p )
{
	assert( pObject_p->m_bDettached );

	pObject_p->m_bDettached = false;

	if( pObject_p->m_currentObjectIndex == INVALID_OBJECTINDEX
		||
		pObject_p->m_pObjMachineOwner != this ){

			pObject_p->m_currentObjectIndex = (OBJECTINDEX)m_objects.size();
			pObject_p->m_pObjMachineOwner = this;
			pObject_p->m_pLayerOwner = m_pLayerRef;

			//pObject_p->VOnInit();

			m_objects.push_back( std::move(pObject_p) );

			//InformSystemsAboutObjectAdded( pObject_p->m_currentObjectIndex );
			// TODO: is adding on the same frame..shouldnt it be at the end of frame?
	}
}

void ObjectMachine::AddObject( const shared_Object_ptr & pObject_p )
{
	assert( pObject_p->m_bDettached );

	pObject_p->m_bDettached = false;

	if( pObject_p->m_currentObjectIndex == INVALID_OBJECTINDEX
		||
		pObject_p->m_pObjMachineOwner != this ){

			pObject_p->m_currentObjectIndex = (OBJECTINDEX)m_objects.size();
			pObject_p->m_pObjMachineOwner = this;
			pObject_p->m_pLayerOwner = m_pLayerRef;

			//pObject_p->VOnInit();

			m_objects.push_back( pObject_p );

			//InformSystemsAboutObjectAdded( pObject_p->m_currentObjectIndex );
	}
}

void ObjectMachine::RemoveObject( OBJECTINDEX objectCurrentIndex_p )
{
	assert( objectCurrentIndex_p != INVALID_OBJECTINDEX );
	assert( !m_objects[objectCurrentIndex_p]->m_bDettached );

	m_objects[objectCurrentIndex_p]->m_bDettached = true;

	m_removedObjects.push_back( objectCurrentIndex_p );
}

void ObjectMachine::RemoveObject( const shared_Object_ptr & pObject_p )
{
	RemoveObject(pObject_p->m_currentObjectIndex);
}

void ObjectMachine::RemoveObject( const Object * pObject_p )
{
	RemoveObject(pObject_p->m_currentObjectIndex);
}

void ObjectMachine::CleanRemovedObjects()
{
	unsigned int nRemoved =	(unsigned int)m_removedObjects.size(); // cache
	unsigned int nObjects =		(unsigned int)m_objects.size();

	for( unsigned int itRemoved = 0, itLast = nObjects - 1;
		itRemoved < nRemoved;
		++itRemoved, --itLast ){

			if( !m_objects[m_removedObjects[itRemoved]]->m_bDettached ){

				--nRemoved;
				continue; // untested
			}

			m_objects[m_removedObjects[itRemoved]]->m_currentObjectIndex = INVALID_OBJECTINDEX;
			m_objects[m_removedObjects[itRemoved]]->m_pObjMachineOwner = nullptr;
			m_objects[m_removedObjects[itRemoved]]->m_pLayerOwner = nullptr;

			if( m_removedObjects[itRemoved] == itLast ){

				continue;
			}

			std::swap( m_objects[m_removedObjects[itRemoved]], m_objects[itLast] );

			if( m_objects[m_removedObjects[itRemoved]]->m_bDettached ){

				// find the swapped task on the list to be destroyed, update the index

				for( unsigned int itToBeDestroyed = itRemoved; itToBeDestroyed < nRemoved; ++itToBeDestroyed ){

					if( m_removedObjects[itToBeDestroyed] == m_objects[m_removedObjects[itRemoved]]->m_currentObjectIndex ){

						m_removedObjects[itToBeDestroyed] = m_removedObjects[itRemoved];
					}
				}
			}
			else{

				m_objects[m_removedObjects[itRemoved]]->m_currentObjectIndex = m_removedObjects[itRemoved];
			}
	}

	// "trim"

	m_objects.resize(nObjects - nRemoved);

	m_removedObjects.clear();
}

void ObjectMachine::CleanRemovedObjectComponents()
{
	for( int itObject = 0, itSize = (int)m_objects.size();
		itObject < itSize;
		++itObject ){

			if( !m_objects[itObject]->m_removedComponents.empty() )
				m_objects[itObject]->CleanRemovedComponents();
	}
}

void ObjectMachine::DispatchObjectInternalEvents(){

	for( int itObject = 0, itSize = (int)m_objects.size();
		 itObject < itSize;
		 ++itObject ){

			m_objects[itObject]->DispatchComponentEvents();

			//if( !m_objects[itObject]->m_removedComponents.empty() ) // not sure if here (TODO)
			//	m_objects[itObject]->CleanRemovedComponents();
	}
};
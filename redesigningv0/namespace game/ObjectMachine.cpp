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
			// 
			// TODO: is adding on the same frame..shouldnt it be at the end of frame?
			// that worked before, but now systems updates components, not objects, so
			// consequently new objects (its compos) will be updated in the frame theyr added in, not
			// the planned behavior =(
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

			m_objects.push_back( pObject_p );
	}
}

void ObjectMachine::RemoveObject( OBJECTINDEX objectCurrentIndex_p )
{
	RemoveObject( m_objects[objectCurrentIndex_p].get() );
}

void ObjectMachine::RemoveObject( const shared_Object_ptr & pObject_p )
{
	RemoveObject(pObject_p.get());
}

void ObjectMachine::RemoveObject( Object * pObject_p )
{
	assert( pObject_p->m_currentObjectIndex != INVALID_OBJECTINDEX );
	assert( !pObject_p->m_bDettached );

	pObject_p->m_bDettached = true;

	m_removedObjects.push_back( pObject_p );
}

void ObjectMachine::CleanRemovedObjects()
{
	// this method swaps all removed objs to the end of the vector, updating theyr indexes, than pops all together

	unsigned int nRemoved =	(unsigned int)m_removedObjects.size(); // cache
	unsigned int nObjects =	(unsigned int)m_objects.size();
	
	int nSkipped = 0;
	for( unsigned int itR = 0, itLast = (nObjects-1); itR < nRemoved; ++itR ){

		if( !m_removedObjects[itR]->m_bDettached ){

			++nSkipped;
			continue;
		}

		// since the removed vector store pointers, theres no dirt data to update
		// but we need to invalidate the discarded, cause its used as check (assertions) when adding and removing..that
		// can be discarded TODO
		
		OBJECTINDEX removedIndex = m_removedObjects[itR]->m_currentObjectIndex;
		m_removedObjects[itR]->m_currentObjectIndex = INVALID_OBJECTINDEX;

		if( removedIndex == itLast ){
			
			--itLast;
			continue; // already "swapped"
		}

		std::swap( m_objects[removedIndex], m_objects[itLast] );// assertion triggered TWICE: m_removedObjects[itR]->m_currentObjectIndex was INVALID_OBJECTINDEX here(how the fuk can that be possible)
		--itLast;

		// update the index of the swapped object (not the one sent to pop)

		m_objects[removedIndex]->m_currentObjectIndex = removedIndex;
	}

	// "trim"

	nRemoved -= nSkipped;
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
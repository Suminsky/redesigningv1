#include "DrawablesQueue.h"
#include <algorithm>


void render::DrawablesQueue::Submit( Drawable & drawable_p )
{
	m_drawables.push_back( drawable_p );

	Entry newEntry = {drawable_p.GetSortKey(), ((int)m_drawables.size())-1};
	m_sortqueue.push_back(newEntry);
}

void render::DrawablesQueue::Submit( Drawable && drawable_p )
{
	m_drawables.emplace_back( std::move(drawable_p) );

	Entry newEntry = {drawable_p.GetSortKey(), ((int)m_drawables.size())-1};
	m_sortqueue.push_back(newEntry);
}

void render::DrawablesQueue::CreateCommandBuffer( RenderCommands & commandList_p, bool bClearStateCache_p )
{
	if( bClearStateCache_p){
		ZeroMemory(m_stateCache, sizeof(dx::Binder*)*dx::E_MAX_BINDS);
	}

	//Prepare();

	// sort drawables

	std::sort(m_sortqueue.begin(), m_sortqueue.end(), Entry());

	// traverse "sorted drawables"

	for(	int itSorted = 0, sortedSize = (int)m_sortqueue.size();
			itSorted < sortedSize;
			++ itSorted ){

		const vStatePtrs & stateGroup = m_drawables[m_sortqueue[itSorted].index].m_vStatePtrs;

		// traverse pipe states

		UINT64 bindsSetMask = 0;

		for(	int itStates = 0, statesSize = (int)stateGroup.size();
				itStates < statesSize;
				++ itStates ){

			// traverse bind commands

			const dx::vBinderPtrs & binds = stateGroup[itStates]->m_vBinderPtrs;
			
			for(	int itBinds = 0, bindsSize = (int)binds.size();
					itBinds < bindsSize;
					++itBinds ){

					// check redundancy: needs to check precedences redundancy between pipe states binds of the same drawable

					if( !(bindsSetMask & binds[itBinds]->TypeBits())	// check if bind was not set by previous states*
						&&
						m_stateCache[binds[itBinds]->TypeIndex()] != binds[itBinds] ){		// check if bind not already set

						bindsSetMask |= binds[itBinds]->TypeBits();
						m_stateCache[binds[itBinds]->TypeIndex()] =  binds[itBinds];

						commandList_p.push_back( binds[itBinds] );
					}

					// * NOTE that later binds are discarded, so to add default binds that may be discarded if drawable
					// is setting a specific bind, it must came later on the states, not before
			}

		}

		// add draw call

		commandList_p.push_back( m_drawables[m_sortqueue[itSorted].index].m_pDrawCall );
	}

	//Prepare();
}

void render::DrawablesQueue::Prepare()
{
	m_drawables.clear();
	m_sortqueue.clear();
}

void render::DrawablesQueue::ResetState( dx::E_BIND eBind_p )
{
	m_stateCache[eBind_p] = NULL;
}
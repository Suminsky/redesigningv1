#include "PCHF.h"

#include "DrawablesQueue.h"
#include <algorithm>
#include <stdint.h>


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

	for(	uint32_t itSorted = 0, sortedSize = (uint32_t)m_sortqueue.size();
			itSorted < sortedSize;
			++ itSorted ){

		const vStatePtrs & stateGroup = m_drawables[m_sortqueue[itSorted].index].m_vStatePtrs;

		// traverse pipe states

		UINT64 bindsSetMask = 0;

		for(	uint32_t itStates = 0, statesSize = (uint32_t)stateGroup.size();
				itStates < statesSize;
				++ itStates ){

			// traverse bind commands

			const dx::vBinderPtrs & binds = stateGroup[itStates]->m_vBinderPtrs;
			
			for(	uint32_t itBinds = 0, bindsSize = (uint32_t)binds.size();
					itBinds < bindsSize;
					++itBinds ){

					// check redundancy: needs to check precedences redundancy between pipe states binds of the same drawable

					if( (bindsSetMask & binds[itBinds]->TypeBits()) != binds[itBinds]->TypeBits() ){	// check if bind was not set by previous states* of the drawable
						
						bindsSetMask |= binds[itBinds]->TypeBits();

						if(	m_stateCache[binds[itBinds]->TypeIndex()] != binds[itBinds] ){		// check if bind not already set

							m_stateCache[binds[itBinds]->TypeIndex()] =  binds[itBinds];

							commandList_p.push_back( binds[itBinds] );
						}
					}
					else{

						int x = 7; x = x;
					}
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
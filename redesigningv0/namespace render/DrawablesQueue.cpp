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
					/*else{
						int x = 0;
					}*/

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
	//m_drawables.resize(0);
	m_sortqueue.clear();
}

void render::DrawablesQueue::ResetState( dx::E_BIND eBind_p )
{
	m_stateCache[eBind_p] = NULL;
}


/*
for( sortarray::const_iterator itDrawable = m_sortqueue.begin(),
		 itEnd = m_sortqueue.end();
		 itDrawable != itEnd; ++itDrawable){

		const vStatePtrs * pStateGroup = m_drawables[(*itDrawable).index].GetPipeStateGroup();

		// traverse pipe states

		UINT64 bindsSetMask = 0;

		for( vStatePtrs::const_iterator itStates = pStateGroup->begin(),
			statesEnd =  pStateGroup->end();
			itStates != statesEnd;
			++itStates ){

			// traverse bind commands
			
		
			for( dx::vBinderPtrs::const_iterator itBinds = (*itStates)->Begin(),
				bindsEnd = (*itStates)->End();
				itBinds != bindsEnd;
				++itBinds ){

					// check redundancy: needs to check precedences redundancy between pipe states binds of the same drawable

					if( !(bindsSetMask & (*itBinds)->TypeBits())	// check if bind was not set by previous states*
						&&
						m_stateCache[(*itBinds)->TypeIndex()] != (*itBinds) ){		// check if bind not already set

						bindsSetMask |= (*itBinds)->TypeBits();
						m_stateCache[(*itBinds)->TypeIndex()] = (*itBinds);
						commandList_p.push_back((*itBinds));
					}

					// * NOTE that later binds are discarded, so to add default binds that may be discarded if drawable
					// is setting a specific bind, it must came later on the states, not before
			}

		}

		// add draw call

		commandList_p.push_back(m_drawables[(*itDrawable).index].GetDrawCall());
	}
 */
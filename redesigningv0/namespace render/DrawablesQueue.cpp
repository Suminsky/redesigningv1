#include "DrawablesQueue.h"
#include <algorithm>

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

		const StatesPtrsVec & stateGroup = m_drawables[m_sortqueue[itSorted].index].m_pipeStateGroup;

		// traverse pipe states

		UINT64 bindsSetMask = 0;

		for(	int itStates = 0, statesSize = (int)stateGroup.size();
				itStates < statesSize;
				++ itStates ){

			// traverse bind commands

			const dx::BindersPtrsVec & binds = stateGroup[itStates]->m_binds;
			
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


/*
for( sortarray::const_iterator itDrawable = m_sortqueue.begin(),
		 itEnd = m_sortqueue.end();
		 itDrawable != itEnd; ++itDrawable){

		const StatesPtrsVec * pStateGroup = m_drawables[(*itDrawable).index].GetPipeStateGroup();

		// traverse pipe states

		UINT64 bindsSetMask = 0;

		for( StatesPtrsVec::const_iterator itStates = pStateGroup->begin(),
			statesEnd =  pStateGroup->end();
			itStates != statesEnd;
			++itStates ){

			// traverse bind commands
			
		
			for( dx::BindersPtrsVec::const_iterator itBinds = (*itStates)->Begin(),
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
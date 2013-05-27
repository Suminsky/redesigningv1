#include "DrawablesQueue.h"
#include <algorithm>

void render::DrawablesQueue::CreateCommandBuffer( dx::commandbuffer & commandList_p, bool bClearStateCache_p )
{
	if( bClearStateCache_p){
		ZeroMemory(m_stateCache, sizeof(dx::Binder*)*dx::E_MAX_BINDS);
	}

	//Prepare();

	// sort drawables

	std::sort(m_sortqueue.begin(), m_sortqueue.end(), Entry());

	// holds current render binds (I feel this should be holded)

	// traverse "sorted drawables"
	sortarray::const_iterator itEnd = m_sortqueue.end();
	for( sortarray::const_iterator it = m_sortqueue.begin();
		 it != itEnd; ++it){

		const dx::statevec * pPipeStates = m_drawables[(*it).index].GetPipeStateGroup();

		// traverse pipe states

		UINT64 bindsSetMask = 0;

		for( dx::statevec::const_iterator itStates = pPipeStates->begin(),
			statesEnd =  pPipeStates->end();
			itStates != statesEnd;
			++itStates ){

			// traverse bind commands
			
		
			for( dx::bindervec::const_iterator itBinds = itStates->get()->Begin(),
				bindsEnd =  itStates->get()->End();
				itBinds != bindsEnd;
				++itBinds ){

					// check redundancy: needs to check precedences redundancy between pipe states of the same drawable

					if( !(bindsSetMask & (*itBinds)->TypeBits())
						&&
						m_stateCache[(*itBinds)->TypeIndex()] != (*itBinds).get() ){		// check if bind not already set

						bindsSetMask |= (*itBinds)->TypeBits();
						m_stateCache[(*itBinds)->TypeIndex()] = (*itBinds).get();
						commandList_p.push_back((*itBinds));
					}
					else{
						int x = 0;
					}
			}

		}

		commandList_p.push_back(m_drawables[(*it).index].GetDrawCall());
	}

	//Prepare();
}
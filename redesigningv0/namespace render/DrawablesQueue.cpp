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
	for( sortarray::const_iterator itDrawable = m_sortqueue.begin(),
		 itEnd = m_sortqueue.end();
		 itDrawable != itEnd; ++itDrawable){

		const dx::StateGroup * pPipeStates = m_drawables[(*itDrawable).index].GetPipeStateGroup();

		// traverse pipe states

		UINT64 bindsSetMask = 0;

		for( dx::StateGroup::const_iterator itStates = pPipeStates->begin(),
			statesEnd =  pPipeStates->end();
			itStates != statesEnd;
			++itStates ){

			// traverse bind commands
			
		
			for( dx::bindervec::const_iterator itBinds = itStates->get()->Begin(),
				bindsEnd =  itStates->get()->End();
				itBinds != bindsEnd;
				++itBinds ){

					// check redundancy: needs to check precedences redundancy between pipe states binds of the same drawable

					if( !(bindsSetMask & (*itBinds)->TypeBits())	// check if bind was not set by previous states*
						&&
						m_stateCache[(*itBinds)->TypeIndex()] != (*itBinds).get() ){		// check if bind not already set

						bindsSetMask |= (*itBinds)->TypeBits();
						m_stateCache[(*itBinds)->TypeIndex()] = (*itBinds).get();
						commandList_p.push_back((*itBinds));
					}
					/*else{
						int x = 0;
					}*/

					// * NOTE that later binds are discarded, so to add default binds that may be discarded if drawable
					// is setting a specific bind, it must came later on the states, not before
			}

		}

		// add draw call

		commandList_p.push_back(m_drawables[(*itDrawable).index].GetDrawCall());
	}

	//Prepare();
}
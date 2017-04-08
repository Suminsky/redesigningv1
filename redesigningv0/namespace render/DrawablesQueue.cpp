#include "PCHF.h"

#include "DrawablesQueue.h"
#include <algorithm>
#include <stdint.h>


#include "../namespace win/HighPerformanceTimer.h"
#include "../namespace win/FileLogger.h"

void render::DrawablesQueue::Submit( Drawable & drawable_p )
{
	m_drawables.push_back( drawable_p );

	//Entry newEntry = {drawable_p.GetSortKey(), ((uint32_t)m_drawables.size())-1};
	m_sortqueue.push_back((uint32_t)(m_drawables.size()) - 1);//newEntry);//
}

void render::DrawablesQueue::Submit( Drawable && drawable_p )
{
	m_drawables.emplace_back( std::move(drawable_p) );

	//Entry newEntry = {drawable_p.GetSortKey(), ((uint32_t)m_drawables.size())-1};
	m_sortqueue.push_back((uint32_t)(m_drawables.size()) - 1);//newEntry);//
}

void render::DrawablesQueue::CreateCommandBuffer( RenderCommands & commandList_p, bool bClearStateCache_p )
{
	win::UniqueHigPerfTimer().m_cronometer.StartCount();

	if( bClearStateCache_p){
		ZeroMemory(m_stateCache, sizeof(dx::Binder*)*dx::E_MAX_BINDS);
	}

	//Prepare();

	// sort drawables

	PrepareForSort();
	Entry::InsertionSort(m_sortqueue, m_drawables);

	// traverse "sorted drawables"

	for(	uint32_t itSorted = 0, sortedSize = (uint32_t)m_sortqueue.size();
			itSorted < sortedSize;
			++ itSorted ){

		const vStatePtrs & stateGroup = m_drawables[m_sortqueue[itSorted]].m_vStatePtrs;

		// traverse pipe states

		UINT64 bindsSetMask = 0;

		for(	uint32_t itStates = 0, statesSize = (uint32_t)stateGroup.size();
				itStates < statesSize;
				++ itStates ){

			// traverse bind commands

			const dx::PipeState::Binds & binds = stateGroup[itStates]->m_vBinderPtrs;
			
			for(	uint32_t itBinds = 0, bindsSize = (uint32_t)binds.Size();
					itBinds < bindsSize;
					++itBinds ){


					// check redundancy: needs to check precedences redundancy between pipe states binds of the same drawable

					if( (bindsSetMask & binds[itBinds]->TypeBits_int()) != binds[itBinds]->TypeBits_int() ){	// check if bind was not set by previous states* of the drawable
						
						bindsSetMask |= binds[itBinds]->TypeBits_int();

						if(	m_stateCache[binds[itBinds]->TypeIndex()] != binds[itBinds] ){		// check if bind not already set

							m_stateCache[binds[itBinds]->TypeIndex()] =  binds[itBinds];

							commandList_p.push_back( binds[itBinds] );
						}
					}
					//else{

					//	int x = 7; x = x;
					//}
			}

		}

		// add draw call

		commandList_p.push_back( m_drawables[m_sortqueue[itSorted]].m_pDrawCall );
	}

	//Prepare();


	DOUBLE sec = 0.0;
	win::UniqueHigPerfTimer().m_cronometer.EndCountSeconds(sec);
	win::UniqueFileLogger()<<sec<<SZ_NEWLINE;

}

// try to achieve temporal coerence

inline void render::DrawablesQueue::PrepareForSort() {

	uint32_t nDrawables = (uint32_t)m_drawables.size();
	uint32_t nPrevSorted = (uint32_t)m_sortqueue.size();

	if (nPrevSorted > nDrawables) {

		m_sortqueue.resize(nDrawables);
		for (uint32_t it = 0; it < nDrawables; ++it)
			m_sortqueue[it] = it;
	}

	else {// <=

		for (uint32_t it = nPrevSorted; it < nDrawables; ++it)
			m_sortqueue.push_back(it);
	}
}

void render::DrawablesQueue::Prepare()
{
	m_drawables.clear();
	//m_sortqueue.clear();
}

void render::DrawablesQueue::ResetState( dx::E_BIND eBind_p )
{
	m_stateCache[eBind_p] = NULL;
}
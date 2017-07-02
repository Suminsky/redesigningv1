#include "PCHF.h"

#include "DrawablesQueue.h"
#include <algorithm>
#include <stdint.h>


#include "../namespace win/HighPerformanceTimer.h"
#include "../namespace win/FileLogger.h"

void render::DrawablesQueue::Submit( Drawable & drawable_p )
{
	Drawable* pNew = m_drawables.StackAlloc();
	pNew->Initialize(m_drawablesPipeMemBuff.StackAlloc(drawable_p.m_vStatePtrs.Size()), drawable_p.m_vStatePtrs.Size());
	*pNew = drawable_p;

	//m_drawables.PushBack( drawable_p );
}

void render::DrawablesQueue::Submit( Drawable && drawable_p )
{
	Drawable* pNew = m_drawables.StackAlloc();
	pNew->Initialize(m_drawablesPipeMemBuff.StackAlloc(drawable_p.m_vStatePtrs.Size()), drawable_p.m_vStatePtrs.Size());
	*pNew = drawable_p;

	//m_drawables.PushBack( std::move(drawable_p) );
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
	InsertionSort(m_sortqueue, m_drawables);

	// traverse "sorted drawables"

	for(	uint32_t itSorted = 0, sortedSize = (uint32_t)m_sortqueue.Size();
			itSorted < sortedSize;
			++ itSorted ){

		const render::Drawable::States & stateGroup = m_drawables[m_sortqueue[itSorted]].m_vStatePtrs;

		// traverse pipe states

		UINT64 bindsSetMask = 0;

		for(	uint32_t itStates = 0, statesSize = (uint32_t)stateGroup.Size();
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

inline void render::DrawablesQueue::PrepareForSort() {

	uint32_t nDrawables = (uint32_t)m_drawables.Size();
	uint32_t nPrevSorted = (uint32_t)m_sortqueue.Size();

	if (nPrevSorted > nDrawables) {

		m_sortqueue.Reset();
		m_sortqueue.StackAlloc(nDrawables);
		for (uint32_t it = 0; it < nDrawables; ++it)
			m_sortqueue[it] = it;
	}
	else {// <=

		for (uint32_t it = nPrevSorted; it < nDrawables; ++it)
			m_sortqueue.PushBack(it);
	}
}

inline void render::DrawablesQueue::InsertionSort(sortstack & a, const drawablestack & drawables)
{
	int n = (int)a.Size();

	for (int i = 1; i < n; ++i)
	{
		uint32_t checkingValue = a[i];
		int prev = i - 1;
		while (prev >= 0 && drawables[a[prev]].GetSortKey() > drawables[checkingValue].GetSortKey())
		{
			a[prev + 1] = a[prev];
			--prev;
		}

		a[prev + 1] = checkingValue;
	}
}

void render::DrawablesQueue::Prepare()
{
	m_drawables.Reset();
	m_drawablesPipeMemBuff.Reset();

	//m_sortqueue.clear();
}

void render::DrawablesQueue::ResetState( dx::E_BIND eBind_p )
{
	m_stateCache[eBind_p] = NULL;
}
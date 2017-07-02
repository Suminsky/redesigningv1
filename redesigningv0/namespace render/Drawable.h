#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/15
	created:	15:2:2013   19:26
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	A object that can be draw to a render target, in a single draw call, that is.

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// system/standard headers

// private headers
#include "dx/pipeline/PipeState.h"
#include "dx/pipeline/DrawCall.h"

namespace render{

	// forward decls

	class DrawablesQueue;

	//========================================================================
	// 
	//========================================================================
	class Drawable{

		friend DrawablesQueue;

	public:
		
		typedef gen::Stack<const dx::PipeState*> States;
		// but...if drawables are volatile, they cant have any stack as member either
		// (not only be stacked thenselves)

		DBG(static uint32_t s_count;)

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Drawable(){
		
			DBG(++s_count;)
		}

		Drawable(
			uint64_t sortKey_p, dx::DrawCall * pDrawCall_p,
			const dx::PipeState** pMem_p, uint32_t maxSize_p)
			:
			m_sortKey(sortKey_p),
			m_pDrawCall(pDrawCall_p),
			m_vStatePtrs(pMem_p, maxSize_p)
		{
			DBG(++s_count;)
		}

		virtual ~Drawable(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Initialize(const dx::PipeState** pMem_p, uint32_t maxSize_p) {

			m_vStatePtrs.Reinitialize(pMem_p, maxSize_p);
		}

		//------------------------------------------------------------------------
		// add a new state to the drawable
		//------------------------------------------------------------------------
		void AddPipelineState(const dx::PipeState * pState_p ){

			m_vStatePtrs.PushBack( pState_p );
		}

		//------------------------------------------------------------------------
		// aux created for text render, that way you can have a tmp drawable
		// that can have some states used, and the last ones replaced every every time.
		//------------------------------------------------------------------------
		void PopLastPipelineState( uint32_t nStates = 1 ){

			m_vStatePtrs.Unstack(nStates);
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		UINT64 GetSortKey() const{ return m_sortKey; }

		//------------------------------------------------------------------------
		// setters
		//------------------------------------------------------------------------
		void SetDrawCall( dx::DrawCall * pDrawCall_p ){

			m_pDrawCall = pDrawCall_p;
		}
		void SetSortKey(uint64_t llSortKey_p ){

			m_sortKey = llSortKey_p;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Clear(){

			m_vStatePtrs.Reset();
		}

	private: // TODO

		States			m_vStatePtrs;			// states w binder commands
		dx::DrawCall *	m_pDrawCall;			// draw

		uint64_t m_sortKey; // viewport, distance, material..defined on a higer lvl
	};
}
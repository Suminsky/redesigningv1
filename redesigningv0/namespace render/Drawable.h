#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/15
	created:	15:2:2013   19:26
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\DrawablesQueue.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render
	file base:	DrawablesQueue
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	A object that can be draw to a render target, in a single draw call, that is.

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// system/standard headers

// private headers
#include "dx/pipeline/State.h"
#include "dx/pipeline/DrawCall.h"

namespace render{

	class DrawablesQueue;

	typedef std::vector<dx::State*> StatesPtrsVec;

	class Drawable{

		friend DrawablesQueue;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Drawable(){};
		virtual ~Drawable(){};

		//------------------------------------------------------------------------
		// add a new state to the drawable
		//------------------------------------------------------------------------
		void AddPipelineState( dx::State * pState_p ){

			m_pipeStateGroup.push_back( pState_p );
		}

		//------------------------------------------------------------------------
		// aux created for text render, that way you can have a tmp drawable
		// that can have some states used, and the last ones replaced every every time.
		// 
		//------------------------------------------------------------------------
		void PopLastPipelineState(){

			m_pipeStateGroup.pop_back();
		}

		//------------------------------------------------------------------------
		// add a group of states to the drawable
		//------------------------------------------------------------------------
		void AddPipelineStateGroup( const StatesPtrsVec & pipeStateGroup_p ){

			m_pipeStateGroup.insert( m_pipeStateGroup.cend(), pipeStateGroup_p.cbegin(), pipeStateGroup_p.cend() );
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		UINT64 GetSortKey() const{ return m_sortKey; }

		//------------------------------------------------------------------------
		// setters
		//------------------------------------------------------------------------
		void SetPipelineStateGroup( const StatesPtrsVec & pipeStateGroup_p ){

			m_pipeStateGroup = pipeStateGroup_p; // copy all states from the given state group
		}
		void SetDrawCall( dx::DrawCall * pDrawCall_p ){

			m_pDrawCall = pDrawCall_p;
		}
		void SetSortKey( UINT64 llSortKey_p ){

			m_sortKey = llSortKey_p;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Clear(){

			m_pipeStateGroup.clear();
		}

	private: // TODO

		StatesPtrsVec m_pipeStateGroup;			// states w binder commands
		dx::DrawCall * m_pDrawCall;				// draw

		UINT64 m_sortKey; // viewport, distance, material..defined on a higer lvl
	};
}
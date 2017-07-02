//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/15
	created:	15:2:2013   19:26
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\DrawablesQueue.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render
	file base:	DrawablesQueue
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	Sorts every renderable stuff so to minimize render state changes

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// system/standard headers


#include <vector>

// private headers

#include "Drawable.h"


namespace render{


	typedef std::vector<dx::Command*> RenderCommands;

	//------------------------------------------------------------------------
	// Receives the Drawables objects, sorts them and creates a command list.
	//------------------------------------------------------------------------
	class DrawablesQueue{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		DrawablesQueue() : m_stateCache(){};
		virtual ~DrawablesQueue(){};

		//------------------------------------------------------------------------
		// Used by drawables to put themselves on the queue
		//------------------------------------------------------------------------
		void Submit( Drawable & drawable_p );
		void Submit( Drawable && drawable_p );

		//------------------------------------------------------------------------
		// clear the queue
		//------------------------------------------------------------------------
		void Prepare();

		//------------------------------------------------------------------------
		// Sets the given state to NULL, effectively force a bind command to be
		// executed even it where already cached.
		// Used for re setting things like render targets after resizing bbuffers.
		//------------------------------------------------------------------------
		void ResetState( dx::E_BIND eBind_p );

		//------------------------------------------------------------------------
		// sort the drawables and put its commands in the list, removing
		// redundant binds
		//------------------------------------------------------------------------
		void CreateCommandBuffer( RenderCommands & commandList_p, bool bClearStateCache_p );

	private:

		typedef gen::Stack<Drawable, 2048>	drawablestack;
		typedef gen::Stack<uint32_t, 2048>	sortstack;
		
		// try to achieve temporal coerence
		void PrepareForSort();
		static void InsertionSort(sortstack & a, const drawablestack & drawables);
		
		dx::Binder	*	m_stateCache[dx::E_MAX_BINDS];
		drawablestack	m_drawables;
		sortstack		m_sortqueue;

		gen::Stack<const dx::PipeState *, 4 * drawablestack::MaxSize()> m_drawablesPipeMemBuff;
	};
}
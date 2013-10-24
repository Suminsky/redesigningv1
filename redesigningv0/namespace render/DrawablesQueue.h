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

#pragma warning( push )
#pragma warning( disable : 4351 ) // array on initialization list now default initialized, VS specific

	typedef std::vector<dx::Command*> RenderCommands;

	//------------------------------------------------------------------------
	// Receives the Drawables objects, sorts them and creates a command list.
	//------------------------------------------------------------------------
	class DrawablesQueue{

		struct Entry{
			UINT64 drawableKey; // sort key
			int index;			// index on the drawables array

			bool operator ()( const Entry & a, const Entry & b)const{ // used by stl sort algos

				return a.drawableKey < b.drawableKey;
			}
		};

		typedef std::vector<Drawable> drawablearray;
		typedef std::vector<Entry> sortarray;

		drawablearray m_drawables;
		sortarray  m_sortqueue;
		dx::Binder* m_stateCache[dx::E_MAX_BINDS];
		
	public:

		DrawablesQueue() : m_stateCache(){};
		virtual ~DrawablesQueue(){};

		//------------------------------------------------------------------------
		// Used by drawables to put themselves on the queue
		//------------------------------------------------------------------------
		void Submit( Drawable & drawable_p ){

			m_drawables.push_back( drawable_p );

			Entry newEntry = {drawable_p.GetSortKey(), ((int)m_drawables.size())-1};
			m_sortqueue.push_back(newEntry);

			//assert( drawable_p.m_sortKey != 13258597302978759884 );
		}
		void Submit( Drawable && drawable_p ){

			m_drawables.emplace_back( std::move(drawable_p) );

			Entry newEntry = {drawable_p.GetSortKey(), ((int)m_drawables.size())-1};
			m_sortqueue.push_back(newEntry);

			//assert( drawable_p.m_sortKey != 13258597302978759884 );
		}

		void Prepare(){

			//m_drawables.clear();
			m_drawables.resize(0);
			m_sortqueue.clear();
		}

		//------------------------------------------------------------------------
		// Sets the given state to NULL, effectively force a bind command to be
		// executed even it where already cached.
		// Used for re setting things like render targets after resizing bbuffers.
		//------------------------------------------------------------------------
		void ResetState( dx::E_BIND eBind_p ){

			m_stateCache[eBind_p] = NULL;
		}

		//------------------------------------------------------------------------
		// sort the drawables and put its commands in the list, removing
		// redundant binds
		//------------------------------------------------------------------------
		void CreateCommandBuffer( RenderCommands & commandList_p, bool bClearStateCache_p );
	};

#pragma warning( pop ) 
}
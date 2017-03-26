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

		typedef std::vector<Drawable>	drawablearray;
		

		struct Entry{

			UINT64 drawableKey; // sort key
			int index;			// index on the drawables array

			bool operator ()( const Entry & a, const Entry & b)const{ // used by stl sort algos

				return a.drawableKey < b.drawableKey;
			}

			bool Greater(Entry & other) {

				return drawableKey > other.drawableKey;
			}

			static void InsertionSort(std::vector<Entry> & a)
			{
				int n = (int)a.size();

				for (int i = 1; i < n; ++i)
				{
					Entry checkingValue = a[i];
					int prev = i - 1;
					while (prev >= 0 && a[prev].Greater(checkingValue))
					{
						a[prev + 1] = a[prev];
						--prev;
					}

					a[prev + 1] = checkingValue;
				}
			}

			// use to insert a new element in an already sorted array, withouth breaking the order
			static void OrderedInsert(std::vector<Entry> & a, int n, Entry & newValue)
			{
				// assumes array is sorted
				assert(n <= a.size());

				a[n] = newValue;

				int prev = n - 1;
				while (prev >= 0 && a[prev].Greater(newValue))
				{
					a[prev + 1] = a[prev];
					--prev;
				}

				a[prev + 1] = newValue;
			}
		};

		typedef std::vector<Entry>		sortarray;
		

		drawablearray	m_drawables;
		sortarray		m_sortqueue;
		dx::Binder	*	m_stateCache[dx::E_MAX_BINDS];
	};

#pragma warning( pop ) 
}
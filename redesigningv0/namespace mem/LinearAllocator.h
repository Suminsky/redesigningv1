#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/01/30
	created:	30:1:2014   17:10
	file:		LinearAllocator.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	- all mem must be freed at once (it only grows)
				- fixed size
				- no waste or fragmentation

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <assert.h>

// private includes
#include "../namespace gen/gen_macros.h"
#include "../namespace gen/gen_data.h"

namespace mem{

	class LinearAllocator{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		LinearAllocator( void * pStart_p, void * pEnd_p )
			:
		m_pStart(pStart_p),
		m_pEnd(pEnd_p),
		m_pCurrent(pStart_p){

			assert( pStart_p );
			assert( pStart_p < pEnd_p );
		}

		//------------------------------------------------------------------------
		// offset: ptr will be aligned after offset
		//------------------------------------------------------------------------
		void * Allocate( size_t size_p, size_t alignment_p, size_t offset_p ){

			assert( size_p > 0);
			assert( gen::IsPowerOfTwo( (int)alignment_p ) );

			size_t alignReadySize = size_p + alignment_p;

		}

		//------------------------------------------------------------------------
		// cant do that
		//------------------------------------------------------------------------
		void Free( void * ptr_p ){

			assert(0);
		}

		void Reset(){

			// fill with pattern
			DBG(
				
				)

			m_pCurrent = m_pStart;
		}

	private:

		void * m_pStart;
		void * m_pEnd;
		void * m_pCurrent;
	};
}
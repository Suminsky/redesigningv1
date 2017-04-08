#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/14
	created:	14:7:2013   23:58
	file:		PipeState.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	A collection of bind commands.
				Its good to have collections to share common sets, like a shader that
				have default values, materials, etc..

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "Binder.h"


namespace render{

	class DrawablesQueue;
}

namespace dx{
	
	class PipeState{

		friend render::DrawablesQueue;
	
	public:

		typedef gen::Stack<Binder*, (uint32_t)-1> Binds;
		DBG(static uint32_t s_count;)

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		PipeState(Binder** ppMem_p, uint32_t maxSize_p)
			:
			m_stateMask(0LL),
			m_vBinderPtrs(ppMem_p, maxSize_p)
		{
			DBG(++s_count;)
		}

		PipeState()
			:
			m_stateMask(0LL)
		{
			DBG(++s_count;)
		}

		~PipeState(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Initialize(Binder** ppMem_p, uint32_t maxSize_p) {

			m_vBinderPtrs.Initialize(ppMem_p, maxSize_p);
		}

		//------------------------------------------------------------------------
		// Adds a command updating the state mask
		//------------------------------------------------------------------------
		void AddBinderCommand( Binder * pBinder_p ){

			assert( !(m_stateMask & pBinder_p->TypeBits_int() ) );
			assert( pBinder_p->TypeIndex() >= 0 &&  pBinder_p->TypeIndex() < E_MAX_BINDS );

			m_vBinderPtrs.PushBack(pBinder_p);
			m_stateMask |= pBinder_p->TypeBits_int();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Reset(){

			m_vBinderPtrs.Reset();
			m_stateMask = 0LL;
		}

		//------------------------------------------------------------------------
		// Iterators
		//------------------------------------------------------------------------
		Binder* & operator[](uint32_t index_p){

			return m_vBinderPtrs[index_p];
		}
		const Binder *const & operator[](uint32_t index_p)const{

			return m_vBinderPtrs[index_p];
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		UINT64 GetStateMask()const{	return m_stateMask;	}

		
	private:

		UINT64					m_stateMask; // mark all binds it binds
		Binds	m_vBinderPtrs;
	};
}
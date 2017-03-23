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

	typedef std::vector<Binder*> vBinderPtrs;
	
	class PipeState{

		friend render::DrawablesQueue;
	
	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		PipeState()
			:
		m_stateMask(0LL)
		{}

		~PipeState(){}

		//------------------------------------------------------------------------
		// Adds a command updating the state mask
		//------------------------------------------------------------------------
		void AddBinderCommand( Binder * pBinder_p ){

			assert( !(m_stateMask & pBinder_p->TypeBits_int() ) );
			assert( pBinder_p->TypeIndex() >= 0 &&  pBinder_p->TypeIndex() < E_MAX_BINDS );

			m_vBinderPtrs.push_back(pBinder_p);
			m_stateMask |= pBinder_p->TypeBits_int();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Reset(){

			m_vBinderPtrs.clear();
			m_stateMask = 0LL;
		}

		//------------------------------------------------------------------------
		// Iterators
		//------------------------------------------------------------------------
		vBinderPtrs::iterator Begin(){	return m_vBinderPtrs.begin();	}
		vBinderPtrs::const_iterator End(){	return m_vBinderPtrs.end();	}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		UINT64 GetStateMask()const{	return m_stateMask;	}

	private:

		std::vector<Binder*>	m_vBinderPtrs;
		UINT64					m_stateMask; // mark all binds it binds
	};


	typedef std::shared_ptr<PipeState> shared_State_ptr;
	typedef std::weak_ptr<PipeState> weak_State_ptr;
}
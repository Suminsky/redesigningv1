#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/14
	created:	14:7:2013   23:58
	file:		State.h
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

	typedef std::vector<Binder*> BindersPtrsVec;
	
	class State{

		friend render::DrawablesQueue;
	
	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		State()
			:
		m_stateMask(0LL)
		{}

		~State(){}

		//------------------------------------------------------------------------
		// Adds a command updating the state mask
		//------------------------------------------------------------------------
		void AddBinderCommand( Binder * pBinder_p ){

			assert( !(m_stateMask & pBinder_p->TypeBits() ) );
			assert( pBinder_p->TypeIndex() >= 0 &&  pBinder_p->TypeIndex() < E_MAX_BINDS );

			m_binds.push_back(pBinder_p);
			m_stateMask |= pBinder_p->TypeBits();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Reset(){

			m_binds.clear();
			m_stateMask = 0LL;
		}

		//------------------------------------------------------------------------
		// Iterators
		//------------------------------------------------------------------------
		BindersPtrsVec::const_iterator Begin(){	return m_binds.begin();	}
		BindersPtrsVec::const_iterator End(){	return m_binds.end();	}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		UINT64 GetStateMask()const{	return m_stateMask;	}

	private:

		BindersPtrsVec m_binds;
		UINT64 m_stateMask; // mark all binds it binds
	};


	typedef std::shared_ptr<State> shared_State_ptr;
	typedef std::weak_ptr<State> weak_State_ptr;
}
#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/29
	created:	29:10:2013   10:54
	file:		System.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	� Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "Object.h"

namespace game{

	class Layer;

	typedef unsigned int SYSTEMINDEX;
	static const unsigned int INVALID_SYSTEMINDEX = (unsigned int)-1;

	class System{

		friend class Layer;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		System()
			:
			m_pLayerOwner(nullptr){}

		virtual ~System(){}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		Layer * GetLayerOwner() const { return m_pLayerOwner; }

	protected:

		Layer * m_pLayerOwner;

	private:


	};

	typedef std::shared_ptr<System> shared_System_ptr;
	typedef std::weak_ptr<System> weak_System_ptr;
}
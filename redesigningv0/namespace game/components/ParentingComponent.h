#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/29
	created:	29:10:2013   22:50
	file:		ParentingComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <vector>
#include <assert.h>

// private includes
#include "../Component.h"

namespace game{

	typedef unsigned int INDEX_ONPARENT;
	static const unsigned int INVALID_INDEX_ONPARENT = (unsigned int)-1;

	class ParentingComponent: public Component{

		friend class ParentingComponent;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ParentingComponent()
			:
			m_indexOnParent(INVALID_INDEX_ONPARENT),
			m_pParent(nullptr){}

		//------------------------------------------------------------------------
		// makes given object a child of this object
		//------------------------------------------------------------------------
		void AttachObject( ParentingComponent * pObject_p );

		//------------------------------------------------------------------------
		// removes given object from the childs
		//------------------------------------------------------------------------
		void DettachObject( ParentingComponent * pObject_p );

		//------------------------------------------------------------------------
		// makes this object a child on the given parent
		//------------------------------------------------------------------------
		void SetParent( ParentingComponent * pNewParent_p );

	private:

		ParentingComponent * m_pParent;
		INDEX_ONPARENT m_indexOnParent;
		std::vector<ParentingComponent*> m_vChilds;
	};

	typedef std::shared_ptr<ParentingComponent> shared_ParentingComponent_ptr;
	typedef std::weak_ptr<ParentingComponent> weak_ParentingComponent_ptr;
}
#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/14
	created:	14:4:2013   19:49
	file:		IObjectComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	the life items that compose a game object

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#include <memory>
#include "ComponentTypes.h"
#include "../namespace gen/Pool.h"

namespace game{

	// froward dcls

	class Object;
	class System;
	class AComponentFactory;

	// new types

	typedef unsigned int COMPONENTINDEX;
	static const unsigned int INVALID_COMPONENTINDEX = (unsigned int)-1;

	typedef unsigned int COMPONENTTYPE;
	static const unsigned int INVALID_COMPONENTTYPE = (unsigned int)-1;

	typedef void* ComponentEventData; // event data used on the object event machine

	//========================================================================
	// 
	//========================================================================
	class Component{

		friend class Object;
		friend class System;
		friend class AComponentFactory;

		DCL_POOLELEMENT();

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Component()
			:
		m_iCurrentRosterIndex((unsigned int)-1),
		m_type(INVALID_COMPONENTTYPE),
		m_currentComponentObjectIndex(INVALID_COMPONENTINDEX),
		m_pObjectOwner(nullptr),
		m_bDetached(true){}

		virtual ~Component(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool IsAttached() const { return !m_bDetached; }

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		Object *  GetObjectOwner(){ return m_pObjectOwner; }
		COMPONENTTYPE GetType() const { return m_type; }

	protected:

		Object * m_pObjectOwner;
		COMPONENTTYPE m_type;

	private:

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual void VOnAttach();
		virtual void VOnDetach();

		COMPONENTINDEX m_currentComponentObjectIndex;

		bool m_bDetached;
	};

	typedef std::shared_ptr<Component> shared_Component_ptr;
	typedef std::weak_ptr<Component> weak_Component_ptr;
}
#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/14
	created:	14:4:2013   19:49
	file:		Component.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	compose a game object

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#include <memory>
#include "ComponentTypes.h"
#include "../namespace gen/Pool.h"
#include "../namespace gen/gen_data.h"

namespace game{

	// froward dcls

	class Object;
	class System;
	class IComponentFactory;

	// new types

	typedef unsigned int COMPONENTINDEX;
	static const unsigned int INVALID_COMPONENTINDEX = (unsigned int)-1;

	typedef unsigned int COMPONENTTYPE;
	static const unsigned int INVALID_COMPONENTTYPE = (unsigned int)-1;

	typedef void* ComponentEventData; // event data used on the object event machine

	//========================================================================
	// 
	//========================================================================
	class Component: public gen::NonCopyable{

		friend class Object;
		friend class System;
		friend class IComponentFactory;

		DCL_POOLELEMENT();

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Component()
			:
		//m_iCurrentRosterIndex((unsigned int)-1),
		m_type(INVALID_COMPONENTTYPE),
		m_currentIndexOnObject(INVALID_COMPONENTINDEX),
		m_pObjectOwner(nullptr),
		m_bDettached(true){

			static int s_count = 0;
			m_ID = s_count++;
		}

		virtual ~Component(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool IsAttached() const { return !m_bDettached; }

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		Object *  GetObjectOwner(){ return m_pObjectOwner; }
		COMPONENTTYPE GetType() const { return m_type; }
		COMPONENTINDEX GetIndexOnObject() const { return m_currentIndexOnObject; }

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Dettach();

	protected:

		Object * m_pObjectOwner;
		COMPONENTTYPE m_type;

	private:

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual void VOnAttach();
		virtual void VOnDetach();

		COMPONENTINDEX m_currentIndexOnObject;

		bool m_bDettached;

		int m_ID;
	};

	typedef gen::pool_ptr<Component> pool_Component_ptr;
}
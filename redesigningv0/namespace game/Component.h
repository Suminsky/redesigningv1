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

namespace game{

	class Object;

	typedef unsigned int COMPONENTINDEX;
	static const unsigned int INVALID_COMPONENTINDEX = (unsigned int)-1;

	//typedef unsigned int ComponentID;

	//========================================================================
	// 
	//========================================================================
	class Component{

		friend class Object;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Component()
			:
			m_currentComponentIndex(INVALID_COMPONENTINDEX),
			m_pObjectOwner(nullptr),
			m_bDead(true){}

		virtual ~Component(){}
		

		//------------------------------------------------------------------------
		// get owner object
		//------------------------------------------------------------------------
		Object *  GetObjectOwner(){ return m_pObjectOwner; }

	protected:

		Object * m_pObjectOwner;

	private:

		COMPONENTINDEX m_currentComponentIndex;
		bool m_bDead;
	};

	typedef std::shared_ptr<Component> shared_Component_ptr;
	typedef std::weak_ptr<Component> weak_Component_ptr;
}
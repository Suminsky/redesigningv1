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
	typedef unsigned int COMPONENT_OBJECTINDEX;
	static const unsigned int INVALID_OBJECTINDEX = (unsigned int)-1;
	typedef unsigned int ComponentID;

	//========================================================================
	// 
	//========================================================================
	class Component{

		friend class Object;

		COMPONENT_OBJECTINDEX m_currentObjectIndex;

	protected:

		ComponentID m_ID;
		Object * m_pOwner;

	public:

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		Component():m_currentObjectIndex(INVALID_OBJECTINDEX){}
		virtual ~Component(){};

		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VOnUpdate( double /*dTime_p*/, double /*dDeltaTime_p*/ ){};
		//virtual void VOnEvent( Event event_p ) = 0;

		//------------------------------------------------------------------------
		// get id
		//------------------------------------------------------------------------
		ComponentID ID(){

			return m_ID;
		}
	};

	typedef std::shared_ptr<Component> shared_Component_ptr;
}
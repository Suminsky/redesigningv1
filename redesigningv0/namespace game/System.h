#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/29
	created:	29:10:2013   10:54
	file:		System.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "Object.h"

namespace game{

	// forward dcls

	class Layer;
	class SystemMachine;

	// new types

	typedef unsigned int SYSTEMINDEX;
	static const unsigned int INVALID_SYSTEMINDEX = (unsigned int)-1;

	typedef void* SystemEventData;

	class System{

		friend class Layer;
		friend class SystemMachine;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		System()
			:
			//m_pLayerOwner(nullptr),
			m_pSysMachineOwner(nullptr),
			m_bDead(true),
			m_currentSystemIndex(INVALID_SYSTEMINDEX){}

		virtual ~System(){}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		//Layer * GetLayerOwner() const { return m_pLayerOwner; }
		SystemMachine * GetSysMachineOwner() const { return m_pSysMachineOwner; }

	protected:

		//Layer * m_pLayerOwner;
		SystemMachine * m_pSysMachineOwner;

	private:

		bool m_bDead;
		SYSTEMINDEX m_currentSystemIndex;

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual void VOnInit(){}
		virtual void VOnUpdate( double /*dAccum_p*/, double /*dDelta_p*/ ){}
		virtual void VOnDestroy(){}
		virtual void VOnDraw( double /*dInterpolation_p*/ ){}

		//virtual void VOnObjectAddedToLayer( game::Object * /*pObject_p*/ ){}
		//virtual void VOnObjectRemovedFromLayer(  game::Object * /*pObject_p*/ ){}
		//virtual void VOnComponentAddedToObjectOnLayer( game::Component * /*pObject_p*/ ){}
		//virtual void VOnComponentRemovedFromObjectOnLayer(  game::Component * /*pObject_p*/ ){}
	};

	typedef std::shared_ptr<System> shared_System_ptr;
	typedef std::weak_ptr<System> weak_System_ptr;
}
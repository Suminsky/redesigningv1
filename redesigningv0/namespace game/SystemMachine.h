#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/10
	created:	10:11:2013   22:55
	file:		SystemMachine.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "System.h"
#include "EventMachine.h"

namespace game{

	class Layer;

	typedef std::vector<shared_System_ptr> Systems;
	typedef std::vector<SYSTEMINDEX> SystemIndexes;

	class SystemMachine{

		friend class Layer;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SystemMachine(){}
		explicit SystemMachine( Layer * pLayer_p )
			:
		m_pLayerRef(pLayer_p)
		{}

		//------------------------------------------------------------------------
		// systems
		//------------------------------------------------------------------------
		void AddSystem( shared_System_ptr && pSystem_p );
		void AddSystem( const shared_System_ptr & pSystem_p );
		void RemoveSystem( SYSTEMINDEX systemCurrentIndex_p );
		void RemoveSystem( const shared_System_ptr & pSystem_p );
		void RemoveSystem( const System * pSystem_p );

		//------------------------------------------------------------------------
		// events
		//------------------------------------------------------------------------
		void RegisterForSystemEvent( EventMachine<SystemEventData>::EventHandlerDelegate eveHandlerDelegate_p, EventType eveType_p );
		void UnregisterForSystemEvent( EventMachine<SystemEventData>::EventHandlerDelegate eveHandlerDelegate_p, EventType eveType_p );
		void AddSystemEvent( EventType eveType_p, SystemEventData eveData_p );
		void DispatchSystemEventImmediately( EventType eveType_p, SystemEventData eveData_p );

		Layer * GetLayer() const { return m_pLayerRef; }

	private:

		//------------------------------------------------------------------------
		// call handlers interested on system events
		//------------------------------------------------------------------------
		void DispatchSystemEvents();

		//------------------------------------------------------------------------
		// cleaning
		//------------------------------------------------------------------------
		void CleanRemovedSystems();

		void SetLayer(Layer * pLayer_p){ m_pLayerRef = pLayer_p;}

		Systems m_systems;
		SystemIndexes m_removedSystems;

		EventMachine<SystemEventData> m_SystemEventMachine;

		Layer * m_pLayerRef;
	};

	typedef std::shared_ptr<SystemMachine> shared_SystemMachine_ptr;
	typedef std::weak_ptr<SystemMachine> weak_SystemMachine_ptr;
}
#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/07
	created:	7:6:2013   1:54
	file:		EventMachine.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <vector>
#include <unordered_map>


// private includes
#include "EventHandler.h"


namespace game{

	typedef std::vector<AEventHandler*> EventHandlers;
	typedef std::unordered_map<EventType, EventHandlers> EventHandlerRegister;

	typedef std::vector<Event> EventQueue;

	class EventMachine{

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		virtual ~EventMachine(){}

		//------------------------------------------------------------------------
		// for each event, check all handlers registered for it, calling their
		// handling method OnEvent
		//------------------------------------------------------------------------
		void DispatchEvents();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void AddEvent( EventType type_p, INT_PTR data_p ){

			// TODO: use real emplace_back on vs2012
			m_events.emplace_back( Event(type_p, data_p) );
		}

		//------------------------------------------------------------------------
		// instead of queuing the event, it call all handlers for processing
		// before the call returns.
		// returns false if no one processed the event.
		//------------------------------------------------------------------------
		bool DispatchEventImmetiately( EventType type_p, INT_PTR data_p );

		//------------------------------------------------------------------------
		// do NOT register inside OnEvent
		//------------------------------------------------------------------------
		void RegisterForEvent( AEventHandler * pHandler_p, EventType eventType_p );

		//------------------------------------------------------------------------
		// do NOT unregister inside OnEvent
		//------------------------------------------------------------------------
		void UnregisterForEvent( AEventHandler * pHandler_p, EventType eventType_p );

	private:

		EventQueue m_events;
		EventHandlerRegister m_register;
	};

	typedef std::shared_ptr<EventMachine> shared_EventMachine_ptr;
	typedef std::weak_ptr<EventMachine> weak_EventMachine_ptr;
}
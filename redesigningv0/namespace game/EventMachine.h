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
#include "../namespace gen/gen_macros.h"


namespace game{

	template<typename EVENT_DATA>
	class EventMachine{

	public:

		typedef Event<EVENT_DATA> Event;
		typedef AEventHandler<EVENT_DATA> AEventHandler;

		typedef std::vector<AEventHandler*> EventHandlers;
		typedef std::unordered_map<EventType, EventHandlers> EventHandlerRegister;

		typedef std::vector<Event> EventQueue;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		virtual ~EventMachine(){}

		//------------------------------------------------------------------------
		// for each event, check all handlers registered for it, calling their
		// handling method OnEvent
		//------------------------------------------------------------------------
		void DispatchEvents()
		{
			// move events, cause the m_events may still change inside VOnEvent, so
			// its better to decouple them from new ones

			// as it is for now...adding or removing listeners would also fuck everything up..
			// so its for now, forbidden... e.e boss solution (TODO)

			EventQueue events = std::move( m_events );
			m_events.clear();	// second the standard, you may use moved containers for no 'precondition needed' operations, like clear().
								// so it became reusable after clear...but that means the bitch will have to reallocate memory again, same
								// penalty as copying instead of moving..TODO (what about creating static instead of tmp, and swapping them?)

			// traverse all current events

			for(	EventQueue::iterator itEvent = events.begin(), itEventEnd = events.end();
					itEvent !=  itEventEnd;
					++ itEvent ){

					// get handlers registered for current event type

					EventHandlerRegister::iterator itHandlersOfCurrentEvent = m_register.find( (*itEvent).GetType() );

					if(	 itHandlersOfCurrentEvent == m_register.end() ) continue;

					EventHandlers & handlers = itHandlersOfCurrentEvent->second; // cache for readability..

					// dispatch event to handlers

					for(	EventHandlers::iterator itHandler = handlers.begin(), itHandlersEnd = handlers.end();
							itHandler != itHandlersEnd;
							++ itHandler ){

							(*itHandler)->VOnEvent( *itEvent ); // TODO: consider "consuming" event if VOnEvent returns false

					}// traverse handlers
			}// traverse events
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void AddEvent( EventType type_p, EVENT_DATA data_p = EVENT_DATA() ){

			// TODO: use real emplace_back on vs2012
			m_events.emplace_back( Event(type_p, data_p) );
		}

		//------------------------------------------------------------------------
		// instead of queuing the event, it call all handlers for processing
		// before the call returns.
		// returns false if no one processed the event.
		//------------------------------------------------------------------------
		bool DispatchEventImmetiately( EventType type_p, EVENT_DATA data_p = 0 )
		{
			// build event

			Event immEvent(type_p, data_p);

			// get handlers registered for current even type

			EventHandlerRegister::iterator itHandlersOfCurrentEvent =
				m_register.find( immEvent.GetType() );

			if(	 itHandlersOfCurrentEvent == m_register.end() ) return false;

			EventHandlers & handlers = itHandlersOfCurrentEvent->second; // cache

			// dispatch event to handlers

			for(	EventHandlers::iterator itHandler = handlers.begin(), itHandlersEnd = handlers.end();
					itHandler != itHandlersEnd;
					++ itHandler ){

					(*itHandler)->VOnEvent( immEvent );
			}

			return true;
		}

		//------------------------------------------------------------------------
		// do NOT register inside OnEvent
		//------------------------------------------------------------------------
		void RegisterForEvent( AEventHandler * pHandler_p, EventType eventType_p )
		{
			// get handlers for given type, if theres none, operator [] creates one,
			// then add new handler to it

			NDBG(

				m_register[eventType_p].push_back( pHandler_p );

			)

			DBG(

				// check for double registering

				EventHandlers & handlers = m_register[eventType_p];

			if( handlers.empty() ){

				handlers.push_back( pHandler_p );
			}
			else{

				EventHandlers::const_iterator itHandlersEnd = handlers.cend();
				EventHandlers::const_iterator itFound = std::find( handlers.cbegin(), itHandlersEnd, pHandler_p );
				assert( itFound == itHandlersEnd );

				handlers.push_back( pHandler_p );
			}

			)
		}

		//------------------------------------------------------------------------
		// do NOT unregister inside OnEvent
		//------------------------------------------------------------------------
		void UnregisterForEvent( AEventHandler * pHandler_p, EventType eventType_p )
		{
			// find/check handlers of event

			EventHandlers & handlers = m_register[eventType_p];
			assert( !handlers.empty());

			EventHandlers::iterator itHandlersEnd = handlers.end();

			EventHandlers::iterator itHandler = std::find( handlers.begin(), itHandlersEnd, pHandler_p );
			assert( itHandler != itHandlersEnd );

			// swap it with last element

			//std::swap( itHandler, --itHandlersEnd ); // stupid bug
			--itHandlersEnd;
			*itHandler = *itHandlersEnd;

			// remove it

			handlers.pop_back();
		}

	private:

		EventQueue m_events;
		EventHandlerRegister m_register;
	};

	//typedef std::shared_ptr<EventMachine> shared_EventMachine_ptr;
	//typedef std::weak_ptr<EventMachine> weak_EventMachine_ptr;
}
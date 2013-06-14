#include "EventMachine.h"

#include <algorithm>
#include <assert.h>

#include "../namespace gen/gen_macros.h"

using namespace game;

void EventMachine::DispatchEvents()
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

bool EventMachine::DispatchEventImmetiately( EventType type_p, INT_PTR data_p, INT_PTR data2_p )
{
	// build event

	Event immEvent(type_p, data_p, data2_p);

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

void EventMachine::RegisterForEvent( AEventHandler * pHandler_p, EventType eventType_p )
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

void EventMachine::UnregisterForEvent( AEventHandler * pHandler_p, EventType eventType_p )
{
	// find/check handlers of event

	EventHandlers & handlers = m_register[eventType_p];
	assert( !handlers.empty());

	EventHandlers::const_iterator itHandlersEnd = handlers.cend();

	EventHandlers::const_iterator itHandler = std::find( handlers.cbegin(), itHandlersEnd, pHandler_p );
	assert( itHandler != itHandlersEnd );

	// swap it with last element

	std::swap( itHandler, --itHandlersEnd );

	// remove it

	handlers.pop_back();
}

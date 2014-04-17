#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/07
	created:	7:6:2013   1:54
	file:		EventMachine.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	TODO, test if holding a single vector of handles is faster
				(instead of handlers per event type)
				(so when dispatching, all handlers would be traversed and checked).

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <vector>
#include <unordered_map>
#include <algorithm>


// private includes
#include "Event.h"
#include "../namespace gen/gen_macros.h"
#include "../namespace gen/Delegate.h"


namespace game{

	template<typename EVENT_DATA>
	class EventMachine{

	public:

		typedef Event<EVENT_DATA> Event;
		typedef gen::Delegate1Param<const Event &> EventHandler;

		typedef std::vector<EventHandler> EventHandlers;
		struct RegisteredHandlers{

			EventType eveType;
			EventHandlers vHandlers;
		};
		
		typedef std::vector<RegisteredHandlers> EventHandlerRegister;
		struct RegisterIt{
			int itR, itH;
		};

		typedef std::vector<Event> EventQueue;

		EventMachine()
			:
		m_iCurrentBuffer(0){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		virtual ~EventMachine(){}

		//------------------------------------------------------------------------
		// for each event, check all handlers registered for itE, calling their
		// handling method OnEvent
		//------------------------------------------------------------------------
		void DispatchEvents()
		{
			// for each event

			int iConsumingBuffer = m_iCurrentBuffer;
			m_iCurrentBuffer = m_iCurrentBuffer == 0 ? 1 : 0;

			for( int itE = 0, nEves = (int) m_events[iConsumingBuffer].size();
				 itE < nEves;
				 ++itE ){

				Event & eve = m_events[iConsumingBuffer][itE];

				// find the registered handlers for the eve type

				for( int itR = 0, nRegistered = (int)m_register.size();
					 itR < nRegistered;
					 ++itR ){

					RegisteredHandlers & reg = m_register[itR];

					if( reg.eveType == eve.GetType() ){

						// dispatch event for all handlers

						for( int itH = 0, nHandlers = (int) reg.vHandlers.size();
							 itH < nHandlers;
							 ++ itH ){

							EventHandler & handler = reg.vHandlers[itH];

							handler( eve );

						}// handlers
					}// found type
				}//regs
			}//eves

			m_events[iConsumingBuffer].clear();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void AddEvent( EventType type_p, EVENT_DATA data_p = EVENT_DATA() ){

			// TODO: use real emplace_back on vs2012
			m_events[m_iCurrentBuffer].emplace_back( Event(type_p, data_p) );
		}

		//------------------------------------------------------------------------
		// instead of queuing the event, it call all handlers for processing
		// before the call returns.
		// returns false if no one processed the event.
		//------------------------------------------------------------------------
		void DispatchEventImmetiately( EventType type_p, EVENT_DATA data_p = 0 )
		{
			// build event

			Event eve(type_p, data_p);

			// find the registered handlers for the eve type

			for( int itR = 0, nRegistered = (int)m_register.size();
				itR < nRegistered;
				++itR ){

					RegisteredHandlers & reg = m_register[itR];

					if( reg.eveType == eve.GetType() ){

						// dispatch event for all handlers

						for( int itH = 0, nHandlers = (int) reg.vHandlers.size();
							itH < nHandlers;
							++ itH ){

								EventHandler & handler = reg.vHandlers[itH];

								handler( eve );

						}// handlers
					}// found type
			}//regs
		}

		//------------------------------------------------------------------------
		//
		//------------------------------------------------------------------------
		void RegisterForEvent( const EventHandler & handler_p, EventType eventType_p )
		{
			
			// traverse register, if theres a evetype registered, add handle to the register
			// if not, create one and register
			// in DBG, check for double registering handler for the same type
			
			for( int it = 0, size = (int)m_register.size();
				 it < size;
				 ++it ){

				if( m_register[it].eveType == eventType_p ){

					NDBG(
					m_register[it].vHandlers.push_back(handler_p);
					);
					DBG(
					assert( FindHandlerInHandlers( m_register[it].vHandlers, handler_p) == -1 );
					m_register[it].vHandlers.push_back(handler_p);
					);

					return;
				}
			}

			RegisteredHandlers newRegisteredHandlers;
			newRegisteredHandlers.eveType = eventType_p;
			newRegisteredHandlers.vHandlers.push_back( handler_p );

			m_register.emplace_back( std::move(newRegisteredHandlers) );
		}

		//------------------------------------------------------------------------
		// do NOT unregister inside OnEvent
		//------------------------------------------------------------------------
		void UnregisterForEvent( const EventHandler & handler_p, EventType eventType_p )
		{
			// find/check handlers of event

			for( int it = 0, size = (int)m_register.size();
				 it < size;
				 ++it ){

					if( m_register[it].eveType == eventType_p ){

						int itUnreg = FindHandlerInHandlers( m_register[it].vHandlers, handler_p );
						assert( it != -1 );

						//RegisterIt toRemove = { it, itUnreg };
						//m_removedHandlers.push_back( toRemove );
						int iHandlersLast = (int)m_register[it].vHandlers.size() -1;
						m_register[it].vHandlers[itUnreg] = m_register[it].vHandlers[iHandlersLast];
						m_register[it].vHandlers.pop_back();

						return;
					}
			}

			assert(0);
		}

	private:

		//------------------------------------------------------------------------
		// aux
		//------------------------------------------------------------------------
		int inline FindHandlerInHandlers( EventHandlers & handlers_p, const EventHandler & handler_p ){

			for( int it = 0, size = (int) handlers_p.size();
				 it < size;
				 ++ it ){

					if( handlers_p[it] == handler_p )
						return it;
			}

			return -1;
		}

		unsigned int m_iCurrentBuffer;
		EventQueue m_events[2]; // double buffering
		EventHandlerRegister m_register;
		//std::vector<RegisterIt> m_removedHandlers;

	};

	//typedef std::shared_ptr<EventMachine> shared_EventMachine_ptr;
	//typedef std::weak_ptr<EventMachine> weak_EventMachine_ptr;
}
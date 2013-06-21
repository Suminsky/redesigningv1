#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/07
	created:	7:6:2013   1:30
	file:		EventHandler.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	The handler is supposed to register for events and override a VOnEvent method

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "Event.h"

namespace game{

	template<typename EVENT_DATA>
	class EventMachine;

	template<typename EVENT_DATA>
	class AEventHandler{

		friend class EventMachine<EVENT_DATA>;

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		virtual ~AEventHandler(){}

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		virtual void VOnEvent( const Event<EVENT_DATA> & event_p ) = 0;
	
	};

	//typedef std::shared_ptr<AEventHandler> shared_AEventHandler_ptr;
	//typedef std::weak_ptr<AEventHandler> weak_AEventHandler_ptr;
}
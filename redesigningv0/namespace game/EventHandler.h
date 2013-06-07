#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/07
	created:	7:6:2013   1:30
	file:		EventHandler.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	The handler is supposed to register for events and override a OnEvent method

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "Event.h"

namespace game{

	class EventMachine;

	class AEventHandler{

		friend class EventMachine;

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		virtual ~AEventHandler(){}

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		virtual void OnEvent( const Event & event_p ) = 0;
	
	};

	typedef std::shared_ptr<AEventHandler> shared_AEventHandler_ptr;
	typedef std::weak_ptr<AEventHandler> weak_AEventHandler_ptr;
}
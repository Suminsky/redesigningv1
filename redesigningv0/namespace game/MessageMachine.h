#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/29
	created:	29:10:2013   17:37
	file:		MessageMachine.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "Message.h"
#include "../namespace gen/Delegate.h"

namespace game{

	class MessageMachine{

		typedef std::vector<gen::Delegate1Param<Message*>> handlers;
		struct Registered{

			MessageType msgType;
			handlers vHandlers;
		};
		typedef std::vector<Registered> Register;

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void DispatchMessage( Message * pMessage_p ){

			// find if theres anyone interested in the message type


		}

	private:

		Register m_register;
	};
}
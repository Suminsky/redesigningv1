#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/29
	created:	29:10:2013   17:01
	file:		Message.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	A message is almost like an event, but sent directly to objects.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes

namespace game{

	typedef unsigned int MessageType;
	static const unsigned int INVALID_MESSAGETYPE = (unsigned int)-1;

	class Message{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		explicit Message( MessageType type_p = INVALID_MESSAGETYPE )
			:
			m_type(type_p)
			{}

		virtual ~Message(){}

		//------------------------------------------------------------------------
		// setters
		//------------------------------------------------------------------------
		void SetType( MessageType type_p ){

			m_type = type_p;
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		MessageType GetType() const { return m_type; }

	private:

		MessageType m_type;
	};

	typedef std::shared_ptr<Message> shared_Message_ptr;
	typedef std::weak_ptr<Message> weak_Message_ptr;
}
#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/07
	created:	7:6:2013   1:33
	file:		Event.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	im thinking on letting it act as a c# structure: always passed by copy

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <BaseTsd.h>
#include <memory>

// private includes

namespace game{

	typedef int EventType;
	static const unsigned int INVALID_EVENTTYPE = (unsigned int)-1;

	struct Event{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Event( EventType type_p = INVALID_EVENTTYPE, INT_PTR data_p = 0 ): m_type(type_p), m_data(data_p){}
		virtual ~Event(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Set( EventType type_p, INT_PTR data_p ){

			m_type = type_p;
			m_data = data_p;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		EventType GetType() const { return m_type; }

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename T >
		T GetData() const { return (T)m_data; }
		INT_PTR GetData() const { return m_data; }

	private:

		EventType m_type;
		INT_PTR m_data;
	
	};

	typedef std::shared_ptr<Event> shared_Event_ptr;
	typedef std::weak_ptr<Event> weak_Event_ptr;
}
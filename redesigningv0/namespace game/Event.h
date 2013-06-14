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
		Event( EventType type_p = INVALID_EVENTTYPE, INT_PTR data_p = 0, INT_PTR data2_p = 0 )
			:
		m_type(type_p), m_data(data_p), m_data2(data2_p){}
		
		virtual ~Event(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Set( EventType type_p, INT_PTR data_p, INT_PTR data2_p = 0 ){

			m_type = type_p;
			m_data = data_p;
			m_data2 = data2_p;
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

		// perhaps its good idea..
		template< typename T >
		T GetData2() const { return (T)m_data2; }
		INT_PTR GetData2() const { return m_data2; }
		// fuckoff, it will do for now // TODO

	private:

		EventType m_type;
		INT_PTR m_data;
		INT_PTR m_data2;
	};

	typedef std::shared_ptr<Event> shared_Event_ptr;
	typedef std::weak_ptr<Event> weak_Event_ptr;
}
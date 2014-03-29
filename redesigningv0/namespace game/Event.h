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

// private includes

namespace game{

	typedef unsigned int EventType;
	static const unsigned int INVALID_EVENTTYPE = (unsigned int)-1;

	template<typename DATA>
	struct Event{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Event( EventType type_p = INVALID_EVENTTYPE, DATA data_p = DATA() )
			:
			m_type(type_p),
			m_data(data_p)
			{}
		
		virtual ~Event(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Set( EventType type_p, DATA data_p = DATA() ){

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
		DATA GetData() const { return m_data; }
		DATA& GetDataRef() { return m_data; }

		template<typename asType> asType GetDataAs()const{ return (asType)m_data; }

	private:

		EventType m_type;
		DATA m_data;
	};

	//typedef std::shared_ptr<Event> shared_Event_ptr;
	//typedef std::weak_ptr<Event> weak_Event_ptr;
}


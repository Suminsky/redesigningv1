#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/07
	created:	7:6:2013   22:05
	file:		AlarmTask.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	a task that triggers an event after some time

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../Task.h"
#include "../EventMachine.h"

namespace game{

	class AlarmTask: public Task{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		AlarmTask()
			:
			m_elapsed(0.0f), m_duration(0.0),
			m_pEventMachineRef(nullptr)
		{}

		void Set( const Event & event_p, double duration_p ){

			m_elapsed = 0.0;
			m_duration = duration_p;
			m_event = event_p;
		}

		void Set( EventMachine * pEventMachine_p ){

			m_pEventMachineRef = pEventMachine_p;
		}

	private:

		void VOnUpdate( double, double delta_p ){

			m_elapsed += delta_p;

			if( m_elapsed >= m_duration ){

				m_pEventMachineRef->AddEvent( m_event.GetType(), m_event.GetData() );

				Chain();
			}
		}

		double m_elapsed;
		double m_duration;

		EventMachine * m_pEventMachineRef;
		Event m_event;

	};

	typedef std::shared_ptr<AlarmTask> shared_AlarmTask_ptr;
	typedef std::weak_ptr<AlarmTask> weak_AlarmTask_ptr;
}
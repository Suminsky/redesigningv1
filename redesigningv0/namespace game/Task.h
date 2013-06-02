#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/23
	created:	23:5:2013   10:33
	file:		Task.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	Task base. Inherit to create a task. Tasks are executed by the task machine.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes

namespace game{

	class TaskMachine;
	class Task;
	typedef std::shared_ptr<Task> shared_Task_ptr;

	typedef unsigned int TASKINDEX;
	static const unsigned int INVALID_TASKINDEX = (unsigned int)-1;	

	class Task{

		friend TaskMachine;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		explicit Task( shared_Task_ptr pChainedTask_p = nullptr ): m_pChainedTask(pChainedTask_p), m_currentTaskIndex(INVALID_TASKINDEX){}
		virtual ~Task(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool Running(){

			return m_currentTaskIndex != INVALID_TASKINDEX;
		}

	protected:

		//------------------------------------------------------------------------
		// completion methods
		// Chain will launch the chained task in this tasks place, failed wont.
		//------------------------------------------------------------------------
		void Chain();
		void Destroy();
		
		shared_Task_ptr m_pChainedTask; // the task to be launched after this one is over

	private: // how could I not think on that before (private virtuals)..well I didn't know u could overload those

		TASKINDEX m_currentTaskIndex;
		TaskMachine * m_pTaskMachineRef;

		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VOnInit(){}
		virtual void VOnUpdate( double, double )=0;
		virtual void VOnDestroy(){}
	};
}
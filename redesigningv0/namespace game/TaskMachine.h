#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/23
	created:	23:5:2013   13:56
	file:		TaskMachine.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	Controls Tasks execution.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <vector>
#include <assert.h>

// private includes
#include "Task.h"
#include "../namespace gen/gen_macros.h"

namespace game{

	typedef std::vector<shared_Task_ptr> Tasks;
	typedef std::vector<TASKINDEX> TaskIndexes;

	class TaskMachine{

		friend Task;

	public:

		//------------------------------------------------------------------------
		// traverse task and update them
		//------------------------------------------------------------------------
		void Update( double accumTime_p, double delta_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void AddTask( shared_Task_ptr && pNewTask_p );
		void AddTask( const shared_Task_ptr & pNewTask_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void AbortTask( TASKINDEX taskCurrentIndex_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool Empty(){return m_tasks.empty();}

	private:

		Tasks m_tasks;
		TaskIndexes m_destroyedTasks;
		//std::vector<Task*> m_removedTasks; TODO

		//------------------------------------------------------------------------
		// call this to kill a task and put his chain on its place
		//------------------------------------------------------------------------
		void ChainTask( TASKINDEX taskCompletedIndex_p );

		//------------------------------------------------------------------------
		// Im currently swapping everything to the end, and updating the index inside
		// m_destryedTasks for each swap (if swapped with another destroyed)
		// pretty fucking messy..the other option is resize and swapping the destroyed to the
		// end without caring..OR, stop doing mess and store raw pointers
		//------------------------------------------------------------------------
		void CleanAbortedTasks();
	};
}
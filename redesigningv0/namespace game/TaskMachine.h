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
		void Update( double accumTime_p, double delta_p ){

			for( Tasks::iterator it = m_tasks.begin(), itEnd = m_tasks.end();
				 it != itEnd ;
				 ++ it ){

				(*it)->VOnUpdate( accumTime_p, delta_p );
			}

			//check if theres destroyed tasks, clean then off

			if( !m_destroyedTasks.empty() )
				CleanAbortedTasks();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void AddTask( const shared_Task_ptr & pNewTask_p ){

			pNewTask_p->VOnInit();
			pNewTask_p->m_currentTaskIndex = TASKINDEX(m_tasks.size());
			pNewTask_p->m_pTaskMachineRef = this;

			m_tasks.push_back( pNewTask_p );
		}
		void AddTask( shared_Task_ptr && pNewTask_p ){
			
			pNewTask_p->VOnInit();
			pNewTask_p->m_currentTaskIndex = TASKINDEX(m_tasks.size());
			pNewTask_p->m_pTaskMachineRef = this;

			m_tasks.push_back( std::move(pNewTask_p) );
		}

		//------------------------------------------------------------------------
		// The only way a task can be finished not spontaneously
		//------------------------------------------------------------------------
		void AbortTask( TASKINDEX taskCurrentIndex_p ){
	
			m_tasks[taskCurrentIndex_p]->VOnDestroy();
			m_destroyedTasks.push_back(taskCurrentIndex_p);
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool Empty(){

			return m_tasks.empty();
		}

	private:

		Tasks m_tasks;
		TaskIndexes m_destroyedTasks;

		//------------------------------------------------------------------------
		// call this to kill a task and put his chain on its place
		//------------------------------------------------------------------------
		void ChainTask( TASKINDEX taskCompletedIndex_p ){
			
			m_tasks[taskCompletedIndex_p]->VOnDestroy();

			m_tasks[taskCompletedIndex_p] = std::move( m_tasks[taskCompletedIndex_p]->m_pChainedTask );
			m_tasks[taskCompletedIndex_p]->m_currentTaskIndex = taskCompletedIndex_p;
			m_tasks[taskCompletedIndex_p]->m_pTaskMachineRef = this;
			m_tasks[taskCompletedIndex_p]->VOnInit();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void CleanAbortedTasks(){				

			// swap all destroyed tasks to the end of the vector than resizes

			unsigned int nDestroyed = (unsigned int)m_destroyedTasks.size(); // cache
			unsigned int nTasks = (unsigned int)m_tasks.size();

			if( nDestroyed == nTasks ){

				m_tasks.clear();
				m_destroyedTasks.clear();
				return;
			}

			for( unsigned int it = 0, itLast = nTasks - 1; it < nDestroyed; ++it ){

				// check if "to be removed" already at end

				if( m_destroyedTasks[it] == itLast - it){

					continue;
				}

				std::swap( m_tasks[m_destroyedTasks[it]], m_tasks[itLast - it] ); // size - 1, size -2, size -3

				m_tasks[m_destroyedTasks[it]]->m_currentTaskIndex = m_destroyedTasks[it]; // update index
			}

			// "trim"
			m_tasks.resize(nTasks - nDestroyed);

			m_destroyedTasks.clear();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		//void RemoveTask( TASKINDEX taskCurrentIndex_p ){

		//	std::swap( m_tasks[taskCurrentIndex_p], m_tasks[m_tasks.size()-1] );
		//	m_tasks[taskCurrentIndex_p]->m_currentTaskIndex = taskCurrentIndex_p; // update index
		//	m_tasks.pop_back();
		//}
	};
}
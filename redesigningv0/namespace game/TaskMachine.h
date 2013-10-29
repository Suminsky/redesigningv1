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
		void AddTask( shared_Task_ptr && pNewTask_p ){

			assert( pNewTask_p->m_bDead );

			pNewTask_p->m_bDead = false;

			if( pNewTask_p-> m_currentTaskIndex == INVALID_TASKINDEX ){

				pNewTask_p->m_currentTaskIndex = TASKINDEX(m_tasks.size());
				pNewTask_p->m_pTaskMachineRef = this;

				pNewTask_p->VOnInit();

				m_tasks.push_back( std::move(pNewTask_p) );
			}
			else{
				
				pNewTask_p->VOnInit();
			}
		}
		void AddTask( const shared_Task_ptr & pNewTask_p ){

			assert( pNewTask_p->m_bDead );

			pNewTask_p->m_bDead = false;

			if( pNewTask_p-> m_currentTaskIndex == INVALID_TASKINDEX ){

				pNewTask_p->m_currentTaskIndex = TASKINDEX(m_tasks.size());
				pNewTask_p->m_pTaskMachineRef = this;

				m_tasks.push_back( pNewTask_p );
			}

			
			pNewTask_p->VOnInit();
		}

		//------------------------------------------------------------------------
		// The only way a task can be finished not spontaneously
		// TODO !!!BUG!!!: a task can be aborted and added on the same frame, causing
		// the task machine to have 2 of the same task..
		// dunno why, but state and layer seem to solve this differently..why I did that here?
		// PROBLEM-> so I tried the state/layer method here, and theres a problem..
		// by not setting INVALID_TASKINDEX here, the test for Running() may return
		// true even if the task will be deleted in the same frame. Means itDestroyed will not
		// add the task because itDestroyed seems the task is already running.
		// I need to change this completly:
		// NEED:
		// - be able to remove and add on the same frame
		// - know if the task is removed or not, any time
		// - remove the task at the end
		//------------------------------------------------------------------------
		void AbortTask( TASKINDEX taskCurrentIndex_p ){

			assert( taskCurrentIndex_p != INVALID_TASKINDEX );
			assert( !m_tasks[taskCurrentIndex_p]->m_bDead ); //not sure TODO
	
			//m_tasks[taskCurrentIndex_p]->VOnDestroy();
			m_tasks[taskCurrentIndex_p]->m_bDead = true;
			//m_tasks[taskCurrentIndex_p]->m_currentTaskIndex = INVALID_TASKINDEX;

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
			m_tasks[taskCompletedIndex_p]->m_currentTaskIndex = INVALID_TASKINDEX;
			m_tasks[taskCompletedIndex_p]->m_bDead = true;

			m_tasks[taskCompletedIndex_p] = std::move( m_tasks[taskCompletedIndex_p]->m_pChainedTask );
			m_tasks[taskCompletedIndex_p]->m_currentTaskIndex = taskCompletedIndex_p;
			m_tasks[taskCompletedIndex_p]->m_bDead = false;
			m_tasks[taskCompletedIndex_p]->m_pTaskMachineRef = this;
			m_tasks[taskCompletedIndex_p]->VOnInit();
		}

		//------------------------------------------------------------------------
		// NOTE: the fuck am I doing anyway...theres a for and 3 ifs here..if I test
		// if task is alive before update and dead after update, I can just do the
		// swap inside the update loop..
		// so thats 2 ifs and a possible swap and resize.
		// against
		// 1 if (to check if theres destroied) and a possible for and 3 ifs and 2 resizes
		// jeez crist man..not to mention that add and remove code becomes clean
		// the thing is, removing at the middle of the frame doesnt make many sense.
		// a frame happens in an instant of time, things shouldnt happen while the update
		// is going on
		// im currently swaping everything to the end, and updating the index inside
		// m_destryedTasks for each swap (if swaped with another destroyed)
		// pretty fucking messy..the other option is resize and swapping the destryed to the
		// end without caring..not sure
		//------------------------------------------------------------------------
		void CleanAbortedTasks(){

			unsigned int nDestroyed =	(unsigned int)m_destroyedTasks.size(); // cache
			unsigned int nTasks =		(unsigned int)m_tasks.size();

			for(	unsigned int itDestroyed = 0, itLast = nTasks - 1;
					itDestroyed < nDestroyed;
					++itDestroyed, --itLast ){

				if( !m_tasks[m_destroyedTasks[itDestroyed]]->m_bDead ){

					--nDestroyed;
					continue; // untested
				}

				
				m_tasks[m_destroyedTasks[itDestroyed]]->VOnDestroy();
				m_tasks[m_destroyedTasks[itDestroyed]]->m_currentTaskIndex = INVALID_TASKINDEX;
				m_tasks[m_destroyedTasks[itDestroyed]]->m_pTaskMachineRef = nullptr;

				if( m_destroyedTasks[itDestroyed] == itLast ){

					continue;
				}

				std::swap( m_tasks[m_destroyedTasks[itDestroyed]], m_tasks[itLast] );

				if( m_tasks[m_destroyedTasks[itDestroyed]]->m_bDead ){

					// find the swaped task on the list to be destroyed, update the index

					for( unsigned int itToBeDestroyed = itDestroyed; itToBeDestroyed < nDestroyed; ++itToBeDestroyed ){

						if( m_destroyedTasks[itToBeDestroyed] == m_tasks[m_destroyedTasks[itDestroyed]]->m_currentTaskIndex ){

							m_destroyedTasks[itToBeDestroyed] = m_destroyedTasks[itDestroyed];
						}
					}
				}
				else{

					m_tasks[m_destroyedTasks[itDestroyed]]->m_currentTaskIndex = m_destroyedTasks[itDestroyed];
				}
			}

			// "trim"

			m_tasks.resize(nTasks - nDestroyed);

			m_destroyedTasks.clear();
		}
	};
}
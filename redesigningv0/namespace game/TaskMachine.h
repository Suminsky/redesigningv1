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
		// 
		//------------------------------------------------------------------------
		void AbortTask( TASKINDEX taskCurrentIndex_p ){

			assert( taskCurrentIndex_p != INVALID_TASKINDEX );
			assert( !m_tasks[taskCurrentIndex_p]->m_bDead ); //not sure TODO
	
			m_tasks[taskCurrentIndex_p]->m_bDead = true;

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
		// Im currently swapping everything to the end, and updating the index inside
		// m_destryedTasks for each swap (if swapped with another destroyed)
		// pretty fucking messy..the other option is resize and swapping the destroyed to the
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

					// find the swapped task on the list to be destroyed, update the index

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
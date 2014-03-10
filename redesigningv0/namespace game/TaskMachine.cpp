#include "TaskMachine.h"

using namespace game;

void game::TaskMachine::Update( double accumTime_p, double delta_p )
{
	for( int it = 0, itEnd = (int)m_tasks.size();
		it != itEnd ;
		++ it ){

			m_tasks[it]->VOnUpdate( accumTime_p, delta_p );
	}

	//check if theres destroyed tasks, clean then off

	if( !m_destroyedTasks.empty() )
		CleanAbortedTasks();
}

void game::TaskMachine::AddTask( shared_Task_ptr && pNewTask_p )
{
	assert( pNewTask_p->m_bDead );

	pNewTask_p->m_bDead = false;

	if( pNewTask_p-> m_currentTaskIndex == INVALID_TASKINDEX ){

		pNewTask_p->m_currentTaskIndex = TASKINDEX(m_tasks.size());
		pNewTask_p->m_pTaskMachineRef = this;

		pNewTask_p->VOnInit();

		m_tasks.push_back( std::move(pNewTask_p) );
	}
	/*else{

	pNewTask_p->VOnInit();
	}*/
}

void game::TaskMachine::AddTask( const shared_Task_ptr & pNewTask_p )
{
	assert( pNewTask_p->m_bDead );

	pNewTask_p->m_bDead = false;

	if( pNewTask_p-> m_currentTaskIndex == INVALID_TASKINDEX ){

		pNewTask_p->m_currentTaskIndex = TASKINDEX(m_tasks.size());
		pNewTask_p->m_pTaskMachineRef = this;

		pNewTask_p->VOnInit();

		m_tasks.push_back( pNewTask_p );
	}


	//pNewTask_p->VOnInit();
}

void game::TaskMachine::AbortTask( TASKINDEX taskCurrentIndex_p )
{
	assert( taskCurrentIndex_p != INVALID_TASKINDEX );
	assert( !m_tasks[taskCurrentIndex_p]->m_bDead ); //not sure TODO

	m_tasks[taskCurrentIndex_p]->m_bDead = true;

	m_destroyedTasks.push_back(taskCurrentIndex_p);
}

void game::TaskMachine::ChainTask( TASKINDEX taskCompletedIndex_p )
{
	m_tasks[taskCompletedIndex_p]->VOnDestroy();
	m_tasks[taskCompletedIndex_p]->m_currentTaskIndex = INVALID_TASKINDEX;
	m_tasks[taskCompletedIndex_p]->m_bDead = true;

	m_tasks[taskCompletedIndex_p] = std::move( m_tasks[taskCompletedIndex_p]->m_pChainedTask );
	m_tasks[taskCompletedIndex_p]->m_currentTaskIndex = taskCompletedIndex_p;
	m_tasks[taskCompletedIndex_p]->m_bDead = false;
	m_tasks[taskCompletedIndex_p]->m_pTaskMachineRef = this;
	m_tasks[taskCompletedIndex_p]->VOnInit();
}

void game::TaskMachine::CleanAbortedTasks()
{
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

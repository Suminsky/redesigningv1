#include "PCHF.h"
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

	//if( !m_destroyedTasks.empty() )
	if( !m_removedTasks.empty() )
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

	//m_destroyedTasks.push_back(taskCurrentIndex_p);
	m_removedTasks.push_back(m_tasks[taskCurrentIndex_p].get());
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
	unsigned int nRemoved =	(unsigned int)m_removedTasks.size(); // cache
	unsigned int nTasks =		(unsigned int)m_tasks.size();

	int nSkipped = 0;
	for( unsigned int itR = 0, itLast = (nTasks-1); itR < nRemoved; ++itR ){

		if( !m_removedTasks[itR]->m_bDead ){

			++nSkipped;
			continue;
		}

		if( m_removedTasks[itR]->m_currentTaskIndex == itLast ){

			m_removedTasks[itR]->VOnDestroy();
			m_removedTasks[itR]->m_currentTaskIndex = INVALID_TASKINDEX;
			--itLast;
			continue; // already "swapped"
		}

		std::swap( m_tasks[m_removedTasks[itR]->m_currentTaskIndex], m_tasks[itLast] );
		--itLast;

		// update the index of the swapped object (not the one sent to pop)

		m_tasks[m_removedTasks[itR]->m_currentTaskIndex]->m_currentTaskIndex = m_removedTasks[itR]->m_currentTaskIndex;

		// since the removed vector store pointers, theres no dirt data to update
		// but we need to invalidate the discarded, cause its used as check when adding and removing..that
		// can be discarded TODO

		m_removedTasks[itR]->VOnDestroy();
		m_removedTasks[itR]->m_currentTaskIndex = INVALID_TASKINDEX;	
	}

	// "trim"

	nRemoved -= nSkipped;
	m_tasks.resize(nTasks - nRemoved);

	m_removedTasks.clear();
}
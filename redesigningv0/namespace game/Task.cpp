#include "Task.h"
#include "TaskMachine.h"

using namespace game;

void Task::Chain()
{
	if( m_pChainedTask ){
		
		// launch chained task

		m_pTaskMachineRef->ChainTask( m_currentTaskIndex );
	}
	else{

		m_pTaskMachineRef->AbortTask( m_currentTaskIndex );
	}
	//m_currentTaskIndex = INVALID_TASKINDEX;
}

void Task::Abort()
{
	m_pTaskMachineRef->AbortTask( m_currentTaskIndex );
	//m_currentTaskIndex = INVALID_TASKINDEX;
}
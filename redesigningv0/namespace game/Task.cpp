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
}

void Task::Destroy()
{
	m_pTaskMachineRef->AbortTask( m_currentTaskIndex );
}
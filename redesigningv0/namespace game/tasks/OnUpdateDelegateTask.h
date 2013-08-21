#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/08/03
	created:	3:8:2013   17:19
	file:		OnUpdateDelegateTask.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	made to take out the burden of creating many little tasks just to handle small
				states. With this task, one can just pass a member function to be executed in
				a chained way...not sure if gonna work..
				if the delegate returns true, chain is called
				o.o damn, am I creating a coroutine?

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <assert.h>
#include <memory>

// private includes
#include "../Task.h"
#include "../../namespace gen/gen_data.h"

namespace game{

	typedef gen::DelegateR2Param< bool, double, double> TaskDelegate;

	class OnUpdateDelegateTask: public Task{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		explicit OnUpdateDelegateTask(	TaskDelegate delegate_p = TaskDelegate(),
										shared_Task_ptr pChainedTask_p = nullptr )
			:
		Task( pChainedTask_p ),
		m_onUpdateCallback(delegate_p)
		{}

		//------------------------------------------------------------------------
		// delayed ctor
		//------------------------------------------------------------------------
		void Set( TaskDelegate delegate_p, shared_Task_ptr pChainedTask_p = nullptr ){

			m_onUpdateCallback = delegate_p;
			m_pChainedTask = pChainedTask_p;
		}

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnUpdate( double dAccum_p, double dDelta_p ){

			assert( m_onUpdateCallback );

			if( m_onUpdateCallback( dAccum_p, dDelta_p ) )
				Chain();
		}

		TaskDelegate m_onUpdateCallback;
	};

	typedef std::shared_ptr<OnUpdateDelegateTask> shared_OnUpdateDelegateTask_ptr;
	typedef std::weak_ptr<OnUpdateDelegateTask> weak_OnUpdateDelegateTask_ptr;
}
#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/24
	created:	24:5:2013   10:12
	file:		LerpValueTask.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	this is actually a float interpolation task, needs a better name (TODO)

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes

#include "../../namespace gen/gen_data.h"
#include "../Task.h"

namespace game{

	class LerpValueTask: public Task{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		LerpValueTask(	float & fValueToLerp_p,
					float fTimeToComplete_p = 1.0f,
					float from = 1.0f, float to = 0.0f,
					shared_Task_ptr pChainedTask_p = nullptr );
		LerpValueTask(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Set(	float fTimeToComplete_p = 1.0f, float from = 1.0f, float to = 0.0f );
		void Set(	float * fValueToLerp_p,
					float fTimeToComplete_p = 1.0f, float from = 1.0f, float to = 0.0f );
		void Set(	const shared_Task_ptr & pChainedTask_p, float & fValueToLerp_p,
					float fTimeToComplete_p = 1.0f, float from = 1.0f, float to = 0.0f );

	private:

		double m_elapsed;
		float m_fTimeToComplete;
		float * m_pfLerpingRef;
		float m_from, m_to;

		//LerpValueTask & operator =( const LerpValueTask & other_p ); // warning supress

		//------------------------------------------------------------------------
		// update
		//------------------------------------------------------------------------
		virtual void VOnUpdate( double, double delta );
	};

	typedef std::shared_ptr<LerpValueTask> shared_LerpValueTask_ptr;

}
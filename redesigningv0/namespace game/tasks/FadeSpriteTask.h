#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/24
	created:	24:5:2013   10:12
	file:		FadeSpriteTask.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes

#include "../../namespace gen/gen_data.h"
#include "../Task.h"

namespace game{

	class SpriteComponent;
	typedef std::shared_ptr<SpriteComponent> shared_SpriteComponent_ptr;

	class FadeSpriteTask: public Task{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		FadeSpriteTask( const shared_SpriteComponent_ptr & pSpriteComp_p,
						float fTimeToComplete_p = 1.0f,
						float from = 1.0f, float to = 0.0f,
						shared_Task_ptr pChainedTask_p = nullptr );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Set( float fTimeToComplete_p = 1.0f, float from = 1.0f, float to = 0.0f );

	private:

		double m_elapsed;
		float m_fTimeToComplete;
		float & m_alphaReference;
		float m_from, m_to;

		FadeSpriteTask & operator =( const FadeSpriteTask & other_p ); // warning supress

		//------------------------------------------------------------------------
		// update
		//------------------------------------------------------------------------
		virtual void VOnUpdate( double, double delta );
	};

	typedef std::shared_ptr<FadeSpriteTask> shared_FadeSpriteTask_ptr;

}
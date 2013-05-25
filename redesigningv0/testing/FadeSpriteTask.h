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

#include "../namespace gen/gen_data.h"
#include "../namespace game/Task.h"

	class SpriteComponent;
	typedef std::shared_ptr<SpriteComponent> shared_SpriteComp_ptr;

	//========================================================================
	// TASK
	//========================================================================
	class FadeSpriteTask: public game::Task{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		FadeSpriteTask( float fTimeToComplete_p, const shared_SpriteComp_ptr & pSpriteComp_p, game::shared_Task_ptr pChainedTask_p = nullptr );

	private:

		float m_fTimeToComplete;
		float & m_alphaReference;

		FadeSpriteTask & operator =( const FadeSpriteTask & other_p );

		//------------------------------------------------------------------------
		// update
		//------------------------------------------------------------------------
		virtual void VOnUpdate( double, double delta )
		{
			static double elapsed = 0.0;
			elapsed += delta;

			float alpha = gen::Tweening<>::Linear( (float)elapsed, m_fTimeToComplete, 1.0f, 0.0f );

			if( alpha <= 0.0f ){

				m_alphaReference = 0.0;

				Chain();
			}
			else{

				m_alphaReference = alpha;
			}
		}
	};

	typedef std::shared_ptr<FadeSpriteTask> shared_FadeSpriteTask_ptr;
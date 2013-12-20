#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/25
	created:	25:11:2013   20:45
	file:		SpriteAnimationSystem.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../System.h"
#include "../../namespace gen/Pool.h"
#include "../components/SpriteAnimationComponent.h"

namespace game{

	class SpriteAnimationSystem: public System{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteAnimationSystem()
			:
		  m_poolAccess(nullptr, nullptr)
		  {}

	private:

		gen::Pool<SpriteAnimationComponent, 256>::PoolAccessor m_poolAccess;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnInit();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnUpdate( double /*dAccum_p*/, double /*dDelta_p*/ );

	};

	typedef std::shared_ptr<SpriteAnimationSystem> shared_SpriteAnimationSystem_ptr;
	typedef std::weak_ptr<SpriteAnimationSystem> weak_SpriteAnimationSystem_ptr;
}
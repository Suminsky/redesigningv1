#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/19
	created:	19:11:2013   21:15
	file:		SpriteSystem.h
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
#include "../components/SpriteComponent_.h"

namespace game{

	class SpriteSystem: public System{

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		SpriteSystem( sprite::SpriteRenderer * pSpriteRenderer_p, sprite::Camera * pCamera_p )
			:
		m_poolAccess(nullptr, nullptr),
		m_pSpriteRendererRef(pSpriteRenderer_p),
		m_pCameraRef(pCamera_p){}

	private:

		gen::Pool<SpriteComponent_>::PoolAccessor m_poolAccess;
		sprite::SpriteRenderer * m_pSpriteRendererRef;
		sprite::Camera * m_pCameraRef;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnInit();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnDraw( double /*dInterpolation_p*/ );

	};

	typedef std::shared_ptr<SpriteSystem> shared_SpriteSystem_ptr;
	typedef std::weak_ptr<SpriteSystem> weak_SpriteSystem_ptr;
}


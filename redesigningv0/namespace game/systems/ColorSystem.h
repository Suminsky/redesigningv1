#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/29
	created:	29:10:2013   14:58
	file:		ColorSystem.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../System.h"
#include "../components/ColorComponent.h"
#include "../../namespace gen/Pool.h"


namespace game{

	class ColorSystem: public System{

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		ColorSystem()
			:
		m_poolAccess(nullptr, nullptr)
		{}

	
	private:

		gen::Pool<ColorComponent>::PoolAccessor m_poolAccess;

		//------------------------------------------------------------------------
		// get pool
		//------------------------------------------------------------------------
		void VOnInit();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnUpdate( double /*dAccum_p*/, double /*dDelta_p*/ );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void RecursiveUpdate( ColorComponent & currentParent_p );
	};

	typedef std::shared_ptr<ColorSystem> shared_ColorSystem_ptr;
	typedef std::weak_ptr<ColorSystem> weak_ColorSystem_ptr;
}
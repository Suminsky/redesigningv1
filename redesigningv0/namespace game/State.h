#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/12
	created:	12:4:2013   20:21
	file:		AState.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	A game state.
				I.e. menu, level 1, level 2, map screen, game over, etc.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#include <memory>

namespace game{

	class AState{
	public:

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~AState(){}

		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VInit(){}
		virtual void VUpdate( const double /*dTime_p*/, const double /*dDeltaTime_p*/ ) = 0;
		virtual void VDraw( const double /*dInterpolation_p*/ ){}
		virtual void VDestroy(){}
	};

	typedef std::shared_ptr<AState> shared_AState_ptr;
}
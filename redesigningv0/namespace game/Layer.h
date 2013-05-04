#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/04
	created:	4:5:2013   17:36
	file:		Layer.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	a state is composed of layers (in game, hud, pause menu, denug console..) 

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "Timer.h"

namespace game{

	class ALayer{

		Timer<double> m_timer;
		bool m_bActive;


		//------------------------------------------------------------------------
		// updates state timer and call updates with timer time
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p ){

			m_timer.Update( dDeltaTime_p );

			VUpdate( m_timer.GetTime(), m_timer.GetDelta() );
		}


	public:

		
		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VInit(){}
		virtual void VUpdate( const double /*dTime_p*/, const double /*dDeltaTime_p*/ ) = 0;
		virtual void VDraw( const double /*dInterpolation_p*/ ){}
		virtual void VDestroy(){}
	};

	typedef std::shared_ptr<ALayer> shared_ALayer_ptr;
}
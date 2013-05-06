#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/04
	created:	4:5:2013   17:36
	file:		Layer.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	a state is composed of layers (in game, hud, pause menu, denug console..)
				the layer is the last "stage", where the game objects needs to lie to be updated

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "Timer.h"
#include "Object.h"

namespace game{

	class State;
	//class shared_Object_ptr;

	typedef unsigned int LAYER_STATEINDEX;
	typedef std::vector<shared_Object_ptr> LayerObjects;

	class ALayer{

		friend State;

		LayerObjects m_objects;

		Timer<double> m_timer;
		LAYER_STATEINDEX m_currentStateIndex;

		//------------------------------------------------------------------------
		// updates state timer and call updates with timer time
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p ){

			m_timer.Update( dDeltaTime_p );

			VUpdate( m_timer.GetTime(), m_timer.GetDelta() );

			// update game::Objects

			for( LayerObjects::const_iterator it = m_objects.begin(), itEnd = m_objects.end();
				 it != itEnd;
				 ++it ){

					 if( (*it)->m_bActive ){
						 
						 (*it)->Update( m_timer.GetTime(), m_timer.GetDelta() );
					 }
			}
		}


	public:

		bool m_bActive;
		
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
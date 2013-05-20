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
#include <algorithm>

// private includes
#include "Timer.h"
#include "Object.h"

namespace game{

	class State;

	typedef unsigned int LAYER_STATEINDEX;
	static const unsigned int INVALID_STATEINDEX = (unsigned int)-1;
	typedef std::vector<shared_Object_ptr> LayerObjects;

	class Layer{

		friend State;

	public:
		bool m_bActive;
		Timer<double> m_timer;

	private:
		LAYER_STATEINDEX m_currentStateIndex;
		LayerObjects m_objects;


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

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Layer( bool bActive_p = true ):m_bActive(bActive_p), m_currentStateIndex(INVALID_STATEINDEX){}
		~Layer(){}
		
		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VInit(){}
		virtual void VUpdate( const double /*dTime_p*/, const double /*dDeltaTime_p*/ ){};
		virtual void VDraw( const double /*dInterpolation_p*/ ){}
		virtual void VDestroy(){}


		//------------------------------------------------------------------------
		// return index of this layer in the state layers container
		//------------------------------------------------------------------------
		LAYER_STATEINDEX GetStateIndex(){return m_currentStateIndex;}

		//------------------------------------------------------------------------
		// object to be updated per loop
		//------------------------------------------------------------------------
		void AddObject( shared_Object_ptr && object_p ){

			m_objects.push_back(object_p);
			object_p->m_currentLayerIndex = (OBJECT_LAYERINDEX)m_objects.size()-1;
		}
		void AddObject( const shared_Object_ptr & object_p ){

			m_objects.push_back(object_p);
		}
		//
		void RmeoveObject( OBJECT_LAYERINDEX objectCurrentIndex_p ){

			//m_objects[objectCurrentIndex_p]->VDestroy();

			std::swap( m_objects[objectCurrentIndex_p], m_objects[m_objects.size()-1] );
			m_objects[objectCurrentIndex_p]->m_currentLayerIndex = objectCurrentIndex_p; // update index
			m_objects.pop_back();
		}
	};

	typedef std::shared_ptr<Layer> shared_Layer_ptr;
}
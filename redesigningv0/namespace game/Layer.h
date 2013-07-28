#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/04
	created:	4:5:2013   17:36
	file:		Layer.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	a state is composed of layers (in game, HUD, pause menu, debug console, fade splash..)
				the layer is the last "stage", where the game objects needs to lie to be updated

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <algorithm>
#include <assert.h>

// private includes
#include "Timer.h"
#include "Object.h"

namespace game{

	class State;

	typedef unsigned int LAYER_STATEINDEX;
	typedef std::vector<OBJECT_LAYERINDEX> ObjectIndexes;
	static const unsigned int INVALID_STATEINDEX = (unsigned int)-1;
	typedef std::vector<shared_Object_ptr> LayerObjects;

	//========================================================================
	// 
	//========================================================================
	class Layer{

		friend State;

	public:
		bool m_bActive;
		Timer<double> m_timer;

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Layer( bool bActive_p = true ):m_bActive(bActive_p), m_currentStateIndex(INVALID_STATEINDEX), m_pStateOwner(nullptr){}
		virtual ~Layer(){}

		//------------------------------------------------------------------------
		// object to be updated per loop
		//------------------------------------------------------------------------
		void AddObject( shared_Object_ptr && object_p ){

			assert( object_p->m_currentLayerIndex == INVALID_OBJECTINDEX );

			object_p->m_pLayerOwner = this;
			object_p->m_currentLayerIndex = (OBJECT_LAYERINDEX)m_objects.size();
			m_objects.push_back(std::move(object_p));
		}
		void AddObject( const shared_Object_ptr & object_p ){

			assert( object_p->m_currentLayerIndex == INVALID_OBJECTINDEX );

			object_p->m_pLayerOwner = this;
			object_p->m_currentLayerIndex = (OBJECT_LAYERINDEX)m_objects.size();
			m_objects.push_back(object_p);
		}
		//
		void RemoveObject( OBJECT_LAYERINDEX objectCurrentIndex_p ){

			m_removedObjects.push_back( objectCurrentIndex_p );
		}

		//------------------------------------------------------------------------
		// return index of this layer in the state layers container
		//------------------------------------------------------------------------
		LAYER_STATEINDEX GetStateIndex(){return m_currentStateIndex;}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		State * GetStateOwner(){ return m_pStateOwner; }

	protected:

		State * m_pStateOwner;

	private:

		LAYER_STATEINDEX m_currentStateIndex;
		LayerObjects m_objects;
		ObjectIndexes m_removedObjects;
		

		//------------------------------------------------------------------------
		// updates state timer and call updates with timer time
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p ){

			m_timer.Update( dDeltaTime_p );

			VOnUpdate( m_timer.GetTime(), m_timer.GetDelta() );

			// update game::Objects

			for( int it = 0, itEnd = (int)m_objects.size(); it != itEnd; ++it ){

				if( m_objects[it]->m_bActive ){
						 
					m_objects[it]->Update( m_timer.GetTime(), m_timer.GetDelta() );
				}
			}

			VLateUpdate( m_timer.GetTime(), m_timer.GetDelta() );

			// clean removed objects

			if( ! m_removedObjects.empty() )
				CleanRemovedObjects();
		}
		
		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VOnInit(){}
		virtual void VOnUpdate( const double /*dTime_p*/, const double /*dDeltaTime_p*/ ){} // called before objects update
		virtual void VLateUpdate( const double /*dTime_p*/, const double /*dDeltaTime_p*/ ){} // called after objects update
		virtual void VOnDraw( const double /*dInterpolation_p*/ ){}
		virtual void VOnDestroy(){}

		virtual void VOnResize( int /*W_p*/, int /*H_p*/ ){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void CleanRemovedObjects(){

			// swap all destroyed layers to the end of the vector than resizes

			unsigned int nDestroyed = (unsigned int)m_removedObjects.size(); // cache
			unsigned int nObjects = (unsigned int)m_objects.size();

			for( unsigned int it = 0, itLast = nObjects - 1; it < nDestroyed; ++it ){

				m_objects[m_removedObjects[it]]->m_currentLayerIndex = INVALID_OBJECTINDEX;
				m_objects[m_removedObjects[it]]->m_pLayerOwner = nullptr;

				// check if "to be removed" already at end

				if( m_removedObjects[it] == itLast - it){ // increment here

					continue;
				}

				std::swap( m_objects[m_removedObjects[it]], m_objects[itLast - it] ); // size - 1, size -2, size -3

				m_objects[m_removedObjects[it]]->m_currentLayerIndex = m_removedObjects[it]; // update index
			}

			// "trim"
			m_objects.resize(nObjects - nDestroyed);

			m_removedObjects.clear();
		}
	};

	typedef std::shared_ptr<Layer> shared_Layer_ptr;
}
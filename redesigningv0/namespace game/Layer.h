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

// private includes
#include "Timer.h"
#include "Object.h"

namespace game{

	class State;

	typedef unsigned int LAYERINDEX;
	static const unsigned int INVALID_LAYERINDEX = (unsigned int)-1;

	typedef std::vector<shared_Object_ptr> LayerObjects;
	typedef std::vector<OBJECTINDEX> ObjectIndexes;

	//========================================================================
	// 
	//========================================================================
	class Layer{

		friend State;

	public:

		bool m_bActive; bool m_bVisible;
		Timer<double> m_timer;

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Layer( bool bActive_p = true, bool bVisible_p = true )
			:
			m_bActive(bActive_p), m_bVisible(bVisible_p),
			m_currentLayerIndex(INVALID_LAYERINDEX),
			m_pStateOwner(nullptr),
			m_bDead(true){}

		virtual ~Layer(){}

		//------------------------------------------------------------------------
		// object to be updated per loop
		//------------------------------------------------------------------------
		void AddObject( shared_Object_ptr && pObject_p ){

			assert( pObject_p->m_bDead );

			pObject_p->m_bDead = false;

			if( pObject_p->m_currentObjectIndex == INVALID_OBJECTINDEX
				||
				pObject_p->m_pLayerOwner != this ){

				pObject_p->m_currentObjectIndex = (OBJECTINDEX)m_objects.size();
				pObject_p->m_pLayerOwner = this;
				
				//pObject_p->VOnInit();

				m_objects.push_back( std::move(pObject_p) );
			}
			else{

				//pObject_p->VOnInit();
			}			
		}
		void AddObject( const shared_Object_ptr & pObject_p ){

			assert( pObject_p->m_bDead );

			pObject_p->m_bDead = false;

			if( pObject_p->m_currentObjectIndex == INVALID_OBJECTINDEX
				||
				pObject_p->m_pLayerOwner != this ){

				pObject_p->m_currentObjectIndex = (OBJECTINDEX)m_objects.size();
				pObject_p->m_pLayerOwner = this;

				m_objects.push_back( pObject_p );
			}

			//pObject_p->VOnInit();
		}
		void RemoveObject( OBJECTINDEX objectCurrentIndex_p ){

			assert( objectCurrentIndex_p != INVALID_OBJECTINDEX );
			assert( !m_objects[objectCurrentIndex_p]->m_bDead );

			m_objects[objectCurrentIndex_p]->m_bDead = true;

			m_removedObjects.push_back( objectCurrentIndex_p );
		}
		void RemoveObject( const shared_Object_ptr & pObject_p ){

			RemoveObject(pObject_p->m_currentObjectIndex);
		}
		void RemoveObject( const Object * pObject_p ){

			RemoveObject(pObject_p->m_currentObjectIndex);
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		State * GetStateOwner() const { return m_pStateOwner; }

	protected:

		State * m_pStateOwner;

	private:

		LAYERINDEX m_currentLayerIndex;
		bool m_bDead;

		LayerObjects m_objects;
		ObjectIndexes m_removedObjects;
		

		//------------------------------------------------------------------------
		// updates state timer and call updates with timer time
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p ){

			m_timer.Update( dDeltaTime_p );

			VOnUpdate( m_timer.GetTime(), m_timer.GetDelta() );

			// update systems?



			// objects dont have updates, only systems that work on objects components,
			// so to clean components removed:

			for( int itObject = 0, itSize = (int)m_objects.size();
				 itObject < itSize;
				 ++itObject ){

				if( !m_objects[itObject]->m_removedComponents.empty() )
					m_objects[itObject]->CleanRemovedComponents();
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

			unsigned int nRemoved =	(unsigned int)m_removedObjects.size(); // cache
			unsigned int nObjects =		(unsigned int)m_objects.size();

			for( unsigned int itRemoved = 0, itLast = nObjects - 1;
				 itRemoved < nRemoved;
				 ++itRemoved, --itLast ){

					if( !m_objects[m_removedObjects[itRemoved]]->m_bDead ){

						--nRemoved;
						continue; // untested
					}

					//m_objects[m_removedObjects[itRemoved]]->Abort();
					m_objects[m_removedObjects[itRemoved]]->m_currentObjectIndex = INVALID_OBJECTINDEX;
					m_objects[m_removedObjects[itRemoved]]->m_pLayerOwner = nullptr;

					if( m_removedObjects[itRemoved] == itLast ){

						continue;
					}

					std::swap( m_objects[m_removedObjects[itRemoved]], m_objects[itLast] );

					if( m_objects[m_removedObjects[itRemoved]]->m_bDead ){

						// find the swaped task on the list to be destroyed, update the index

						for( unsigned int itToBeDestroyed = itRemoved; itToBeDestroyed < nRemoved; ++itToBeDestroyed ){

							if( m_removedObjects[itToBeDestroyed] == m_objects[m_removedObjects[itRemoved]]->m_currentObjectIndex ){

								m_removedObjects[itToBeDestroyed] = m_removedObjects[itRemoved];
							}
						}
					}
					else{

						m_objects[m_removedObjects[itRemoved]]->m_currentObjectIndex = m_removedObjects[itRemoved];
					}
			}

			// "trim"

			m_objects.resize(nObjects - nRemoved);

			m_removedObjects.clear();
		}
	};

	typedef std::shared_ptr<Layer> shared_Layer_ptr;
	typedef std::weak_ptr<Layer> weak_Layer_ptr;
}
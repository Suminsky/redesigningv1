#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/14
	created:	14:4:2013   18:54
	file:		Object.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	The game object, the rice of the game.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <assert.h>
#include <memory>
#include <vector>

// private includes

#include "Component.h"
#include "EventMachine.h"

namespace gen{
	template<typename T>	class Pool;
}

namespace game{

	class ObjectMachine;
	class ObjectFactory;
	class Layer;
	class System;
	class Component;

	//typedef unsigned int ObjectID;

	typedef unsigned int OBJECTINDEX;
	static const unsigned int INVALID_OBJECTINDEX = (unsigned int)-1;
	
	typedef std::vector<pool_Component_ptr> ObjectComponents;

	//========================================================================
	// 
	//========================================================================
	class Object: public gen::NonCopyable{

		friend class Layer;
		friend class ObjectMachine;
		friend class ObjectFactory;

		DCL_POOLELEMENT();

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Object()
			:
			m_currentObjectIndex(INVALID_OBJECTINDEX),
			m_pLayerOwner(nullptr),
			m_pObjMachineOwner(nullptr),
			m_bDettached(true){

				static int s_count = 0;
				m_ID = s_count++;
				m_szName[0] = 0;
				m_prefab = -1;
			}

		~Object(){

			//TODO: dettach components?
			//BREAKHERE;
			// 
			for( int it = 0, nCompo = (int)m_components.size(); it < nCompo; ++it ){

				m_components[it]->m_bDettached = true;
			}
		}

		//------------------------------------------------------------------------
		// component stuff
		//------------------------------------------------------------------------
		void AttachComponent( pool_Component_ptr && pComponent_p );
		void AttachComponent( const pool_Component_ptr & pComponent_p );
		void DettachComponent( COMPONENTINDEX componentCurrentIndex_p );
		void DettachComponent( const pool_Component_ptr & pComponent_p );
		void DettachComponent( Component * pComponent_p );

		//------------------------------------------------------------------------
		// event/message stuff
		//------------------------------------------------------------------------
		void RegisterForComponentEvent( EventMachine<ComponentEventData>::EventHandler eveHandlerDelegate_p, EventType eveType_p );
		void UnregisterForComponentEvent( EventMachine<ComponentEventData>::EventHandler eveHandlerDelegate_p, EventType eveType_p );
		void AddComponentEvent( EventType eveType_p, ComponentEventData eveData_p );
		void DispatchComponentEventImmediately( EventType eveType_p, ComponentEventData eveData_p );

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		Layer * GetLayerOwner() const { return m_pLayerOwner; }
		ObjectComponents & GetComponents(){	return m_components; }
		bool IsAttached() const { return !m_bDettached; }
		template< typename DerivedComponent >
		gen::pool_ptr<DerivedComponent> GetFirstOfComponent(){

			for( int it = 0, iSize = (int)m_components.size();
		         it < iSize;
				 ++it ){

				if( m_components[it]->GetType() == COMPONENT_TYPE(DerivedComponent) ){
				
					return m_components[it];
				}
			}

			return gen::pool_ptr<DerivedComponent>();
		}
		pool_Component_ptr GetFirstOfComponent( const unsigned int iType_p ){

			for( int it = 0, iSize = (int)m_components.size();
				it < iSize;
				++it ){

					if( m_components[it]->GetType() == iType_p ){

						return m_components[it];
					}
			}

			return pool_Component_ptr();
		}
		template< typename DerivedComponent >
		gen::pool_ptr<DerivedComponent> GetNthOfComponent( int nth_p ){

			for( int it = 0, currentTH = 0, iSize = (int)m_components.size();
				it < iSize;
				++it ){

					if( m_components[it]->GetType() == COMPONENT_TYPE(DerivedComponent) ){

						if( currentTH == nth_p ) return m_components[it];

						++currentTH;
					}
			}

			return gen::pool_ptr<DerivedComponent>();
		}
		int GetID() const { return m_ID; }
		const char* GetName()const{ return &m_szName[0]; }
		int GetPrefab() const{ return m_prefab;}
		void SetPrefab(int prefab){ m_prefab = prefab;}//HACK, TODO remove

		//------------------------------------------------------------------------
		// setters
		//------------------------------------------------------------------------
		void SetName( const char * newName_p ){

			memcpy(m_szName, newName_p, sizeof(char)*(gen::stringUtil::CountString(newName_p,64)+1) );
		}

	protected:

		Layer * m_pLayerOwner;
		ObjectMachine * m_pObjMachineOwner;

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void DispatchComponentEvents();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void CleanRemovedComponents();


		OBJECTINDEX m_currentObjectIndex;
		bool m_bDettached;

		ObjectComponents m_components;
		std::vector<Component*> m_removedComponents;

		EventMachine<ComponentEventData> m_objectEventMachine;

		// TODO: make obj a huge block of mem, using max compos, eves and eve handlers

		int m_ID;
		char m_szName[64];
		int m_prefab;
	};

	typedef gen::pool_ptr<Object> pool_Object_ptr;
}
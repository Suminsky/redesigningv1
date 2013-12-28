#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/14
	created:	14:4:2013   18:54
	file:		Object.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	The game object, the rice of the game.
				NOTE:
				Cache friendly idea:
				store all components (derived from Component) instances in a contiguous block (std::vector)
				and make objects point to then based on a pointer to type plus a pointer to the right instance.
				To update the actors, instead of iterating actors, one would update all components of the same
				type, till all types.

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

namespace game{

	class ObjectMachine;
	class Layer;
	class System;
	class Component;

	//typedef unsigned int ObjectID;

	typedef unsigned int OBJECTINDEX;
	static const unsigned int INVALID_OBJECTINDEX = (unsigned int)-1;
	
	typedef std::vector<pool_Component_ptr> ObjectComponents;
	typedef std::vector<COMPONENTINDEX> ComponentIndexes;

	//========================================================================
	// 
	//========================================================================
	class Object{

		friend class Layer;
		friend class ObjectMachine;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Object()
			:
			m_currentObjectIndex(INVALID_OBJECTINDEX),
			m_pLayerOwner(nullptr),
			m_pObjMachineOwner(nullptr),
			m_bDettached(true){}

		virtual ~Object(){
			BREAKHERE;
		}

		//------------------------------------------------------------------------
		// component stuff
		//------------------------------------------------------------------------
		void AttachComponent( pool_Component_ptr && pComponent_p );
		void AttachComponent( const pool_Component_ptr & pComponent_p );
		void DettachComponent( COMPONENTINDEX componentCurrentIndex_p );
		void DettachComponent( const pool_Component_ptr & pComponent_p );
		void DettachComponent( const Component * pComponent_p );

		//------------------------------------------------------------------------
		// event/message stuff
		//------------------------------------------------------------------------
		void RegisterForComponentEvent( EventMachine<ComponentEventData>::EventHandlerDelegate eveHandlerDelegate_p, EventType eveType_p );
		void UnregisterForComponentEvent( EventMachine<ComponentEventData>::EventHandlerDelegate eveHandlerDelegate_p, EventType eveType_p );
		void AddComponentEvent( EventType eveType_p, ComponentEventData eveData_p );
		void DispatchComponentEventImmediately( EventType eveType_p, ComponentEventData eveData_p );

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		Layer * GetLayerOwner() const { return m_pLayerOwner; }
		ObjectComponents & GetComponents(){	return m_components; }
		bool IsAttached() const { return !m_bDettached; }

		// TODO: this method used to return WEAK ptr
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
		ComponentIndexes m_removedComponents;

		EventMachine<ComponentEventData> m_objectEventMachine;
	};

	typedef std::shared_ptr<Object> shared_Object_ptr;
	typedef std::weak_ptr<Object> weak_Object_ptr;
}
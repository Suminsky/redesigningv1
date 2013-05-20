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

#include <memory>
#include <unordered_map>

// private includes

#include "Component.h"

namespace game{

	class Component;

	typedef unsigned int ObjectID;
	typedef unsigned int OBJECT_LAYERINDEX;
	static const unsigned int INVALID_LAYERINDEX = (unsigned int)-1;
	
	typedef std::vector<shared_Component_ptr> ObjectComponents;

	//========================================================================
	// 
	//========================================================================
	class Object{

		friend class State;
		friend class Layer;

	public:
		bool m_bActive;

	private:
		ObjectID m_ID;
		OBJECT_LAYERINDEX m_currentLayerIndex;
		ObjectComponents m_components;

	public:
		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Object( bool bActive_p = true ):m_bActive(bActive_p), m_currentLayerIndex(INVALID_LAYERINDEX){}
		virtual ~Object(){}

		//------------------------------------------------------------------------
		// update the object components
		//------------------------------------------------------------------------
		void Update( double dTime_p, double dDelta_p ){

			for( ObjectComponents::iterator it = m_components.begin(), itEnd = m_components.end();
					it != itEnd; ++ it ){

					(*it)->VOnUpdate( dTime_p, dDelta_p );
			}
		}

		//------------------------------------------------------------------------
		// get id
		//------------------------------------------------------------------------
		ObjectID ID() const{

			return m_ID;
		}

		//------------------------------------------------------------------------
		// get component by id (the id is the index on the object)
		//------------------------------------------------------------------------
		template<class DerivedComponent>
		std::weak_ptr<DerivedComponent> GetComponent( COMPONENT_OBJECTINDEX index_p ) const {

			return m_components[index_p];
		}

		//------------------------------------------------------------------------
		// component stuff
		//------------------------------------------------------------------------
		void AddComponent( shared_Component_ptr && pComponent_p ){

			m_components.push_back( pComponent_p );
			pComponent_p->m_currentObjectIndex = (COMPONENT_OBJECTINDEX)m_components.size()-1;
		}
		void AddComponent( const shared_Component_ptr & pComponent_p ){

			m_components.push_back( pComponent_p );
			pComponent_p->m_currentObjectIndex = (COMPONENT_OBJECTINDEX)m_components.size()-1;
		}
		void RemoveComponent( COMPONENT_OBJECTINDEX componentCurrentIndex_p ){

			std::swap( m_components[componentCurrentIndex_p], m_components[m_components.size()-1] );
			m_components[componentCurrentIndex_p]->m_currentObjectIndex = componentCurrentIndex_p; // update index
			m_components.pop_back();
		}
	};

	typedef std::shared_ptr<Object> shared_Object_ptr;
}
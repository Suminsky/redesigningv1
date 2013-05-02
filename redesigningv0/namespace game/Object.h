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
				store all components (derived from AComponent) instances in a contiguous block (std::vector)
				and make objects point to then based on a pointer to type plus a pointer to the right instance.
				To update the actors, instead of iterating actors, one would update all components of the same
				type, till all types.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standar includes

#include <memory>
#include <unordered_map>

// private includes

#include "Component.h"

namespace game{

	class AComponent;

	typedef unsigned int ObjectID;

	//========================================================================
	// 
	//========================================================================
	class Object{
	protected:

		ObjectID m_ID;
		std::vector<shared_AComponent_ptr> m_components;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Object(){}
		virtual ~Object(){}

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
		std::weak_ptr<DerivedComponent> GetComponent( ComponentID id_p ) const {

			return m_components[id_p];
		}
	};

	typedef std::shared_ptr<Object> shared_Object_ptr;
}
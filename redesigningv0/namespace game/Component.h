#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/04/14
	created:	14:4:2013   19:49
	file:		IObjectComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	the life items that compose a game object

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#include <memory>

namespace game{

	class Object;

	typedef unsigned int ComponentID;

	//========================================================================
	// 
	//========================================================================
	class AComponent{

		friend class Object;

	protected:

		ComponentID m_ID;
		Object * m_pOwner;

	public:

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~AComponent() = 0;

		//------------------------------------------------------------------------
		// to be override
		//------------------------------------------------------------------------
		virtual void VOnUpdate( double /*dTime_p*/, double /*dDeltaTime_p*/ ) = 0;

		//------------------------------------------------------------------------
		// get id
		//------------------------------------------------------------------------
		ComponentID ID(){

			return m_ID;
		}
	};

	typedef std::shared_ptr<AComponent> shared_AComponent_ptr;
}
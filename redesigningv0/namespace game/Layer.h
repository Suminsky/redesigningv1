#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/04
	created:	4:5:2013   17:36
	file:		Layer.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	a state is composed of layers (in game, HUD, pause menu, debug console, fade splash..)
				the layer is the last "stage", where the game objects needs to lie to be updated

				a layer have objects and systems, systems do work on specific objects components.
				so the layer work is to inform systems about components changes

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <algorithm>

// private includes
#include "Timer.h"
#include "ObjectMachine.h"
#include "SystemMachine.h"
#include "ComponentFactory.h"
#include "ObjectFactory.h"

namespace game{

	// forward dcls

	class State;

	// new types

	typedef unsigned int LAYERINDEX;
	static const unsigned int INVALID_LAYERINDEX = (unsigned int)-1;

	typedef std::vector<pool_Object_ptr> LayerObjects;
	typedef std::vector<OBJECTINDEX> ObjectIndexes;

	//========================================================================
	// 
	//========================================================================
	class Layer{

		friend State;

	public:

		bool m_bActive;
		bool m_bVisible;
		Timer<double> m_timer;

		ObjectFactory		m_objFactory;
		SystemMachine		m_systems;
		ComponentFactory	m_componentFactory;
		ObjectMachine		m_objects;
		

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Layer( uint32_t nMaxObjs_p, bool bActive_p = true, bool bVisible_p = true )
			:
			m_bActive(bActive_p), m_bVisible(bVisible_p),
			m_currentLayerIndex(INVALID_LAYERINDEX),
			m_pStateOwner(nullptr),
			m_bDettached(true),
			m_objFactory(nMaxObjs_p)
			{
				m_objFactory.SetLayer(this);
				m_objects.SetLayer(this);
				m_systems.SetLayer(this);
			}

		virtual ~Layer(){}

		//------------------------------------------------------------------------
		// objects (bridge for object machine)
		//------------------------------------------------------------------------
		void AddObject( pool_Object_ptr && pObject_p );
		void AddObject( const pool_Object_ptr & pObject_p );
		void RemoveObject( OBJECTINDEX objectCurrentIndex_p );
		void RemoveObject( const pool_Object_ptr & pObject_p );
		void RemoveObject( const Object * pObject_p );

		pool_Object_ptr CreateObject(){

			return m_objFactory.CreateObject();
		}
		pool_Object_ptr CreateObject( text::GfigElementA *pGfig_p ){

			return m_objFactory.CreateObject(pGfig_p);
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		State * GetStateOwner() const { return m_pStateOwner; }

		bool IsAttached() const { return !m_bDettached; }

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void SerializeAttachedObjs( text::GfigElementA *pGfig_p );

	protected:

		State * m_pStateOwner;

	private:

		//------------------------------------------------------------------------
		// updates state timer and call updates with timer time
		//------------------------------------------------------------------------
		void Update( const double dDeltaTime_p );
		void Draw( const double /*dInterpolation_p*/, const double dDelta_p = 0.0 );

		LAYERINDEX m_currentLayerIndex;
		bool m_bDettached;
		

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual void VOnAttach(){}
		virtual void VOnUpdate(		const double /*dTime_p*/, const double /*dDeltaTime_p*/ ){} // called before objects update
		virtual void VLateUpdate(	const double /*dTime_p*/, const double /*dDeltaTime_p*/ ){} // called after objects update
		virtual void VOnDraw(		const double /*dInterpolation_p*/, const double /*dDelta_p = 0.0*/ ){}
		virtual void VOnRemove(){}

		virtual void VOnResize( int /*W_p*/, int /*H_p*/ ){}		
	};

	typedef std::shared_ptr<Layer> shared_Layer_ptr;
	typedef std::weak_ptr<Layer> weak_Layer_ptr;
}
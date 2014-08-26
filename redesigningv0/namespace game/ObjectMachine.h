#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/10
	created:	10:11:2013   23:45
	file:		ObjectMachine.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "Object.h"

namespace game{

	class Layer;

	typedef std::vector<shared_Object_ptr> Objects;
	typedef std::vector<OBJECTINDEX> ObjectIndexes;

	class ObjectMachine{

		friend class Layer;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ObjectMachine(){}
		explicit ObjectMachine( Layer * pLayer_p )
			:
		m_pLayerRef(pLayer_p)
		{}

		//------------------------------------------------------------------------
		// objects
		//------------------------------------------------------------------------
		void AddObject( shared_Object_ptr && pObject_p );
		void AddObject( const shared_Object_ptr & pObject_p );
		void RemoveObject( OBJECTINDEX objectCurrentIndex_p );
		void RemoveObject( const shared_Object_ptr & pObject_p );
		void RemoveObject( Object * pObject_p );

		Layer * GetLayer() const { return m_pLayerRef; }

	private:

		//------------------------------------------------------------------------
		// internal object events
		//------------------------------------------------------------------------
		void DispatchObjectInternalEvents();

		//------------------------------------------------------------------------
		// cleaning
		//------------------------------------------------------------------------
		void CleanRemovedObjectComponents();
		void CleanRemovedObjects();

		void SetLayer( Layer * pLayer_p ){ m_pLayerRef = pLayer_p; }


		Objects m_objects;
		std::vector<Object*> m_removedObjects;

		Layer * m_pLayerRef;

	};

	typedef std::shared_ptr<ObjectMachine> shared_ObjectMachine_ptr;
	typedef std::weak_ptr<ObjectMachine> weak_ObjectMachine_ptr;
}
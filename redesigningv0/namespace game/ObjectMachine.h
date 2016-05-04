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
		void AddObject( pool_Object_ptr && pObject_p );
		void AddObject( const pool_Object_ptr & pObject_p );
		void RemoveObject( OBJECTINDEX objectCurrentIndex_p );
		void RemoveObject( const pool_Object_ptr & pObject_p );
		void RemoveObject( Object * pObject_p );

		Layer * GetLayer() const { return m_pLayerRef; }

		uint32_t GetSize() const { return (uint32_t)m_objects.size(); }

		pool_Object_ptr GetGameObject( const OBJECTINDEX it ) const { return m_objects[it]; }
		pool_Object_ptr GetGameObject( const char * szName ) const {

			for( uint32_t it = 0, n = (uint32_t)m_objects.size(); it < n; ++it ){

				if( gen::stringUtil::IsEqual( szName, m_objects[it]->m_szName, 64 ) )
					return m_objects[it];
			}

			return pool_Object_ptr();
		}
		pool_Object_ptr GetGameObject( const int ID ) const {

			for( uint32_t it = 0, n = (uint32_t)m_objects.size(); it < n; ++it ){

				if( m_objects[it]->m_ID == ID )
					return m_objects[it];
			}

			return pool_Object_ptr();
		}

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


		std::vector<pool_Object_ptr> m_objects;
		std::vector<Object*>		m_removedObjects;

		Layer * m_pLayerRef;
	};

	typedef std::shared_ptr<ObjectMachine> shared_ObjectMachine_ptr;
	typedef std::weak_ptr<ObjectMachine> weak_ObjectMachine_ptr;
}
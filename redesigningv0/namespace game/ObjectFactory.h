#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/22
	created:	22:11:2013   12:20
	file:		ObjectFactory.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <string>
#include <vector>

// private includes
#include "Object.h"

// forward dcls

namespace text{

	struct GfigElementA;
}

namespace game{

	class Layer;

	class ObjectFactory{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ObjectFactory(unsigned int maxObjs_p)
			:
		m_pool(maxObjs_p),
		m_pLayerOwner(nullptr)
		{

			int x =7; x = x;
		}
		~ObjectFactory(){

			BREAKHERE;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void SetLayer( Layer * pLayer_p ){ m_pLayerOwner = pLayer_p; }

		//------------------------------------------------------------------------
		// TODO:use a pool too
		//------------------------------------------------------------------------
		pool_Object_ptr CreateObject();
		pool_Object_ptr CreateObject( text::GfigElementA * pGfig_p );
		pool_Object_ptr CloneObject( const Object * pObj_p );
		void Serialize( const Object * pObj_p, text::GfigElementA * pGFig_p );
		void SerializePrefabs( text::GfigElementA * pGFig_p);

		void LoadNewPrefab( const Object * pObj_p, const std::string & szName_p );
		void LoadNewPrefab( text::GfigElementA * pGfig_p );
		void UnLoadAllPrefabs(){ m_prefabs.clear(); }

		unsigned int PrefabIndexFromName( const std::string & szName_p );
		pool_Object_ptr CreateObjectFromPrefab( unsigned int iPrefab_p );
		pool_Object_ptr CreateObjectFromPrefab( const std::string & szName_p );
		pool_Object_ptr CreateObjectFromPrefab( unsigned int iPrefab_p, text::GfigElementA * pIntanceGfig_p );
		pool_Object_ptr CreateObjectFromPrefab( const std::string & szName_p, text::GfigElementA * pIntanceGfig_p );

	private:

		gen::Pool<Object> m_pool;

		Layer				* m_pLayerOwner;

		struct Prefab{
			std::string szName; // TODOL obsolete, name now on obj
			pool_Object_ptr pObj;
		};
		std::vector<Prefab> m_prefabs;
	};

	typedef std::shared_ptr<ObjectFactory> shared_ObjectFactory_ptr;
	typedef std::weak_ptr<ObjectFactory> weak_ObjectFactory_ptr;
}
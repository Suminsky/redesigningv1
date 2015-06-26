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
#include <memory>
#include <string>
#include <vector>

// private includes

// forward dcls

namespace text{

	struct GfigElementA;
}

namespace game{

	class Object; typedef std::shared_ptr<Object> shared_Object_ptr;
	class Layer;

	class AObjectFactory{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		AObjectFactory()
			:
		m_pLayerOwner(nullptr)
		{}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void SetLayer( Layer * pLayer_p ){ m_pLayerOwner = pLayer_p; }

		//------------------------------------------------------------------------
		// TODO:use a pool too
		//------------------------------------------------------------------------
		virtual shared_Object_ptr VCreateObject();
		virtual shared_Object_ptr VCreateObject( text::GfigElementA * pGfig_p );
		virtual shared_Object_ptr VCloneObject( const Object * pObj_p );
		virtual void VSerialize( const Object * pObj_p, text::GfigElementA * pGFig_p );

		void LoadNewPrefab( const Object * pObj_p, const std::string & szName_p );
		void LoadNewPrefab( text::GfigElementA * pGfig_p );
		void UnLoadAllPrefabs(){ m_prefabs.clear(); }

		unsigned int PrefabIndexFromName( const std::string & szName_p );
		shared_Object_ptr CreateObjectFromPrefab( unsigned int iPrefab_p );
		shared_Object_ptr CreateObjectFromPrefab( const std::string & szName_p );
		shared_Object_ptr CreateObjectFromPrefab( unsigned int iPrefab_p, text::GfigElementA * pIntanceGfig_p );
		shared_Object_ptr CreateObjectFromPrefab( const std::string & szName_p, text::GfigElementA * pIntanceGfig_p );
		

	private:

		Layer * m_pLayerOwner;

		struct Prefab{
			std::string szName;
			shared_Object_ptr pObj;
		};

		std::vector<Prefab> m_prefabs;

	};

	typedef std::shared_ptr<AObjectFactory> shared_AObjectFactory_ptr;
	typedef std::weak_ptr<AObjectFactory> weak_AObjectFactory_ptr;
}
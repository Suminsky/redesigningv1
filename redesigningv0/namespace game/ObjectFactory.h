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
		void SetLayer( Layer * pLayer_p ){

			m_pLayerOwner = pLayer_p;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		virtual shared_Object_ptr VCreateObject();
		virtual shared_Object_ptr VCreateObject( text::GfigElementA * pGfig_p );

	private:

		Layer * m_pLayerOwner;

	};

	typedef std::shared_ptr<AObjectFactory> shared_AObjectFactory_ptr;
	typedef std::weak_ptr<AObjectFactory> weak_AObjectFactory_ptr;
}
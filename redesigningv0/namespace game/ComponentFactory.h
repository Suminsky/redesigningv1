#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/11
	created:	11:11:2013   9:34
	file:		ComponentFactory.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <map>
#include <assert.h>

// private includes
#include "ComponentTypes.h"
#include "../namespace gen/gen_macros.h"

namespace text{
	struct GfigElementA;
}
namespace gen{
	template<typename T> class pool_ptr;
}


namespace game{

	class Component; typedef gen::pool_ptr<Component> pool_Component_ptr;
	class ComponentFactory;

	//========================================================================
	// 
	//========================================================================
	class AComponentFactory{

		friend class ComponentFactory;

	public:

	private:

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent() = 0;
		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p ) = 0;
	};

	typedef std::shared_ptr<AComponentFactory> shared_AComponentFactory_ptr;
	typedef std::weak_ptr<AComponentFactory> weak_AComponentFactory_ptr;

	//========================================================================
	// 
	//========================================================================
	//TODO: use vector and binary search instead of map
	typedef std::map<int, shared_AComponentFactory_ptr> ComponentFactoryRegistry;
	typedef std::map<std::string, int> ComponentNameToTypeMap;

	class ComponentFactory{

	public:

		~ComponentFactory(){}

		//------------------------------------------------------------------------
		// creators
		//------------------------------------------------------------------------
		pool_Component_ptr CreateComponent( int iType_p );
		pool_Component_ptr CreateComponent( const char * szComponent_p );
		pool_Component_ptr CreateComponent( int iType_p, text::GfigElementA * pGFig_p );
		pool_Component_ptr CreateComponent( const char * szComponent_p, text::GfigElementA * pGFig_p );

		template< typename DerivedComponent > gen::pool_ptr<DerivedComponent> CreateComponent();
		template< typename DerivedComponent > gen::pool_ptr<DerivedComponent> CreateComponent( text::GfigElementA * pGFig_p );

		//------------------------------------------------------------------------
		// registers
		//------------------------------------------------------------------------
		template< typename DerivedComponent >
		void RegisterFactory( const shared_AComponentFactory_ptr & pFactory_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		shared_AComponentFactory_ptr GetComponentFactory( int iType_p );
		template< typename DerivedComponent > shared_AComponentFactory_ptr GetComponentFactory();

	private:

		ComponentFactoryRegistry m_registry;
		ComponentNameToTypeMap m_nameToType;
	};
}

#include "ComponentFactory.inl"
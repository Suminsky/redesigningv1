#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/11
	created:	11:11:2013   9:34
	file:		ComponentFactory.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	� Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
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


namespace game{

	class Component; typedef std::shared_ptr<Component> shared_Component_ptr;
	class ComponentFactories;

	//========================================================================
	// 
	//========================================================================
	class AComponentFactory{

		friend class ComponentFactories;

	public:

	private:

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual shared_Component_ptr VCreateComponent() = 0;
		virtual shared_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p ) = 0;
	};

	typedef std::shared_ptr<AComponentFactory> shared_AComponentFactory_ptr;
	typedef std::weak_ptr<AComponentFactory> weak_AComponentFactory_ptr;

	//========================================================================
	// 
	//========================================================================
	
	typedef std::map<int, shared_AComponentFactory_ptr> ComponentFactoryRegistry;
	typedef std::map<std::string, int> ComponentNameToTypeMap;

	class ComponentFactories{

	public:

		//------------------------------------------------------------------------
		// creators
		//------------------------------------------------------------------------
		shared_Component_ptr CreateComponent( int iType_p );
		shared_Component_ptr CreateComponent( const char * szComponent_p );
		shared_Component_ptr CreateComponent( int iType_p, text::GfigElementA * pGFig_p );
		shared_Component_ptr CreateComponent( const char * szComponent_p, text::GfigElementA * pGFig_p );

		template< typename DerivedComponent > std::shared_ptr<DerivedComponent> CreateComponent();
		template< typename DerivedComponent > std::shared_ptr<DerivedComponent> CreateComponent( text::GfigElementA * pGFig_p );

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
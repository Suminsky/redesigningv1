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
#include <vector>
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
	class IComponentFactory{

		friend class ComponentFactory;

	public:

		virtual ~IComponentFactory(){}

	private:

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent() = 0;
		virtual pool_Component_ptr VCloneComponent(  const Component * pCompo_p ) = 0;

		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p ) = 0;
		virtual void VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p ) = 0;

		virtual void VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p ) = 0;
		virtual void VSerialize( const Component * pCompo_p, const Component * pDefaults_p, text::GfigElementA * pGFig_p );

		virtual Component* VGetDefaultCompo();

		//virtual pool_Component_ptr VCreateComponent( gen::DataStream * pData_p ) = 0;
		//virtual void VUpdateComponent( Component * pCompo_p, gen::DataStream * pData_p ) = 0;
		//virtual void VSerialize( Component * pCompo_p, gen::DataStream * pData_p ) = 0;
		
		// TODO:
		// Every create compo will receive a defaults compo with pre initialized data
		// So this will be cloned, and updated...
		// so there will be no need for a "create" that sets defaults (althou it do less work than cloning and (re)updating)
		// This will also allow for initialization from prefab automatically
		// Each factory will have a local compo with the default value
		// Serialze will always compare against a compo, and the default serialize will serialize everything with no comparison
		// (cause it doesnt make sense to keep another one that compares against fixed values)
	};

	typedef std::shared_ptr<IComponentFactory> shared_IComponentFactory_ptr;
	typedef std::weak_ptr<IComponentFactory> weak_IComponentFactory_ptr;

	//========================================================================
	// 
	//========================================================================
	struct RegisteredFactory{

		unsigned int compoType;
		shared_IComponentFactory_ptr pCompoFactory;
		const char * szCompoName;
	};
	typedef std::vector<RegisteredFactory> ComponentFactoryRegistry;

	class ComponentFactory{

	public:

		~ComponentFactory(){}

		//------------------------------------------------------------------------
		// creators
		//------------------------------------------------------------------------
		pool_Component_ptr CreateComponent( unsigned int iType_p );
		pool_Component_ptr CreateComponent( const char * szComponent_p );
		pool_Component_ptr CreateComponent( unsigned int iType_p, text::GfigElementA * pGFig_p );
		pool_Component_ptr CreateComponent( const char * szComponent_p, text::GfigElementA * pGFig_p );
		pool_Component_ptr CloneComponent( const Component * pComponent_p );
		void UpdateComponent( Component * pComponent_p, text::GfigElementA * pGFig_p );

		template< typename DerivedComponent > gen::pool_ptr<DerivedComponent> CreateComponent();
		template< typename DerivedComponent > gen::pool_ptr<DerivedComponent> CreateComponent( text::GfigElementA * pGFig_p );
		template< typename DerivedComponent > gen::pool_ptr<DerivedComponent> CloneComponent( const DerivedComponent * pCompo_p );

		void Serialize( const Component * pComponent_p,  text::GfigElementA * pGFig_p );
		void Serialize( const Component * pComponent_p, const Component * pDefaults_p, text::GfigElementA * pGFig_p );

		Component* GetDefaultCompo( unsigned int iType_p );
		//------------------------------------------------------------------------
		// registers
		//------------------------------------------------------------------------
		template< typename DerivedComponent >
		void RegisterFactory( const shared_IComponentFactory_ptr & pFactory_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		unsigned int GetCompoTypeFromName( const std::string & szCompoName_p );
		//const std::string & GetCompoNameFromType( const unsigned int iType_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		shared_IComponentFactory_ptr GetComponentFactory( unsigned int iType_p );
		template< typename DerivedComponent > shared_IComponentFactory_ptr GetComponentFactory();
		
	private:

		ComponentFactoryRegistry m_registry;
	};
}

#include "ComponentFactory.inl"
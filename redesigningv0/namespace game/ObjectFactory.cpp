#include "ObjectFactory.h"
#include "Object.h"
#include "Layer.h"
#include "../namespace text/TextParser.h"

using namespace game;

shared_Object_ptr AObjectFactory::VCreateObject(){

	return std::make_shared<Object>();
}

shared_Object_ptr AObjectFactory::VCreateObject( text::GfigElementA * pGfig_p ){

	shared_Object_ptr pObj = std::make_shared<Object>();

	for( int it = 0, iSize = (int)pGfig_p->m_subElements.size();
		it<iSize;
		++it ){

		text::GfigElementA & gfigComponent = pGfig_p->m_subElements[it];

		pool_Component_ptr pCompo = m_pLayerOwner->m_componentFactory.CreateComponent( gfigComponent.m_name.c_str(), &gfigComponent );
		if( pCompo ){

			pObj->AttachComponent( std::move(pCompo) );
		}
	}

	return pObj;
}
#include "ParentingComponent.h"

using namespace game;

void game::ParentingComponent::AttachObject( ParentingComponent * pObject_p )
{
	pObject_p->SetParent( this );
	pObject_p->m_indexOnParent = (int)m_vChilds.size();

	m_vChilds.push_back( pObject_p );
}

void game::ParentingComponent::DettachObject( ParentingComponent * pObject_p )
{
	assert( pObject_p->m_pParent == this );

	pObject_p->m_pParent = nullptr;
	pObject_p->m_indexOnParent = INVALID_INDEX_ONPARENT;

	int itLast = (int)m_vChilds.size();

	std::swap( m_vChilds[pObject_p->m_indexOnParent], m_vChilds[itLast] );
	m_vChilds.pop_back();
}

void game::ParentingComponent::SetParent( ParentingComponent * pNewParent_p )
{
	if( m_pParent ){

		m_pParent->DettachObject( this );
	}

	m_pParent->AttachObject( pNewParent_p );
}

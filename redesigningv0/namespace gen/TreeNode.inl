#include "TreeNode.h"

using namespace gen;

template< typename T >
void TreeNode<T>::AddNode( TreeNode * pNode_p )
{
	pNode_p->SetParent( this );
	pNode_p->m_indexOnParent = (int)m_vChilds.size();

	m_vChilds.push_back( pNode_p );
}

template< typename T >
void TreeNode<T>::RemoveNode( TreeNode * pNode_p )
{
	assert( pNode_p->m_pParent == this );

	pNode_p->m_pParent = nullptr;
	pNode_p->m_indexOnParent = INVALID_INDEX_ONPARENT;

	int itLast = (int)m_vChilds.size();

	std::swap( m_vChilds[pNode_p->m_indexOnParent], m_vChilds[itLast] );
	m_vChilds.pop_back();
}

template< typename T >
void TreeNode<T>::SetParent( TreeNode * pNewParent_p )
{
	// NOTE: it doesnt add itself to the new parent, AddNode does that

	if( m_pParent ){

		m_pParent->RemoveNode( this );
	}

	m_pParent = pNewParent_p;
}
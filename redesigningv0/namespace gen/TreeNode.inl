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

	int itLast = (int)m_vChilds.size()-1;

	// "swap"
	m_vChilds[pNode_p->m_indexOnParent] = m_vChilds[itLast];
	m_vChilds[pNode_p->m_indexOnParent]->m_indexOnParent = pNode_p->m_indexOnParent;

	pNode_p->m_pParent = nullptr;
	pNode_p->m_indexOnParent = INVALID_INDEX_ONPARENT;

	m_vChilds.pop_back();
}

template< typename T>
void gen::TreeNode<T>::RemoveAllNodes()
{
	int nChilds = (int)m_vChilds.size();
	for( int it = 0; it < nChilds; ++it ){

		m_vChilds[it]->m_indexOnParent = INVALID_INDEX_ONPARENT;
		m_vChilds[it]->m_pParent = nullptr;
	}

	m_vChilds.clear();
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

template< typename T>
TreeNode<T>::~TreeNode()
{
	// remove all childs from this node

	for( int it = 0, iSize = (int)m_vChilds.size();
		 it < iSize;
		 ++it ){

			 m_vChilds[it]->m_pParent = nullptr;
			 m_vChilds[it]->m_indexOnParent = INVALID_INDEX_ONPARENT;
	}

	SetParent( nullptr );
}
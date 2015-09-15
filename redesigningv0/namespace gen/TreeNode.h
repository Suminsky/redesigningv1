#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/09
	created:	9:11:2013   12:55
	file:		Node.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	tree node
				controls parenting/hierarchy stuff

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <vector>

// private includes

namespace gen{

	typedef unsigned int INDEX_ONPARENT;
	static const unsigned int INVALID_INDEX_ONPARENT = (unsigned int)-1;

	template< typename T>
	class TreeNode{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		TreeNode()
			:
		m_indexOnParent(INVALID_INDEX_ONPARENT),
		m_pParent(nullptr){}
		~TreeNode();

		//------------------------------------------------------------------------
		// makes given node a child of this node
		//------------------------------------------------------------------------
		void AddNode( TreeNode * pNode_p );

		//------------------------------------------------------------------------
		// removes given node from the childs
		//------------------------------------------------------------------------
		void RemoveNode( TreeNode * pNode_p );
		void RemoveAllNodes();

		//------------------------------------------------------------------------
		// data management
		//------------------------------------------------------------------------
		T GetData() const{ return m_data; }
		void SetData( const T & data_p ){ m_data = data_p; }
		void SetData( T && data_p ){ m_data = std::move(data_p); }

		//------------------------------------------------------------------------
		// accessors
		//------------------------------------------------------------------------
		TreeNode * GetParent() const { return m_pParent; }
		unsigned int GetNumberOfChilds() const { return (unsigned int)m_vChilds.size(); }
		TreeNode * GetChild( INDEX_ONPARENT iIndex_p ) const { return m_vChilds[iIndex_p]; }

	private:

		//------------------------------------------------------------------------
		// set a new parent, removing this from current parent, if any
		//------------------------------------------------------------------------
		void SetParent( TreeNode * pNewParent_p );

		T m_data;

		TreeNode * m_pParent;
		std::vector<TreeNode*> m_vChilds;
		INDEX_ONPARENT m_indexOnParent;
	};
}

#include "TreeNode.inl"
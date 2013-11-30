#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/29
	created:	29:10:2013   14:23
	file:		ColorComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

#include <DirectXMath.h>

// private includes
#include "../Component.h"
#include "../../namespace gen/TreeNode.h"
#include "../../namespace gen/Pool.h"
#include "../ComponentFactory.h"

namespace gen{
	template<typename T, unsigned int SIZE>	class Pool;
}
namespace game{

	class ColorSystem;
	class ColorComponentFactory;

	//========================================================================
	// 
	//========================================================================
	class ColorComponent: public Component{

		friend class ColorSystem;
		friend class ColorComponentFactory;
		template<typename T, unsigned int SIZE> friend class gen::Pool;

	public:

		DirectX::XMFLOAT4 m_offsetColor;	// color that affects only the owner object
		DirectX::XMFLOAT4 m_localColor;	// color that affects the owner and its childs

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ColorComponent();

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		DirectX::XMFLOAT4 GetFinalColor() const{ return m_finalColor; }
		DirectX::XMFLOAT4 GetWorldColor() const{ return m_worldColor; }
		DirectX::XMFLOAT4 GetPreviousFinalColor() const{ return m_previousFinalColor; }

		//------------------------------------------------------------------------
		// parenting stuff
		//------------------------------------------------------------------------
		void AddChild( ColorComponent * pColor_p ){

			m_node.AddNode( &pColor_p->m_node);
		}
		void RemoveChild( ColorComponent * pColor_p ){

			m_node.RemoveNode( &pColor_p->m_node );
		}

	private:

		//------------------------------------------------------------------------
		// stores local * parent world
		// NOTE! should be called after UpdateLocalColor, otherwise it will return
		// a possibly outdated color
		//------------------------------------------------------------------------
		void UpdateWorldAndFinalColor( const DirectX::XMFLOAT4 & parentWorldColor_p );

		DirectX::XMFLOAT4 m_worldColor; // local * parent world
		DirectX::XMFLOAT4 m_finalColor;
		DirectX::XMFLOAT4 m_previousFinalColor;

		unsigned int m_iCurrentRosterIndex;

		gen::TreeNode<ColorComponent*> m_node;
	};


	typedef std::shared_ptr<ColorComponent> shared_ColorComponent_ptr;
	typedef std::weak_ptr<ColorComponent> weak_ColorComponent_ptr;

	//========================================================================
	// 
	//========================================================================

	class ColorComponentFactory: public AComponentFactory{

		friend class ColorSystem;

	private:

		gen::Pool<ColorComponent, 256> m_pool;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		DirectX::XMFLOAT4 GetRGBAFromGfig( text::GfigElementA * pGFig_p );

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual shared_Component_ptr VCreateComponent(){

			return MAKE_STACK_SHAREDPTR( ColorComponent, m_pool.Allocate() );

		}
		virtual shared_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );

	};

	typedef std::shared_ptr<ColorComponentFactory> shared_ColorComponentFactory_ptr;
	typedef std::weak_ptr<ColorComponentFactory> weak_ColorComponentFactory_ptr;
}
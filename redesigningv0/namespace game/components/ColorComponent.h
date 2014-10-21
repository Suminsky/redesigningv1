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
	template<typename T>	class Pool;
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
		bool GonnaSnap()const{ return m_bSnap; }

		//------------------------------------------------------------------------
		// parenting stuff
		//------------------------------------------------------------------------
		void AddChild( ColorComponent * pColor_p ){

			m_node.AddNode( &pColor_p->m_node);
		}
		void RemoveChild( ColorComponent * pColor_p ){

			m_node.RemoveNode( &pColor_p->m_node );
		}

		void Snap(){ m_bSnap = true; }

	private:

		bool m_bSnap;

		//------------------------------------------------------------------------
		// stores local * parent world
		// NOTE! should be called after UpdateLocalColor, otherwise it will return
		// a possibly outdated color
		//------------------------------------------------------------------------
		void UpdateWorldAndFinalColor( const DirectX::XMFLOAT4 & parentWorldColor_p );
		void UpdateWorldAndFinalColor();

		DirectX::XMFLOAT4 m_worldColor; // local * parent world
		DirectX::XMFLOAT4 m_finalColor;
		DirectX::XMFLOAT4 m_previousFinalColor;

		gen::TreeNode<ColorComponent*> m_node;
	};

	typedef gen::pool_ptr<ColorComponent> pool_ColorCompo_ptr;
	typedef std::shared_ptr<ColorComponent> shared_ColorComponent_ptr;
	typedef std::weak_ptr<ColorComponent> weak_ColorComponent_ptr;

	//========================================================================
	// 
	//========================================================================

	class ColorComponentFactory: public AComponentFactory{

		friend class ColorSystem;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ColorComponentFactory(unsigned int maxComponents_p)
			:
		m_pool(maxComponents_p){}

	private:

		gen::Pool<ColorComponent> m_pool;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		DirectX::XMFLOAT4 GetRGBAFromGfig( text::GfigElementA * pGFig_p );

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent(){

			return pool_Component_ptr(m_pool);

		}
		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );
		virtual pool_Component_ptr VCloneComponent( const Component* pCompo_p );

	};

	typedef std::shared_ptr<ColorComponentFactory> shared_ColorComponentFactory_ptr;
	typedef std::weak_ptr<ColorComponentFactory> weak_ColorComponentFactory_ptr;
}
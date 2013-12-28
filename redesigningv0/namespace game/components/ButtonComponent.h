#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/25
	created:	25:5:2013   21:58
	file:		ButtonComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "SpriteComponent_.h"
#include "../../namespace gen/gen_data.h"
#include "../../namespace gen/Pool.h"
#include "../ComponentFactory.h"

namespace game{

	template< typename DELEGATE_PARAM = int >
	class ButtonComponent: public Component{
	
	public:
		typedef gen::pool_ptr<ButtonComponent> pool_ButtonCompo_ptr;

		typedef gen::Delegate1Param<DELEGATE_PARAM> Delegate1Param;

		enum E_STATE{

			E_STATE_NORMAL = 0,
			E_STATE_HOVER,
			E_STATE_PRESSED
		};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ButtonComponent(){}
		~ButtonComponent(){
			BREAKHERE;
		}
		ButtonComponent( int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f, Delegate1Param callBack_p = Delegate1Param() );
		ButtonComponent( int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, DirectX::XMFLOAT4 uvPressed_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f, Delegate1Param callBack_p = Delegate1Param() );
		void Init(int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f, Delegate1Param callBack_p = Delegate1Param());
		void Init(int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, DirectX::XMFLOAT4 uvPressed_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f, Delegate1Param callBack_p = Delegate1Param());

		void SetHoverImage(){

			m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Hover;
			m_eState = E_STATE_HOVER;
		}
		void SetNormalImage(){

			m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Normal;
			m_eState = E_STATE_NORMAL;
		}
		void SetPressedImage(){

			m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Pressed;
			m_eState = E_STATE_PRESSED;
		}

		E_STATE GetState(){ return m_eState; }

		//------------------------------------------------------------------------
		// use for mouse collision
		//------------------------------------------------------------------------
		bool GetPointCollision( DirectX::XMFLOAT2 mouseXY_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		int GetID() const { return m_ID; }

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void OnClickCallback( const DELEGATE_PARAM & param_p );

		private:

		pool_SpriteCompo__ptr m_pSpriteCompoRef;
		DirectX::XMFLOAT4 m_uv_Normal, m_uv_Hover, m_uv_Pressed;
		DirectX::XMFLOAT2 m_borderGap;	// used to make the "collision box" smaller
		E_STATE m_eState;
		int m_ID;

		Delegate1Param m_OnClickCallback;
												
	};

	
	typedef std::shared_ptr<ButtonComponent<>> shared_ButtonComponent_ptr;

	//========================================================================
	// 
	//========================================================================
	template< typename DELEGATE_PARAM = int >
	class ButtonComponentFactory: public AComponentFactory{

	public:

		typedef std::shared_ptr<ButtonComponentFactory> shared_ButtonComponentFactory_ptr;
		typedef std::weak_ptr<ButtonComponentFactory> weak_ButtonComponentFactory_ptr;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ButtonComponentFactory(unsigned int maxComponents_p)
			:
		m_pool(maxComponents_p){}

	private:

		gen::Pool<ButtonComponent<DELEGATE_PARAM>> m_pool;

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent(){

			return pool_Component_ptr(m_pool);

		}
		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * /*pGFig_p*/ ){

			assert(0);
			return pool_Component_ptr(m_pool);
		}

	};
}

#include "ButtonComponent.inl"
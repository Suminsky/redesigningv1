#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/25
	created:	25:5:2013   21:58
	file:		SpriteButtonCompo.h
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

	class SpriteButtonCompo: public Component{
	
	public:
		typedef gen::pool_ptr<SpriteButtonCompo> pool_SpriteButtonCompo_ptr;

		enum E_STATE{

			E_STATE_NORMAL = 0,
			E_STATE_HOVER,
			E_STATE_PRESSED
		};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteButtonCompo(){
			m_type = COMPONENT_TYPE(SpriteButtonCompo);
		}
		~SpriteButtonCompo(){
			m_pSpriteCompoRef.Release();
		}
		SpriteButtonCompo( int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f );
		SpriteButtonCompo( int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, DirectX::XMFLOAT4 uvPressed_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f );
		void Init(int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f );
		void Init(int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p, DirectX::XMFLOAT4 uvHover_p, DirectX::XMFLOAT4 uvPressed_p, float wBorder_p = 0.0f, float hBorder_p = 0.0f );

		void SetHoverUV(){

			m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Hover;
			m_pSpriteCompoRef->m_renderData.m_bUpdate = true;
			m_eState = E_STATE_HOVER;
		}
		void SetNormalUV(){

			m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Normal;
			m_pSpriteCompoRef->m_renderData.m_bUpdate = true;
			m_eState = E_STATE_NORMAL;
		}
		void SetPressedUV(){

			m_pSpriteCompoRef->m_renderData.m_uvRect = m_uv_Pressed;
			m_pSpriteCompoRef->m_renderData.m_bUpdate = true;
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

		private:

		pool_SpriteCompo__ptr m_pSpriteCompoRef;
		DirectX::XMFLOAT4 m_uv_Normal, m_uv_Hover, m_uv_Pressed;
		DirectX::XMFLOAT2 m_borderGap;	// used to make the "collision box" smaller
		E_STATE m_eState;
		int m_ID;												
	};

	
	typedef std::shared_ptr<SpriteButtonCompo> shared_ButtonComponent_ptr;

	//========================================================================
	// 
	//========================================================================
	class SpriteButtonCompoFactory: public AComponentFactory{

	public:

		typedef std::shared_ptr<SpriteButtonCompoFactory> shared_ButtonComponentFactory_ptr;
		typedef std::weak_ptr<SpriteButtonCompoFactory> weak_ButtonComponentFactory_ptr;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteButtonCompoFactory(unsigned int maxComponents_p)
			:
		m_pool(maxComponents_p){}

	private:

		gen::Pool<SpriteButtonCompo> m_pool;

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent(){return pool_Component_ptr(m_pool);}
		virtual pool_Component_ptr VCloneComponent( const Component * /*pCompo_p*/ ){
			assert(0);
			 pool_Component_ptr pButton(m_pool);

			 return pButton;
		}
		virtual void VSerialize( const Component * /*pCompo_p*/, text::GfigElementA * /*pGfig_p*/ ){

			assert(0);
		}
		void VUpdateComponent( Component * /*pCompo_p*/, text::GfigElementA * /*pGFig_p*/ ){
			assert(0);
		}
		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * /*pGFig_p*/ ){

			assert(0);
			return pool_Component_ptr(m_pool);
		}
		virtual void VSerialize( Component * /*pCompo_p*/, text::GfigElementA * /*pGFig_p*/ ){
			assert(0);
		}

	};
}
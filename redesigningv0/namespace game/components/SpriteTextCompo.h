#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2015/06/29
	created:	29:6:2015   20:27
	file:		SpriteTextCompo.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <DirectXMath.h>

// private includes
#include "../../namespace render/dx/Device.h"
#include "../Component.h"
#include "../../namespace gen/Pool.h"
#include "../ComponentFactory.h"
#include "../Event.h"
#include "../../namespace sprite/SpriteRenderer.h"

#include "../../namespace gen/gen_data.h"
#include "../../namespace sprite/SortMask.h"

namespace dx{

	class Device;
}
namespace sprite{

	//class SpriteRenderer;
	class SpriteTextComposer;
}

namespace game{

	struct spriteInstance{

		float res[2];
		float padding[2];
		float uvRect[4];
		float mWorld[16];
		float color[4];
	};

	class SpriteTextCompoFactory;

	class SpriteTextCompo: public Component{

		friend class SpriteTextCompoFactory;
		friend class SpriteTextSys;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteTextCompo();

		void Initialize(
			wchar_t * pSzMemBlock_p, spriteInstance * pInstMemBlock_p,
			uint32_t maxChars_p,
			const wchar_t * szText_p,
			uint32_t fontID_p,
			sprite::SpriteTextComposer * pTextComposer_p,
			sprite::E_BLENDTYPE eBlend_p, sprite::E_SAMPLERTYPE eSampler_p,
			sprite::SpriteRenderer * pSpriteRenderer_p,
			dx::Device * pDevice_p
			);

		//------------------------------------------------------------------------
		// those 2 use the first sprite instance to store the render data,
		// use updaterenderdatatoallinstances for the other
		//------------------------------------------------------------------------
		void InterpolateWorld( double dInterp_p );
		void InterpolateColor( double dInterp );

		//------------------------------------------------------------------------
		// copy first instance data to consecutive instances till m_nCurrentSzLen
		//------------------------------------------------------------------------
		void UpdateInterpolatedRenderDataToAllInstances();

		void SetColor( DirectX::XMFLOAT4 color_p );
		void SetCamera(sprite::Camera * pCamera) { m_pCamera = pCamera; m_sortKey.bitfield.viewportLayer = 200; } // this vp layer is a hack, TODO fix this shit

		void SetSortKey( UINT64 key ){m_sortKey.intRepresentation = key;}
		UINT64 GetSortKey()const{return m_sortKey.intRepresentation;}

		void SetText( const wchar_t * szNewText_p, sprite::SpriteTextComposer * pTextComposer_p );

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnAttach();
		void VOnDetach();
		void OnColorEventDelegate( const Event<ComponentEventData> & event_p );
		void OnTransformEventDelegate( const Event<ComponentEventData> & event_p );

		DirectX::XMFLOAT4X4 m_currentTrafo, m_previousTrafo;
		DirectX::XMFLOAT4 m_currentColor, m_previousColor;

		uint32_t m_fontID;

		wchar_t * m_szText; 
		spriteInstance * m_pRenderData;
		uint32_t m_maxChars, m_nCurrentSzLen, m_nCurremtDrawnChars;

		DirectX::XMFLOAT2 m_textRes; // total res of the width and height of the text
		sprite::SortMask m_sortKey;

		dx::DrawIndexedInstanced m_drawIndexedInstanced;
		dx::PipeState m_pipeState_tex_bs_ss;
		sprite::Camera * m_pCamera;

		bool m_bUpdateBindData;
	};

	typedef gen::pool_ptr<SpriteTextCompo> pool_SpriteTextCompo__ptr;

	//========================================================================
	// 
	//========================================================================

	class SpriteTextCompoFactory: public IComponentFactory{

		friend class SpriteTextSys;

		enum{

			E_MAXTEXTCHARS = 8192,
		};

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteTextCompoFactory(
			unsigned int maxComponents_p,
			sprite::SpriteTextComposer * pTextComposer_p,
			dx::Device * pDevice_p,
			sprite::SpriteRenderer * pSpriteRenderer_p )
			:
		m_pool(maxComponents_p),
		m_pTextComposer(pTextComposer_p),
		m_pSpriteRenderer(pSpriteRenderer_p),
		m_pDeviceRef(pDevice_p)
		{
			InitializeDxVBBuffer(pDevice_p);
			m_instVBBinder.Initialize( 1, m_pDxInstVBBuffer, sizeof(spriteInstance) );
		}

		~SpriteTextCompoFactory();

		static bool SerializeBlendType( const sprite::E_BLENDTYPE eBlend_p, text::GfigElementA & gFig_p );

	private:

		void InitializeDxVBBuffer( dx::Device * pDevice_p );

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		virtual pool_Component_ptr VCreateComponent(){ return pool_Component_ptr(m_pool);}
		virtual pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );
		virtual pool_Component_ptr VCloneComponent( const Component * pCompo_p );
		virtual void VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p );
		virtual void VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p );
	
		gen::Pool<SpriteTextCompo> m_pool;
		dx::Device * m_pDeviceRef;
		sprite::SpriteTextComposer * m_pTextComposer;
		sprite::SpriteRenderer * m_pSpriteRenderer;

		ID3D11Buffer * m_pDxInstVBBuffer;
		dx::BindIAVertexBuffer m_instVBBinder;

		gen::Stack<wchar_t, E_MAXTEXTCHARS> m_szTextStack;
		gen::Stack<spriteInstance, E_MAXTEXTCHARS> m_spriteInstStack;
	};

	typedef std::shared_ptr<SpriteTextCompoFactory> shared_SpriteTextCompoFactory_ptr;
	typedef std::weak_ptr<SpriteTextCompoFactory> weak_SpriteTextCompoFactory_ptr;
}
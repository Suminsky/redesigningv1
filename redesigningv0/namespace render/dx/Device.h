#pragma once

// libraries needed

#pragma comment( lib, "D3D11.lib")

// system/standard headers

#include <D3D11.h>

// private headers

#include "ResourceCache.h"
#include "../DrawablesQueue.h"

namespace dx{

	typedef ResourceCache<RenderTargetView,		1>	RTVCache;
	typedef ResourceCache<Texture2DResource,	60>	Tex2DCache;
	typedef ResourceCache<VertexShader,			3>	VSCache;
	typedef ResourceCache<PixelShader,			3>	PSCache;
	typedef ResourceCache<BufferResource,		7>	BufferCache;
	typedef ResourceCache<BlendState,			2>	BlendStateCache;
	typedef ResourceCache<ShaderResourceView,	60>	ShaderResourceViewCache;
	typedef ResourceCache<InputLayout,			2>	InputLayoutCache;
	typedef ResourceCache<SamplerState,			2>	SamplerStateCache;

	class Device{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Device()
			:
			m_pDevice(NULL), m_pContext(NULL), m_pFactory(NULL),
			m_pCacheRTV(NULL), m_pCacheTex2D(NULL), m_pCacheVS(NULL), m_pCachePS(NULL), m_pCacheBuffer(NULL),
			m_pCacheBlendState(NULL), m_pCacheSRV(NULL), m_pCacheInputLayout(NULL), m_pCacheSamplerState(NULL){

				InitCaches();
		};
		virtual ~Device(){

			if( m_pDevice ) m_pDevice->Release();
			if( m_pContext ) m_pContext->Release();
			if( m_pFactory ) m_pFactory->Release();

			if( m_pCacheRTV )		delete m_pCacheRTV;
			if( m_pCacheTex2D )		delete m_pCacheTex2D;
			if( m_pCacheVS)			delete m_pCacheVS;
			if( m_pCachePS)			delete m_pCachePS;
			if( m_pCacheBuffer )	delete m_pCacheBuffer;
			if( m_pCacheBlendState ) delete m_pCacheBlendState;
			if( m_pCacheSRV )		delete m_pCacheSRV;
			if( m_pCacheInputLayout ) delete m_pCacheInputLayout;
			if( m_pCacheSamplerState ) delete m_pCacheSamplerState;
		};

		ID3D11Device * GetDevice(){ return m_pDevice;}
		ID3D11DeviceContext * GetContext(){ return m_pContext;}

		//------------------------------------------------------------------------
		// Feature Levels array can be NULL, then all the features will be tested,
		// starting by the highest.
		//
		// Creates a device and a device immediate context.
		//
		//------------------------------------------------------------------------
		void InitDevice(
			//in:
			IDXGIAdapter1 *pAdapter_p,
			const BOOL bSingleThreaded_p,
			//in/holded:
			IDXGIFactory1 *pFactory_p, //not used only holded
			const D3D_FEATURE_LEVEL *pFeatureLVLSrequested_p, const UINT nFeatures_p	);

		//------------------------------------------------------------------------
		// Executes a list of render commands
		//------------------------------------------------------------------------
		void ExecutePipelineCommands( const render::RenderCommands & commandList_p );

		RTVCache				* m_pCacheRTV;
		Tex2DCache				* m_pCacheTex2D;
		VSCache					* m_pCacheVS;
		PSCache					* m_pCachePS;
		BufferCache				* m_pCacheBuffer;
		BlendStateCache			* m_pCacheBlendState;
		ShaderResourceViewCache * m_pCacheSRV;
		InputLayoutCache		* m_pCacheInputLayout;
		SamplerStateCache		* m_pCacheSamplerState;

		private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void InitCaches(){

			m_pCacheRTV = new RTVCache(m_pDevice);
			m_pCacheTex2D = new Tex2DCache(m_pDevice);
			m_pCacheVS = new VSCache(m_pDevice);
			m_pCachePS = new PSCache(m_pDevice);
			m_pCacheBuffer = new BufferCache(m_pDevice);
			m_pCacheBlendState = new BlendStateCache(m_pDevice);
			m_pCacheSRV = new ShaderResourceViewCache(m_pDevice);
			m_pCacheInputLayout = new InputLayoutCache(m_pDevice);
			m_pCacheSamplerState = new SamplerStateCache(m_pDevice);
		}

		ID3D11Device * m_pDevice;
		ID3D11DeviceContext * m_pContext;

		IDXGIFactory1 *m_pFactory;	//used for recreating swap chain

		D3D_FEATURE_LEVEL m_featureLVLavailable;
	};
}
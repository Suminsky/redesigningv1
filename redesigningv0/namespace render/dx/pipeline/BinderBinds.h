//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/03/16
	created:	15:2:2013   18:17
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\namespace pipeline\Binder.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\namespace pipeline
	file base:	Binder
	file ext:	h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	This is a command pattern class.

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

#include "../../ConstantBuffer.h"
#include <assert.h>

namespace dx{

//========================================================================
// IA STAGE
//========================================================================
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindIAInputLayout: public Binder{

	ID3D11InputLayout * m_pInputLayout;

public:

	BindIAInputLayout(ID3D11InputLayout * pInputLayout_p)
		:
	Binder( IA_InputLayout, E_IA_InputLayout ),
	m_pInputLayout(pInputLayout_p){}

	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->IASetInputLayout( m_pInputLayout );
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindIAPrimitiveTopology : public Binder{

	D3D11_PRIMITIVE_TOPOLOGY m_topology;

public:

	BindIAPrimitiveTopology( const D3D11_PRIMITIVE_TOPOLOGY & primitiveTopology_p )
		:
	Binder( IA_PrimitiveTopology, E_IA_PrimitiveTopology ),
	m_topology(primitiveTopology_p){}


	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->IASetPrimitiveTopology( m_topology );
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindIAVertexBuffer: public Binder{

	UINT m_iStartSlot;
	ID3D11Buffer * m_pVertexBuffer;
	UINT m_iStride; // size of a element in the buffer ( a "vertex structure" )
	UINT m_iByteOffset; // offset in bytes to the first element in the buffer that is to be used

public:

	BindIAVertexBuffer( const UINT iSlot_p, ID3D11Buffer * pVBuffer_p, const UINT iStride_p, const UINT iOffset_p = 0 )
		:
	Binder( 1LL << (E_IA_VertexBuffer0 + iSlot_p), E_BIND(E_IA_VertexBuffer0 + iSlot_p) ),
		m_iStartSlot(iSlot_p), m_pVertexBuffer(pVBuffer_p), m_iStride(iStride_p), m_iByteOffset(iOffset_p){}

	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->IASetVertexBuffers( m_iStartSlot, 1, &m_pVertexBuffer, &m_iStride, &m_iByteOffset );
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindIAIndexBuffer: public Binder{

	ID3D11Buffer * m_pIndexBuffer;
	DXGI_FORMAT m_format;	// DXGI_FORMAT_R16_UINT or DXGI_FORMAT_R32_UINT
	UINT m_iByteOffset;

public:

	BindIAIndexBuffer( ID3D11Buffer * pIndexBuffer_p, const DXGI_FORMAT format_p = DXGI_FORMAT_R16_UINT, UINT iOffset_p = 0 )
		:
		Binder( IA_IndexBuffer, E_IA_IndexBuffer),
		m_pIndexBuffer(pIndexBuffer_p), m_format(format_p), m_iByteOffset(iOffset_p){}


	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->IASetIndexBuffer( m_pIndexBuffer, m_format, m_iByteOffset );
	}
};
//========================================================================
// VS STAGE
//========================================================================
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindVSVertexShader : public Binder{

	ID3D11VertexShader *m_pVS;

public:

	BindVSVertexShader( ID3D11VertexShader * pVS_p )
		: 
		Binder(VS_VertexShader, E_VertexShader),
		m_pVS(pVS_p){}

	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->VSSetShader( m_pVS, NULL, 0);
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindVSConstantBuffer : public Binder{

	UINT m_iStartSlot;
	ID3D11Buffer *m_pConstantBuffer;
	std::shared_ptr<render::ConstantBuffer> m_pConstantBufferData;

public:

	BindVSConstantBuffer( UINT iSlot_p, ID3D11Buffer *pConstantBuffers_p,  const std::shared_ptr<render::ConstantBuffer> & pConstBufferData_p )
		:
	  Binder( 1LL << (E_VS_CBuffer0 + iSlot_p), E_BIND(E_VS_CBuffer0 + iSlot_p) ),
	  m_iStartSlot( iSlot_p ), m_pConstantBuffer( pConstantBuffers_p ),
	  m_pConstantBufferData(pConstBufferData_p){

		  assert( iSlot_p < 4);	// handling up to 4 cbuffers slot
	  }
	  BindVSConstantBuffer( UINT iSlot_p, ID3D11Buffer *pConstantBuffers_p )
		  :
	  Binder( 1LL << (E_VS_CBuffer0 + iSlot_p), E_BIND(E_VS_CBuffer0 + iSlot_p) ),
		  m_iStartSlot( iSlot_p ), m_pConstantBuffer( pConstantBuffers_p ){

			  assert( iSlot_p < 4);	// handling up to 4 cbuffers slot
	  }

	//------------------------------------------------------------------------
	// Updates data to the buffer before binding it.
	//------------------------------------------------------------------------
	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		m_pConstantBufferData->Update( pDeviceContext_p, m_pConstantBuffer );

		pDeviceContext_p->VSSetConstantBuffers( m_iStartSlot, 1, &m_pConstantBuffer);
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindVSShaderResourceView: public Binder{

	UINT m_iStartSlot;
	ID3D11ShaderResourceView *m_pSRV;

public:

	BindVSShaderResourceView( UINT iSlot_p, ID3D11ShaderResourceView *pSRV_p )
		:
	Binder( 1LL << (E_VS_ShaderResourceView0 + iSlot_p), E_BIND(E_VS_ShaderResourceView0 + iSlot_p) ),
		m_iStartSlot( iSlot_p ), m_pSRV( pSRV_p ){

			assert( iSlot_p < 4);	// handling up to 4 srv slot
	}

	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->VSSetShaderResources( m_iStartSlot, 1, &m_pSRV);
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindVSSampler: public Binder{

	UINT m_iSlot;
	ID3D11SamplerState * m_pSamplerState;

public:

	BindVSSampler( UINT iSlot_p, ID3D11SamplerState * pSamplerState_p )
	:
	Binder(1LL << (E_VS_Sampler0+iSlot_p), E_BIND(E_VS_Sampler0+iSlot_p)),
		m_iSlot(iSlot_p), m_pSamplerState(pSamplerState_p){}

	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->VSSetSamplers( m_iSlot, 1, &m_pSamplerState );
	}
};
//========================================================================
// PS STAGE
//========================================================================
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindPSPixelShader : public Binder{

	ID3D11PixelShader *m_pPS;

public:

	BindPSPixelShader( ID3D11PixelShader * pPS_p ): m_pPS(pPS_p),
	Binder(PS_PixelShader, E_PixelShader){}

	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->PSSetShader( m_pPS, NULL, 0);
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindPSConstantBuffer : public Binder{

	UINT m_iStartSlot;
	ID3D11Buffer *m_pConstantBuffer;
	std::shared_ptr<render::ConstantBuffer> m_pConstantBufferData;

public:

	BindPSConstantBuffer( UINT iSlot_p, ID3D11Buffer *pConstantBuffers_p,  const std::shared_ptr<render::ConstantBuffer> & pConstBufferData_p )
		:
		Binder( 1LL << (E_PS_CBuffer0 + iSlot_p), E_BIND(E_PS_CBuffer0 + iSlot_p) ),
		m_iStartSlot( iSlot_p ), m_pConstantBuffer( pConstantBuffers_p ),
		m_pConstantBufferData(pConstBufferData_p){

			assert( iSlot_p < 4);	// handling up to 4 cbuffers slot
	}

	//------------------------------------------------------------------------
	// Updates data to the buffer before binding it.
	//------------------------------------------------------------------------
	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		m_pConstantBufferData->Update( pDeviceContext_p, m_pConstantBuffer );

		pDeviceContext_p->PSSetConstantBuffers( m_iStartSlot, 1, &m_pConstantBuffer);
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindPSShaderResourceView: public Binder{

	UINT m_iStartSlot;
	ID3D11ShaderResourceView *m_pSRV;

public:

	BindPSShaderResourceView( UINT iSlot_p, ID3D11ShaderResourceView *pSRV_p )
		:
	Binder( 1LL << (E_PS_ShaderResourceView0 + iSlot_p), E_BIND(E_PS_ShaderResourceView0 + iSlot_p) ),
		m_iStartSlot( iSlot_p ), m_pSRV( pSRV_p ){

			assert( iSlot_p < 4);	// handling up to 4 srv slot
	}

	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->PSSetShaderResources( m_iStartSlot, 1, &m_pSRV);
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindPSSampler: public Binder{

	UINT m_iSlot;
	ID3D11SamplerState * m_pSamplerState;

public:

	BindPSSampler( UINT iSlot_p, ID3D11SamplerState * pSamplerState_p )
	:
	Binder(1LL << (E_PS_Sampler0+iSlot_p), E_BIND(E_PS_Sampler0+iSlot_p)),
	m_iSlot(iSlot_p), m_pSamplerState(pSamplerState_p){}

	BindPSSampler(){}

	void Init( UINT iSlot_p, ID3D11SamplerState * pSamplerState_p ){

		//Binder::m_typeBits = 1LL << (E_PS_Sampler0+iSlot_p);
		//Binder::m_typeIndex = E_BIND(E_PS_Sampler0+iSlot_p);
		Set( 1LL << (E_PS_Sampler0+iSlot_p), E_BIND(E_PS_Sampler0+iSlot_p));

		m_iSlot = iSlot_p;
		m_pSamplerState = pSamplerState_p;
	}

	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->PSSetSamplers( m_iSlot, 1, &m_pSamplerState );
	}
};
//========================================================================
// RS STAGE
//========================================================================
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindRSViewPort: public Binder{

public:

	//------------------------------------------------------------------------
	// ctor
	//------------------------------------------------------------------------
	BindRSViewPort( D3D11_VIEWPORT & vp_p )
		:
	Binder( RS_ViewPort, E_RS_ViewPort ),
	m_viewPort(vp_p)
	{}


	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->RSSetViewports(1, &m_viewPort);
	}

private:

	D3D11_VIEWPORT & m_viewPort;

	BindRSViewPort & operator = ( const BindRSViewPort & other_p );
};
//========================================================================
// OM STAGE
//========================================================================
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindOMBlendState : public Binder{
	
	ID3D11BlendState * pBlendState;
	float * m_pBlendFactor;	//RGBA, used by blends that specify "blend factor"
	UINT m_sampleMask;	// witch samples gets updated...I dont get it

public:

	BindOMBlendState( ID3D11BlendState * pBlendState_p, float * blendFactor_p, UINT sampleMask_p = 0xffffffff )
		:
	Binder(OM_BlendState, E_OM_BlendState),
	pBlendState(pBlendState_p), m_pBlendFactor(blendFactor_p), m_sampleMask(sampleMask_p){}

	BindOMBlendState()
		: Binder(OM_BlendState, E_OM_BlendState){}

	void Init(  ID3D11BlendState * pBlendState_p, float * blendFactor_p, UINT sampleMask_p = 0xffffffff ){

		pBlendState = pBlendState_p;
		m_pBlendFactor = blendFactor_p;
		m_sampleMask = sampleMask_p;
	}
	

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->OMSetBlendState( pBlendState, m_pBlendFactor, m_sampleMask );
	}
};
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
class BindOMRenderTargetDepthStencil: public Binder{

public:

	//------------------------------------------------------------------------
	// ctors
	//------------------------------------------------------------------------
	BindOMRenderTargetDepthStencil( ID3D11RenderTargetView * pRTV_p, ID3D11DepthStencilView * pDSV_p )
		:
	Binder( OM_RenderTargetDepthStencil, E_OM_RenderTargetDepthStencil ),
	m_pRTV(pRTV_p), m_pDSV(pDSV_p)
	{}

	BindOMRenderTargetDepthStencil()
		:
	Binder( OM_RenderTargetDepthStencil, E_OM_RenderTargetDepthStencil )
	{}

	//------------------------------------------------------------------------
	// delayed construction
	//------------------------------------------------------------------------
	void Initialize( ID3D11RenderTargetView * pRTV_p, ID3D11DepthStencilView * pDSV_p ){

		m_pRTV = pRTV_p;
		m_pDSV = pDSV_p;
	}

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

		pDeviceContext_p->OMSetRenderTargets( 1, &m_pRTV, m_pDSV );
	}

private:

	ID3D11RenderTargetView * m_pRTV;
	ID3D11DepthStencilView * m_pDSV;
};
}
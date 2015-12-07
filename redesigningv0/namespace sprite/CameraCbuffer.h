//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/03/31
	created:	31:3:2013   21:03
	filename: 	c:\Users\Gateway\Documents\Visual Studio 2010\Projects\redesigningv1\redesigningv0\Sprite.h
	file path:	c:\Users\Gateway\Documents\Visual Studio 2010\Projects\redesigningv1\redesigningv0
	file base:	Sprite
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

// system/default headers

#include <D3D11.h>
#include <DirectXMath.h>
//#include <boost/shared_ptr.hpp>

// private headers

#include "../namespace render/dx/pipeline/Binder.h"

namespace sprite{

	//========================================================================
	// Specialized layout cbuffer
	//camera data
								//n		//bytes	//align
	//matrix mViewProjection;	//16	64		0-15
	//========================================================================
	struct CameraCbuffer{
		
		static const UINT s_SIZE = 64;
		DirectX::XMMATRIX m_mViewProjection;

		bool m_bUpdate;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		CameraCbuffer():m_mViewProjection(DirectX::XMMatrixIdentity()), m_bUpdate(true){}

		//------------------------------------------------------------------------
		// Test if buffer data was modified since last update call, than updates it,
		// or not.
		//------------------------------------------------------------------------
		void UpdateIfDirt( ID3D11DeviceContext * pContext_p, ID3D11Buffer *& pConstantBuffer_p ){

			if( m_bUpdate ){

				D3D11_MAPPED_SUBRESOURCE mapped;
				pContext_p->Map( pConstantBuffer_p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
				memcpy( mapped.pData, this, s_SIZE );
				pContext_p->Unmap( pConstantBuffer_p, 0 );
				
				//pContext_p->UpdateSubresource( pConstantBuffer_p, 0, NULL, this, 0, 0 );
				m_bUpdate = false;
			}
		}
	};

	typedef std::shared_ptr<CameraCbuffer> shared_CameraCbuffer_ptr;

	//========================================================================
	// bind command specialized for drawable cbuffer
	//========================================================================
	class BindVSCameraCBuffer: public dx::Binder{

	public:

		//------------------------------------------------------------------------
		// ctors
		//------------------------------------------------------------------------
		BindVSCameraCBuffer( ID3D11Buffer *pConstantBuffers_p, CameraCbuffer * pConstBufferData_p )
			:
		Binder( 1LL << dx::E_VS_CBuffer1 , dx::E_VS_CBuffer1 ),
		m_iStartSlot( 1 ),
		m_pConstantBuffer( pConstantBuffers_p ),
		m_pConstantBufferData(pConstBufferData_p)
		{}
		BindVSCameraCBuffer()
			:
		Binder( 1LL << dx::E_VS_CBuffer1 , dx::E_VS_CBuffer1 ),
		m_iStartSlot( 1 )
		{}

		//------------------------------------------------------------------------
		// delayed construction
		//------------------------------------------------------------------------
		void Initialize( ID3D11Buffer *pConstantBuffers_p, CameraCbuffer * pConstBufferData_p ){

			m_pConstantBuffer = pConstantBuffers_p;
			m_pConstantBufferData = pConstBufferData_p;
		}

		//------------------------------------------------------------------------
		// Updates data to the buffer before binding it.
		//------------------------------------------------------------------------
		virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

			if( m_pConstantBuffer != s_pConstantBufferBound ){

				m_pConstantBufferData->UpdateIfDirt( pDeviceContext_p, m_pConstantBuffer );

				pDeviceContext_p->VSSetConstantBuffers( m_iStartSlot, 1, &m_pConstantBuffer);

				// cache:
				s_pConstantBufferBound = m_pConstantBuffer;
			}
			else{

				// force update

				D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
				pDeviceContext_p->Map( m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
				memcpy( mapped.pData, m_pConstantBufferData, m_pConstantBufferData->s_SIZE );
				pDeviceContext_p->Unmap( m_pConstantBuffer, 0 );

				m_pConstantBufferData->m_bUpdate = false;

				//pDeviceContext_p->UpdateSubresource( m_pConstantBuffer, 0, NULL, m_pConstantBufferData.get(), 0, 0 );
			}
		}

	private:

		UINT			m_iStartSlot;
		ID3D11Buffer *	m_pConstantBuffer;
		CameraCbuffer * m_pConstantBufferData;

		static ID3D11Buffer * s_pConstantBufferBound;
	};
}
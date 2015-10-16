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
#include "../namespace gen/gen_macros.h"
#include "../namespace render/dx/pipeline/Binder.h"

namespace sprite{

	//========================================================================
	// On HLSL, data is aligned by "float4", each data that doesnt fit into
	// a float4 jumps to the next one
	//========================================================================
#pragma warning(push)
#pragma warning(disable:4324) // padding due forced align

	struct DrawableCbuffer{
		
		static const UINT s_SIZE = 112;

		DirectX::XMFLOAT2 m_res;
		DirectX::XMFLOAT2 m_padding;
		DirectX::XMFLOAT4 m_uvRect;
		DirectX::XMMATRIX m_mWorld;	// a.k.a render mWorld
		DirectX::XMFLOAT4 m_color;
		

		bool m_bUpdate;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		DrawableCbuffer()
			:
		m_bUpdate(true)
		{

			m_padding.x = m_padding.y = 0.0f;
		}

		DrawableCbuffer( float width_p, float height_p, float uvOffsetU_p, float uvOffsetV_p, DirectX::CXMMATRIX mWorld_p );

		//------------------------------------------------------------------------
		// Test if buffer data was modified since last update call, than updates it,
		// or not.
		//------------------------------------------------------------------------
		inline void UpdateIfDirt( ID3D11DeviceContext * pContext_p, ID3D11Buffer *& pConstantBuffer_p ){
			
			// need 2 flags, a dirt per object (did I changed since last time), and a dirt for the
			// buffer (the shared buffer), (its my data on the buffer?)
			
			if( m_bUpdate ){
	
				D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
				pContext_p->Map( pConstantBuffer_p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
				memcpy( mapped.pData, this, s_SIZE );
				pContext_p->Unmap( pConstantBuffer_p, 0 );

				//pContext_p->UpdateSubresource( pConstantBuffer_p, 0, NULL, this, 0, 0 );
				m_bUpdate = false;
			}
		}

		//ALLIGN16ONLY;
	};

#pragma warning(pop)

	typedef std::shared_ptr<DrawableCbuffer> shared_DrawableCbuffer_ptr;


	//========================================================================
	// bind command specialized for drawable cbuffer
	//========================================================================
	class BindVSDrawableCBuffer: public dx::Binder{

	public:

		//------------------------------------------------------------------------
		// ctors
		//------------------------------------------------------------------------
		BindVSDrawableCBuffer( ID3D11Buffer *pConstantBuffers_p,  DrawableCbuffer * pConstBufferData_p )
			:
		Binder( 1LL << dx::E_VS_CBuffer0 , dx::E_VS_CBuffer0 ),
		m_iStartSlot( 0 ),
		m_pConstantBuffer( pConstantBuffers_p ),
		m_pConstantBufferData(pConstBufferData_p)
		{}

		BindVSDrawableCBuffer()
			:
		Binder( 1LL << dx::E_VS_CBuffer0 , dx::E_VS_CBuffer0 ),
		m_iStartSlot( 0 )
		{}

		//------------------------------------------------------------------------
		// delayed construction
		//------------------------------------------------------------------------
		void Initialize( ID3D11Buffer *pConstantBuffers_p,  DrawableCbuffer * pConstBufferData_p ){

			m_pConstantBuffer = pConstantBuffers_p;
			m_pConstantBufferData = pConstBufferData_p;
		}

		//------------------------------------------------------------------------
		// Updates data to the buffer before binding it.
		// 
		// NOTE: if another binder command class binds the same buffer interface,
		// this binder can consider the buffer unbound and rebind it.
		// 
		// If another binder bind a different one, this class may not bind, just
		// update. Thats why its REQUIRED to have only one binder class for each
		// slot.
		//------------------------------------------------------------------------
		virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

			if( m_pConstantBuffer != s_pConstantBufferBound ){

				// update if data is dirt - dirt on the object
					
				m_pConstantBufferData->UpdateIfDirt( pDeviceContext_p, m_pConstantBuffer );

				// bind buffer

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

				//pDeviceContext_p->UpdateSubresource( m_pConstantBuffer, 0, NULL, m_pConstantBufferData, 0, 0 );
			}
		}

	private:

		UINT m_iStartSlot;

		ID3D11Buffer * m_pConstantBuffer;
		DrawableCbuffer * m_pConstantBufferData;

		static ID3D11Buffer *s_pConstantBufferBound;	// test: using to check if this buffer need to be bound
	};


	typedef std::shared_ptr<BindVSDrawableCBuffer> shared_BindVSDrawableCBuffer_ptr;
}
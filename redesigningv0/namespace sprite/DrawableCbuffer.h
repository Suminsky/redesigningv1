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
		

		bool m_bUpdate;	// set to true to update the ID3D11Buffer
						// IMPORTANT: this NEED to be placed after the above data, because this class itself is the one
						// send to the buffer...theres no bool on that buffer, so the s_SIZE excludes it, if placed after
						// 
						// EXPLANATION:
						// the whole point of that update is delegate the buffer update to the point where the bind cmd is
						// executed, that way I can have objects share the same ID3D11Buffer, witch is awesome. If that where
						// to be update in the game loop, the buffer would contain only the last objects data, and then Id be 
						// obligated to use multiple buffers.
						// ERRATA:
						// if I have multiple objects sharing a ID3D11Buffer, than I NEED to update, even if the data didnt changed,
						// because the data is always remaining from the last object, so setting update to false is like saying to
						// use data from the last object, which is stupid..
						// Updating always have the counter side of if an object have a unique ID3D11Buffer, it will update needlessly.
						// Well, I`m already checking if the current bound ID3D11Buffer is the one to be bound, I can just use that and:
						// -if its the same buffer, update regardless of this bool;
						// -if its other buffer, update in accord to the bool; - WRONG (see errata 2)
						// ERRATA 2:
						// if its other buffer, it may set the new buffer without the renderable data, but with another one
						// in it. Say current buffer bound is different, and current object buffer is shared among others.
						// If object didnt change its data, the update flag will be false, but the current data in the buffer
						// can be of any object! - solving this with another static for the data
						// 
						// 
						// IMPORTANT 2:
						// delegating to the bind cmd means that it will only be updated if the buffer/binder command is not
						// already settled, otherwise the cache will skip it. Reseting the cache is not a good solution,
						// because the buffer will be re settled just to be updated..the point of having the buffer shared
						// becomes meaningless.

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
		void UpdateIfDirt( ID3D11DeviceContext * pContext_p, ID3D11Buffer *& pConstantBuffer_p ){
			
			// need 2 flags, a dirt per object (did I changed since last time), and a dirt for the
			// buffer (the shared buffer), (its my data on the buffer?)
			
			// TODO: first fullscreen switch doesnt improve performance!!! second and consecutives rise 1000 FPS
							//havy text // normal // normal FS
			if( m_bUpdate ){//1336 //2350 //3090	// same for map and updatesubresource
	
				D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
				pContext_p->Map( pConstantBuffer_p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
				memcpy( mapped.pData, this, s_SIZE );
				pContext_p->Unmap( pConstantBuffer_p, 0 );

				//pContext_p->UpdateSubresource( pConstantBuffer_p, 0, NULL, this, 0, 0 );
				m_bUpdate = false;
			}
		}
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
		m_pConstantBuffer( pConstantBuffers_p ), m_pConstantBufferData(pConstBufferData_p)
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

				// update if data is dirt - dirt on the buffer

				if( s_pConstantBufferDataBound != m_pConstantBufferData ){
				
					D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
					pDeviceContext_p->Map( m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
					memcpy( mapped.pData, m_pConstantBufferData, m_pConstantBufferData->s_SIZE );
					pDeviceContext_p->Unmap( m_pConstantBuffer, 0 );

					//pDeviceContext_p->UpdateSubresource( m_pConstantBuffer, 0, NULL, m_pConstantBufferData, 0, 0 );

					s_pConstantBufferDataBound = m_pConstantBufferData;
				}
				else{

					// update if data is dirt - dirt on the object
					
					m_pConstantBufferData->UpdateIfDirt( pDeviceContext_p, m_pConstantBuffer );
				}

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

				//pDeviceContext_p->UpdateSubresource( m_pConstantBuffer, 0, NULL, m_pConstantBufferData, 0, 0 );
			}
		}

	private:

		UINT m_iStartSlot;

		ID3D11Buffer * m_pConstantBuffer;
		DrawableCbuffer * m_pConstantBufferData;

		static ID3D11Buffer *s_pConstantBufferBound;	// test: using to check if this buffer need to be bound
		static DrawableCbuffer *s_pConstantBufferDataBound;
	};


	typedef std::shared_ptr<BindVSDrawableCBuffer> shared_BindVSDrawableCBuffer_ptr;
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/12
	created:	12:2:2013   17:12
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\type wrappers\InputLayout.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\type wrappers
	file base:	InputLayout
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	DX Resource wrapper.
				***WARNING***
				->In conformance with ResourceCache template T.-> NOT, I think a buffer resource cant really be cached, 
				unless it is immutable, but in that case, no way Im holding all the data for comparisom..
				So I think buffer resources are never cached...

				For sure a shader should share its CB that changes every time, but if the object holding the buffer
				doesnt know its shared, it might get unknown data on "its" buffer..this should be handled by a more
				hig lvl class.

				I think the resourceCache must have a specialization for BufferResource..
				TODO
				or maybe just give a szName to the buffer..why not

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// libraries needed

#pragma comment( lib, "D3D11.lib")
#pragma comment( lib, "D3DX11.lib")

// system/standard headers
#include <string>
#include <D3Dcommon.h>
#include <D3D11.h>
#include <D3DX11core.h>	//including because of D3DERR_INVALIDCALL

#include <exception>
//#include <boost/smart_ptr.hpp>

namespace dx{

	template< class T, int fixedSize > class ResourceCache;


	//========================================================================
	// VBs, IBs, CBs..
	//========================================================================
	class BufferResource{

		typedef ID3D11Buffer res;
		typedef ID3D11Buffer* pRes;

		template< class T, int fixedSize > friend class ResourceCache;

		BufferResource();

	public:

		//------------------------------------------------------------------------
		// buffer description only
		//------------------------------------------------------------------------
		struct Description{

			//std::string szName;
			D3D11_BUFFER_DESC bufferDesc;

			//------------------------------------------------------------------------
			// compares
			//------------------------------------------------------------------------
			bool operator == ( const Description & anotherDesc_p ){

				//if( !szName.empty() )
				//	return szName == anotherDesc_p.szName;
				//else

				return (
							bufferDesc.BindFlags == anotherDesc_p.bufferDesc.BindFlags
						&& bufferDesc.ByteWidth == anotherDesc_p.bufferDesc.ByteWidth
						&& bufferDesc.CPUAccessFlags == anotherDesc_p.bufferDesc.CPUAccessFlags
						&& bufferDesc.MiscFlags == anotherDesc_p.bufferDesc.MiscFlags
						&& bufferDesc.StructureByteStride == anotherDesc_p.bufferDesc.StructureByteStride
						&& bufferDesc.Usage == anotherDesc_p.bufferDesc.Usage
						);
			}
		};


		//------------------------------------------------------------------------
		// Creates...
		//------------------------------------------------------------------------
		struct CreationParams{

			D3D11_SUBRESOURCE_DATA * pInitialData;

			Description desc;
		};

		//------------------------------------------------------------------------
		// Creates...
		//------------------------------------------------------------------------
		static ID3D11Buffer* Create( ID3D11Device * pDeviceRef_p, const CreationParams & params_p ){

			ID3D11Buffer * pBuffer;

			if( FAILED(
				pDeviceRef_p->CreateBuffer( &params_p.desc.bufferDesc, params_p.pInitialData, &pBuffer)
				)){

					throw std::exception("render:dx create buffer resource failed.");
			}

			return pBuffer;
		}


		virtual ~BufferResource();
	};
}
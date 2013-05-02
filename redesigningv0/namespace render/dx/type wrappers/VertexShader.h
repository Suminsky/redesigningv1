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
				In conformance with ResourceCache template T.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// libraries needed

#pragma comment( lib, "D3D11.lib")
#pragma comment( lib, "D3DX11.lib")

// system/standard headers

#include <D3Dcommon.h>
#include <D3D11.h>
#include <D3DX11core.h>	//including because of D3DERR_INVALIDCALL
#include <string>

//#include <boost/smart_ptr.hpp>

namespace dx{

	template< class T, int fixedSize > class ResourceCache;


	class VertexShader{

		typedef ID3D11VertexShader res;
		typedef ID3D11VertexShader* pRes;

		template< class T, int fixedSize > friend class ResourceCache;

		VertexShader();

	public:

		//------------------------------------------------------------------------
		// Holds description for the type and comparison method
		//------------------------------------------------------------------------
		struct Description{

			UINT64 iPermutation;		// test
			std::string	szHLSL;			// filename
			std::string szProgramName;			// vertex shader name
			

			//------------------------------------------------------------------------
			// compare
			//------------------------------------------------------------------------
			bool operator == ( const Description & anotherDesc_p ){

				if(		iPermutation != anotherDesc_p.iPermutation
					||	szHLSL != anotherDesc_p.szHLSL
					||	szProgramName != anotherDesc_p.szProgramName	) return false;

				return true;
			}
		};

		//------------------------------------------------------------------------
		// Holds params for creation
		//------------------------------------------------------------------------
		struct CreationParams{

			ID3DBlob* pShaderBytes;		// compiled shader

			Description desc;			
		};

		//------------------------------------------------------------------------
		// Creates...
		//------------------------------------------------------------------------
		static ID3D11VertexShader* Create( ID3D11Device * pDeviceRef_p, const CreationParams & params_p ){
			
			ID3D11VertexShader *pVertexShader;		

			if( FAILED(
				pDeviceRef_p->CreateVertexShader( params_p.pShaderBytes->GetBufferPointer(), params_p.pShaderBytes->GetBufferSize(), NULL, &pVertexShader )
				)){

					//params_p.pShaderBytes->Release();
					throw std::exception("render:dx create VS failed");
			}

			//params_p.pShaderBytes->Release();

			return pVertexShader;
		}

		virtual ~VertexShader();
	};
}
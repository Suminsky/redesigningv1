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

				TODO: create createfromfile methods

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


	class Texture2DResource{

		typedef ID3D11Texture2D res;
		typedef ID3D11Texture2D* pRes;

		template< class T, int fixedSize > friend class ResourceCache;

		Texture2DResource();

	public:

		//------------------------------------------------------------------------
		// Holds description for the type and comparison method
		//------------------------------------------------------------------------
		struct Description{

			std::string	szFilename;			// filename (if null, ignore)
			D3D11_TEXTURE2D_DESC tex2Ddesc;			

			//------------------------------------------------------------------------
			// compare file name with precedence.
			// if theres no filename(null) than the description is compared.
			//------------------------------------------------------------------------
			bool operator == ( const Description & anotherDesc_p ){

				if( !anotherDesc_p.szFilename.empty() ){

					if( szFilename == anotherDesc_p.szFilename ) return true;

					return false;
				}
				else{

					if( memcmp( &tex2Ddesc, &anotherDesc_p.tex2Ddesc, sizeof(D3D11_TEXTURE2D_DESC)) == 0 )
						return true;

					return false;
				}
			}
		};


		//------------------------------------------------------------------------
		// Holds params for creation
		//------------------------------------------------------------------------
		struct CreationParams{

			D3D11_SUBRESOURCE_DATA * pInitialData;
			
			Description desc;			
		};

		//------------------------------------------------------------------------
		// Creates...
		//------------------------------------------------------------------------
		static ID3D11Texture2D* Create( ID3D11Device * pDeviceRef_p, const CreationParams & params_p ){
			
			ID3D11Texture2D *pTex2D;		

			if( params_p.pInitialData ){
				if( FAILED(
					pDeviceRef_p->CreateTexture2D( &params_p.desc.tex2Ddesc, params_p.pInitialData, &pTex2D )
					)){
					
						throw std::exception("render:dx create tex2D failed");
				}
			}
			else{
				if( FAILED(
				D3DX11CreateTextureFromFileA( pDeviceRef_p, params_p.desc.szFilename.c_str(), NULL, NULL, (ID3D11Resource**)&pTex2D, NULL )
				)){

					throw std::exception("render:dx create tex2D failed");
				}
			}

			return pTex2D;
		}

		virtual ~Texture2DResource();
	};
}
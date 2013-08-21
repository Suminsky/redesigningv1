//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/12
	created:	12:2:2013   17:12
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
#include <assert.h>

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
					// TODO: compare desc
					return false;
				}
				else{
					assert(0); // use a name even if not from file

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
			else
			{
				/*D3DX11_IMAGE_INFO imgInfo;
				D3DX11_IMAGE_LOAD_INFO loadInfo;

				D3DX11GetImageInfoFromFileA(params_p.desc.szFilename.c_str(), NULL, &imgInfo, nullptr);
				loadInfo.Width = imgInfo.Width;
				loadInfo.Height = imgInfo.Height;
				loadInfo.Depth = imgInfo.Depth;
				loadInfo.FirstMipLevel = 0;
				loadInfo.MipLevels = 1;
				loadInfo.Usage = D3D11_USAGE_DEFAULT;
				loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE ;
				loadInfo.CpuAccessFlags = 0;
				loadInfo.MiscFlags = 0;
				loadInfo.Format = imgInfo.Format;
				loadInfo.Filter = D3DX11_FILTER_NONE;
				loadInfo.MipFilter = D3DX11_FILTER_NONE;
				loadInfo.pSrcInfo = &imgInfo;*/

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
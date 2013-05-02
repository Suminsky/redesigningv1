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

	class BlendState{

		typedef ID3D11BlendState res;
		typedef ID3D11BlendState* pRes;

		template< class T, int fixedSize > friend class ResourceCache;

		BlendState();

	public:

		//------------------------------------------------------------------------
		// Holds description for the type and comparison method
		//------------------------------------------------------------------------
		struct Description{

			D3D11_BLEND_DESC blendDesc;

			//------------------------------------------------------------------------
			// compare 
			//------------------------------------------------------------------------
			bool operator == ( const Description & anotherDesc_p ){

				if( memcmp(&blendDesc, &anotherDesc_p.blendDesc, sizeof(D3D11_BLEND_DESC)) == 0 ) return true;

				return false;
			}
		};


		//------------------------------------------------------------------------
		// Holds params for creation
		//------------------------------------------------------------------------
		struct CreationParams{

			Description desc;			
		};

		//------------------------------------------------------------------------
		// Creates...
		//------------------------------------------------------------------------
		static ID3D11BlendState* Create( ID3D11Device * pDeviceRef_p, const CreationParams & params_p ){
			
			ID3D11BlendState *pBlendState;		

			if( FAILED(
				pDeviceRef_p->CreateBlendState( &params_p.desc.blendDesc, &pBlendState )
				)){
					
					throw std::exception("render:dx create blendstate failed");
			}

			return pBlendState;
		}

		virtual ~BlendState(){};
	};
}
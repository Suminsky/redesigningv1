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

#include <boost/smart_ptr.hpp>

namespace dx{

	template< class T, int fixedSize > class ResourceCache;

	//========================================================================
	// "Create an input-layout object to describe the input-buffer data for the input-assembler stage."
	// Requires a compiled shader bytes with signature.
	//========================================================================
	class InputLayout{

		typedef ID3D11InputLayout res;
		typedef ID3D11InputLayout* pRes;

		template< class T, int fixedSize > friend class ResourceCache;

	public:

		InputLayout(){};

		//------------------------------------------------------------------------
		// Holds description for the type and comparison method
		//------------------------------------------------------------------------
		struct Description{


			boost::shared_array<D3D11_INPUT_ELEMENT_DESC> pInputElementsDescs;
			UINT nElements;

			//------------------------------------------------------------------------
			// Compare the input elements
			//------------------------------------------------------------------------
			bool operator == ( const Description & anotherDesc_p ){

				if( nElements != anotherDesc_p.nElements) return false;

				for( UINT it = 0; it < nElements; it ++ ){

					if(		pInputElementsDescs[it].Format != anotherDesc_p.pInputElementsDescs[it].Format			// byte format
						||	pInputElementsDescs[it].InputSlot != anotherDesc_p.pInputElementsDescs[it].InputSlot	// 0-15
						||	pInputElementsDescs[it].InputSlotClass != anotherDesc_p.pInputElementsDescs[it].InputSlotClass	// per vertex? per instance?	
						||	pInputElementsDescs[it].InstanceDataStepRate != anotherDesc_p.pInputElementsDescs[it].InstanceDataStepRate	// per instance then, how many per buffer element?
						||	pInputElementsDescs[it].AlignedByteOffset != anotherDesc_p.pInputElementsDescs[it].AlignedByteOffset // opt offset between elements, gen D3D11_APPEND_ALIGNED_ELEMENT 
						||	pInputElementsDescs[it].SemanticIndex  != anotherDesc_p.pInputElementsDescs[it].SemanticIndex	// semantic name modifier
						)
						return false;
				}

				return true;
			}
		};

		//------------------------------------------------------------------------
		// Holds params for creation 
		//------------------------------------------------------------------------
		struct CreationParams{

			void* pShaderSig;	//do not care about this dude memory (its for creation only, not used anymore)
			SIZE_T iSizeShaderSig;

			Description desc;
		};

		//------------------------------------------------------------------------
		// Creates...
		//------------------------------------------------------------------------
		static ID3D11InputLayout* Create( ID3D11Device * pDeviceRef_p, const CreationParams & params_p ){

			ID3D11InputLayout *pInputLayout;

			if(FAILED(
				pDeviceRef_p->CreateInputLayout(	params_p.desc.pInputElementsDescs.get(),
													params_p.desc.nElements,
													params_p.pShaderSig,
													params_p.iSizeShaderSig,
													&pInputLayout )
				)){

					throw std::exception("render:dx create input layout failed");
			}

			return pInputLayout;
		}

		virtual ~InputLayout(){};
	};
}
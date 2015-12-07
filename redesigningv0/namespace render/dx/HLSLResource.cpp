#include "HLSLResource.h"

#include <D3DX11.h>
#include <D3Dcompiler.h>

#include "../../namespace gen/gen_macros.h"
#include "../../namespace win/win_macros.h"

using namespace dx;


void dx::ShaderResource::LoadShaderProgram( dx::E_BIND type_p, dx::Device * pDevice_p, const char * szXS_p, const char* szProfile_p, bool bRowMajor_p /*= false*/, ID3DBlob ** ppShaderBytes_p /*= NULL*/, UINT64 iPermutationMask_p /*= 0*/, UINT iPermutationIndex_p /*= 0*/, const D3D10_SHADER_MACRO * pDefines_p /*= NULL */ )
{
	assert( iPermutationIndex_p < m_nPermutations );

	// compile shader

	UINT iCompileFlags = DBG(D3DCOMPILE_DEBUG)NDBG(D3DCOMPILE_OPTIMIZATION_LEVEL3)|(bRowMajor_p?D3DCOMPILE_PACK_MATRIX_ROW_MAJOR:0);
	ID3DBlob * pErrorMsg = NULL;
	ID3DBlob * pShaderBytes = NULL;

	if( FAILED( D3DX11CompileFromFileA( m_szName.c_str(), pDefines_p, NULL, szXS_p, szProfile_p,
		iCompileFlags, 0, NULL,
		&pShaderBytes, &pErrorMsg, NULL)) )
	{

		_MBA("DX Sprite Problem", pErrorMsg->GetBufferPointer(), MB_E );
		pErrorMsg->Release();

		throw std::exception("failed to compile shader");
	}

	// create shader

	switch( type_p ){

	case E_VertexShader:{

		VertexShader::CreationParams xsDesc;
		xsDesc.pShaderBytes = pShaderBytes;
		xsDesc.desc.iPermutation = iPermutationMask_p;
		xsDesc.desc.szHLSL = m_szName;
		xsDesc.desc.szProgramName = szXS_p;

		// set permutation

		m_permutations[iPermutationIndex_p].m_optionsBitMask = iPermutationMask_p;
		m_permutations[iPermutationIndex_p].m_pipeState.AddBinderCommand( &m_VSCache.GetFromID( pDevice_p->m_pCacheVS->Acquire(xsDesc) ) );

						}break;
	case E_PixelShader:{

		PixelShader::CreationParams xsDesc;
		xsDesc.pShaderBytes = pShaderBytes;
		xsDesc.desc.iPermutation = iPermutationMask_p;
		xsDesc.desc.szHLSL = m_szName;
		xsDesc.desc.szProgramName = szXS_p;

		// set permutation

		m_permutations[iPermutationIndex_p].m_optionsBitMask = iPermutationMask_p;
		m_permutations[iPermutationIndex_p].m_pipeState.AddBinderCommand( &m_PSCache.GetFromID( pDevice_p->m_pCachePS->Acquire(xsDesc) ) );

					   }break;
	}

	if( pErrorMsg )
		pErrorMsg->Release();

	if( ppShaderBytes_p ){

		(*ppShaderBytes_p) = pShaderBytes;
	}
	else{
		pShaderBytes->Release();
	}
}

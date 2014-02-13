//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/16
	created:	16:2:2013   11:45
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\Shader.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx
	file base:	Shader
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	Loads and compiles shader programs, storing all of its permutations.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

// libraries needed

#pragma comment( lib, "D3D11.lib")
#pragma comment( lib, "D3Dcompiler.lib")

#include <INITGUID.H>	// solves IID_ID3D11ShaderReflection unresolved lnk2001
						// http://support.microsoft.com/kb/130869?wa=wsignin1.0 for details
// system/standard headers

#include <D3D11.h>
#include <string>

// private includes

#include "Device.h"
#include "pipeline/Binder.h"
#include "BinderCache.h"

namespace dx{

	class Device;

	//------------------------------------------------------------------------
	// A shader permutation is a hlsl file compiled with a set of defines.
	// It holds a collection of binds, wich can include different shader programs
	// bindings( vs, ps..), different cbuffers binding, resources...
	//------------------------------------------------------------------------
	struct ShaderPermutation{

		dx::PipeState m_pipeState;	// collection of Bind shader PROGRAMS ONLY
		UINT64 m_optionsBitMask;		// flags
	};

	//========================================================================
	// Holds all shader programs from a HLSL file, together with all of its
	// permutations.
	//========================================================================
	struct ShaderResource{

		std::string m_szName;
		std::unique_ptr<ShaderPermutation[]> m_permutations;
		UINT m_nPermutations;

		BinderCache< BindVSVertexShader, VSCache > m_VSCache;
		BinderCache< BindPSPixelShader, PSCache > m_PSCache;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ShaderResource( const char* szHLSL_p, UINT nPermutations_p, UINT nShaderPrograms_p, dx::Device * pDevice_p )
			:
			m_permutations( new ShaderPermutation[nPermutations_p]()),
			m_nPermutations(nPermutations_p),
			m_szName(szHLSL_p)
			{
				m_VSCache.Init( nShaderPrograms_p, pDevice_p->m_pCacheVS );
				m_PSCache.Init( nShaderPrograms_p, pDevice_p->m_pCachePS );
			}
		ShaderResource(){}
		virtual ~ShaderResource(){};

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Init( const char* szHLSL_p, UINT nPermutations_p, UINT nShaderPrograms_p, dx::Device * pDevice_p ){

			m_permutations.reset(new ShaderPermutation[nPermutations_p]);
			m_nPermutations = nPermutations_p;
			m_szName = szHLSL_p;

			m_VSCache.Init( nShaderPrograms_p, pDevice_p->m_pCacheVS );
			m_PSCache.Init( nShaderPrograms_p, pDevice_p->m_pCachePS );
		}

		//------------------------------------------------------------------------
		// Add to the permutation (given) state the bind commands to bind the
		// shader programs.
		//------------------------------------------------------------------------
		void LoadShaderProgram(	dx::E_BIND type_p, dx::Device * pDevice_p,
			const char * szXS_p, const char* szProfile_p,
			bool bRowMajor_p = false,
			ID3DBlob ** ppShaderBytes_p = NULL,
			UINT64 iPermutationMask_p = 0, UINT iPermutationIndex_p = 0,
			const D3D10_SHADER_MACRO * pDefines_p = NULL  );

	};
}
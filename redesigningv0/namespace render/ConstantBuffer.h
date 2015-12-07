//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/16
	created:	16:2:2013   11:45
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\Shader.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx
	file base:	Shader
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// system/standard headers


#include <D3D11.h>
//#include <boost/smart_ptr.hpp>
#include <memory>

namespace render{

	//========================================================================
	// holds CBs actual data(values)
	// use this for generic/general cbuffers
	//========================================================================
	class ConstantBuffer{

		typedef UINT byteoffset;

		UINT m_nBytes;
		UINT m_nVariables;

		std::unique_ptr<BYTE[]> m_variablesBuffer;
		std::unique_ptr<byteoffset[]> m_variablesOffsets;
		std::unique_ptr<UINT[]> m_variablesSizes;

		bool m_bNeedUpdate;	// used to check if it needs update to the pipeline

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ConstantBuffer( const UINT size_p, const UINT nVariables_p, const UINT * pVariablesSizes_p );
		ConstantBuffer( const UINT size_p, const UINT nVariables_p )
			:
			m_variablesBuffer(	new BYTE[size_p]()),
			m_variablesOffsets(	new byteoffset[nVariables_p]),
			m_variablesSizes(	new UINT[nVariables_p]){

				m_nBytes = size_p;
				m_nVariables = nVariables_p;
				m_bNeedUpdate = false;
		}
		ConstantBuffer(){}
		void Init( const UINT size_p, const UINT nVariables_p, const UINT * pVariablesSizes_p  );
		void Init( const UINT * pVariablesSizes_p ){

			UINT iOffsetCache = 0;
			for( UINT it = 0; it < m_nVariables; ++it ){

				m_variablesOffsets[it] = iOffsetCache;
				m_variablesSizes[it] = pVariablesSizes_p[it];

				iOffsetCache += m_variablesSizes[it];
			}
		}

		UINT GetSize() const{
			return m_nBytes;
		}

		//------------------------------------------------------------------------
		// Set variable by index
		//------------------------------------------------------------------------
		void SetVariable( UINT index_p, const BYTE* data_p ){

			memcpy( &m_variablesBuffer[ m_variablesOffsets[index_p] ], data_p, m_variablesSizes[index_p] );
			m_bNeedUpdate = true;
		}

		//------------------------------------------------------------------------
		// Get variable by index
		//------------------------------------------------------------------------
		BYTE * GetVariable( UINT index_p ){
			return &m_variablesBuffer[m_variablesOffsets[index_p]];
		}

		//------------------------------------------------------------------------
		// Set the entire buffer at once
		//------------------------------------------------------------------------
		void Set( const BYTE * data_p ){

			memcpy( m_variablesBuffer.get(), data_p, m_nBytes );
			m_bNeedUpdate = true;
		}

		//------------------------------------------------------------------------
		// Test if buffer data was modified since last update call, than updates it,
		// or not.
		//------------------------------------------------------------------------
		void Update( ID3D11DeviceContext * pContext_p, ID3D11Buffer *& pConstantBuffer_p ){

			if( m_bNeedUpdate ){

				pContext_p->UpdateSubresource( pConstantBuffer_p, 0, NULL, m_variablesBuffer.get(), 0, 0 );
				m_bNeedUpdate = false;
			}
		}
	};

	typedef std::shared_ptr<ConstantBuffer> shared_CBuffer_ptr;
}
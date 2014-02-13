//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/15
	created:	15:2:2013   18:17
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\namespace render\DrawCall.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\namespace render
	file base:	DrawCall
	file ext:	h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	This is a command pattern class.

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// libraries needed

#pragma comment( lib, "D3D11.lib")
#pragma comment( lib, "D3DX11.lib" )

// system/standard headers

#include <D3Dcommon.h>
#include <D3D11.h>
#include <D3DX11core.h>	//including because of D3DERR_INVALIDCALL
#include <memory>

// private headers
#include "Command.h"

namespace dx{


	//========================================================================
	// Draw stuff trough given device context.
	//========================================================================
	class DrawCall: public Command{
	public:
		enum E_TYPE{

			E_Normal,
			E_Indexed,
			E_Instanced,
			E_IndexedInstanced			
		};
	private:

		E_TYPE m_type;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		DrawCall( E_TYPE eType_p ): m_type(eType_p){}
		virtual ~DrawCall(){};

		//------------------------------------------------------------------------
		// What this object binds?
		//------------------------------------------------------------------------
		E_TYPE Type() const {
		
			return m_type;
		}
	};

	typedef std::shared_ptr<DrawCall> shared_DrawCall_ptr;

	//========================================================================
	// 
	//========================================================================
	class DrawIndexed: public DrawCall{

		UINT m_nIndexes;		// how much indexes from the bound buffer to use
		UINT m_iStartIndex;		// from where to start on the buffer
		int m_iBaseVertexLocation;	// "value added to each index before reading a vertex from the vertex buffer"
									// "Offset from the start of the vertex buffer to the first vertex."
	public:

		DrawIndexed( UINT nIndexes_p, UINT iStartIndex_p = 0, int iBaseVertexLocation_p = 0 )
			:
		DrawCall(E_Indexed),
		m_nIndexes(nIndexes_p), m_iStartIndex(iStartIndex_p), m_iBaseVertexLocation(iBaseVertexLocation_p){}

		virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

			pDeviceContext_p->DrawIndexed( m_nIndexes, m_iStartIndex, m_iBaseVertexLocation );
		}
	};

	typedef std::shared_ptr<DrawIndexed> shared_DrawIndexed_ptr;

	//========================================================================
	// 
	//========================================================================
	class DrawIndexedInstanced: public DrawCall{

		UINT	m_nIndexPerInstance;		// Number of indices read from the index buffer for each instance.
		UINT	m_nInstances;
		UINT	m_iStartIndexLocation;
		int		m_iBaseVertexLocation;
		UINT	m_iStartInstanceLocation;	// A value added to each index before reading per-instance data from a vertex buffer.

	public:

		DrawIndexedInstanced( UINT nIndexPerInstance_p, UINT nInstances_p = 0,
								UINT iStartIndexLocation_p = 0, int iBaseVertexLocation_p = 0, UINT iStartInstanceLocation_p = 0 )
			:
		DrawCall(E_IndexedInstanced),
		m_nIndexPerInstance(nIndexPerInstance_p), m_nInstances(nInstances_p),
		m_iStartIndexLocation(iStartIndexLocation_p),
		m_iBaseVertexLocation(iBaseVertexLocation_p),
		m_iStartInstanceLocation(iStartInstanceLocation_p){}

		void UpdateNumberOfInstances( UINT nInstances_p ){

			m_nInstances = nInstances_p;
		}

		virtual void Execute( ID3D11DeviceContext * pDeviceContext_p ){

			pDeviceContext_p->DrawIndexedInstanced( m_nIndexPerInstance, m_nInstances, m_iStartIndexLocation, m_iBaseVertexLocation, m_iStartInstanceLocation );
		}
	};
}
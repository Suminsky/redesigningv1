//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/03/16
	created:	16:2:2013   11:45
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\Shader.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx
	file base:	Shader
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	TODO: create a default camera that holds common RTV (the back buffer), common viewport (full screen one), and cameras that get
				their bind commands from it

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

#include <vector>

// private headers
#include "../namespace render/dx/pipeline/Binder.h"
#include "../namespace render/dx/Device.h"
#include "CameraCbuffer.h"

namespace sprite{

	class Camera{
	public:

		DirectX::XMMATRIX	m_mProjection;		// the projection matrix, the aspect ratio and scaling
		DirectX::XMMATRIX	m_mView;			// the view matrix, the camera pos and orientation
		D3D11_VIEWPORT		m_viewPort;			// the viewport rectangle on the render target
		//UINT			m_RenderTargetID;		// the render target the camera will output to
	
		dx::State m_pipeState; //binds viewport, render target, 	

	private:

		CameraCbuffer  m_renderData;

	public:
		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Camera(){

			// initializes projection and view to identity

			m_mProjection = m_mView = DirectX::XMMatrixIdentity();
		}
		Camera( UINT width_p, UINT height_p, dx::Device * pDevice_p, ID3D11RenderTargetView * pRTV_p ){

			// initializes projection and view to identity

			m_mView = DirectX::XMMatrixIdentity();

			BuildPipeState(width_p, height_p, pDevice_p, pRTV_p);
		}
		virtual ~Camera(){}

		//------------------------------------------------------------------------
		// builds/rebuilds the pipe state, setting viewport, cbuffer1 and RTV 
		// bind commands
		//------------------------------------------------------------------------
		void BuildPipeState( UINT width_p, UINT height_p,  dx::Device * pDevice_p, ID3D11RenderTargetView * pRTV_p );

		//------------------------------------------------------------------------
		// Update cbuffer data, and set to update to buffer interface
		//------------------------------------------------------------------------
		void Update();
	};
}
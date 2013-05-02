//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/15
	created:	15:2:2013   19:26
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\DrawablesQueue.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render
	file base:	DrawablesQueue
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	A object that can be draw to a render target, in a single draw call, that is.

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// libraries needed

#pragma comment( lib, "DXGI.lib")

// system/standard headers

#include <D3D11.h>

namespace dx{

	class SwapChain{

	public:
		struct ModePriority{

			int iResolution,
				iFormat,
				iRefreshRate,
				iScanlineOrder,
				iScaling;

			/*ModePriority( int iResolution_p, int iFormat_p, int iRefreshRate_p, int iScanlineOrder_p, int iScaling_p):
				iResolution(iResolution_p),
				iFormat(iFormat_p),
				iRefreshRate(iRefreshRate_p),
				iScanlineOrder(iScanlineOrder_p),
				iScaling(iScaling_p){};*/
		};
		struct GpuAndMode{
			IDXGIAdapter1 * pAdapter;
			IDXGIOutput * pOutput;
			DXGI_MODE_DESC mode;
			ModePriority modeMatchPriority;

			GpuAndMode(){}
			GpuAndMode(IDXGIAdapter1 * adapter_p, IDXGIOutput * outputMode_p, DXGI_MODE_DESC & mode_p)
				: pAdapter(adapter_p), pOutput(outputMode_p), mode(mode_p){}
		};

		IDXGISwapChain * m_pSwapChain;

		DXGI_MODE_DESC m_BackBufferDisplayMode;	//current display(pOutput) mode being used
		IDXGIOutput * m_pOutput;				//setted by param when creating the swap chain, for convenience

		ID3D11RenderTargetView * m_pBackBufferRTV;		// i dont see why a swap chain would be created if you do not get a view from its bbuffer??

		UINT m_iPresentFlags;
		UINT m_nBuffers;
		UINT m_iSyncInterval;				//vsync = 1
		UINT m_iFlags;
		UINT m_nMSAA;
		UINT m_iMSAAQuality;
		BOOL m_bWindowed;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		SwapChain() : m_pSwapChain(NULL), m_pOutput(NULL), m_pBackBufferRTV(NULL){};
		virtual ~SwapChain(){

			if( m_pSwapChain )
				m_pSwapChain->Release();
			if( m_pOutput )
				m_pOutput->Release();
			if(m_pBackBufferRTV)
				m_pBackBufferRTV->Release();
		};

		//------------------------------------------------------------------------
		// creates the swap chain
		//------------------------------------------------------------------------
		void CreateTheSwapChain(	//in:
									ID3D11Device *pDevice_p,
									IDXGIFactory1 *pFactory_p,
									const HWND hWnd_p,//reqired
									const BOOL bChangeResolutionWhenResizingInFullscreen_p,
									//in / holded:
									const BOOL bWindowed_p,
									const UINT nMSAA_p, const UINT iMSAAqlty_p,
									const DXGI_MODE_DESC OutputMode_p,//used also to check MSAA for the format								
									const UINT nBuffer_p,
									IDXGIOutput *pOutput_p	 //not used only holded
									);

		//------------------------------------------------------------------------
		// resize buffers
		// the rtv is released and recreated
		//------------------------------------------------------------------------
		void Resize( UINT width_p, UINT height_p, ID3D11Device * pDevice_p );

		//------------------------------------------------------------------------
		// changes between windowed and fullscreen mode, depending on current mode.
		//------------------------------------------------------------------------
		bool SwitchFullscreenMode();

		//------------------------------------------------------------------------
		//Resolution is meanfull only for fullscreen.
		//Returns:
		//The pAdapter witch supports requested attributes;
		//The Output tied to the pAdapter, witch supports the attributes;
		//The Output mode itself. But only if the function succeeds, if it fails, it will return
		//closest matched pOutput.
		//The Factory Used to get the pAdapter...This is needed because the dx device uses the pAdapter(the one returned here)
		//factory to be created, and DX gives a warning if you use a different factory to create the swap chain...Then
		//just pass this to the swap chain creation..
		//
		//The pAdapter and the pOutput have to be realised by the requester(outside).
		//
		//pAdapter = video card
		//pOutput = monitor
		//
		//The pAdapter is used to dx device creation(only, and getting its outputs.Thats why its not holded by the class).
		//The pOutput is used mainly for getting a mode desc, but also for entering fullscren.
		//The mode is used for creating the swapchain, and for changing the display mode when
		//going fullscreen(ResizeTarget).
		//	But its usefull more yet cause it holds Format and Resolution info, witch are used
		//	widely.(resizebuffers, creating render target, etc..)
		//
		//NOTE: The scaling mode describes what to do when(on fullscreen) the bbuffer/rt have a size different from the monitor
		//resolution(should it center or strech?). When the monitor res and the bbuffer/rt have the same res,
		//theres nothing to be donne(means the scaling should never be stretched or centered, otherwise it doesnt find any
		//display mode..)-> Thats how you find the native resolution, when you get the unspecified scaling for a resolution.
		//SO, centered or stretched are a for not native res only, and for that case they a must! theres no unspecified for a not
		//native res.
		//
		//NOTE: Remember to check if setting a fullscreen with 1440x900 mode, with centered or stretched gives a error.
		//
		//NOTE: Actually, forget that..MSDN says use current desktop resolution as the native one.
		//
		//-THROWS EXCEPTION in case of fail in creating factory.
		//-Return:	GF_ERROR_FAIL if didnt find
		//			GF_ERROR_OK if find exactly match to all requested features.
		//			GF_FIND_CLOSE if find one that doesnt satisfies all requests.
		//------------------------------------------------------------------------
		int GetCapableAdapterAndOutputAndModeClosestMatch(	//in:
															DXGI_MODE_DESC &desiredFSDisplayMode_p,
															//in/out:
															GpuAndMode &gpuAndMode_p, //in only thepriorities
															//out:
															IDXGIFactory1 *& pFactory_p );

		//------------------------------------------------------------------------
		// get a view from from the swap chain back buffer.
		// makes the back buffer drawable
		//------------------------------------------------------------------------
		void CreateRTVFromBackBuffer( ID3D11Device * pDevice_p );
	};
}
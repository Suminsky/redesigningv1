#pragma once

#define FULLSCREEN	false
#define N_RENDERBUFFERS 3
#define WINDOW_W 1280
#define WINDOW_H 720

// private includes
#include "../namespace win/win_IDs.h"
#include "../namespace win/Window.h"
#include "../namespace render/dx/SwapChain.h"
#include "../namespace render/dx/Device.h"
#include "../namespace sprite/SpriteRenderer.h"
#include "../namespace game/Game.h"

#include "../namespace win/MouseInput.h"

#include "TestState.h"

class TestGameWindow : public win::Window, public game::Game{

public:

	dx::SwapChain m_swapChain;
	dx::Device m_device;
	sprite::SpriteRenderer m_spriteRenderer;
	win::MouseInput m_mouseInput;

	//------------------------------------------------------------------------
	// ctor
	//------------------------------------------------------------------------
	TestGameWindow( HINSTANCE hInst_p ):Window(hInst_p){

		RegisterWndClass( TEXT("className"), NULL, LoadIcon( hInst_p, TEXT("RC_GSP_ICONS")), CS_PARENTDC );

		win::Rect winRect( 100, 100, WINDOW_W, WINDOW_H );
		Create(winRect, TEXT("className"), NULL, TEXT("title"), WS_VISIBLE|WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE );


		//m_spriteRenderer.LoadShader( &m_device, "vs_Sprite", "ps_Sprite");



		//if(FULLSCREEN)
		//	m_spriteRenderer.m_camera.BuildPipeState( 1440, 900, &m_device, m_swapChain.m_pBackBufferRTV );
		//else
		//	m_spriteRenderer.m_camera.BuildPipeState( WINDOW_W, WINDOW_H, &m_device, m_swapChain.m_pBackBufferRTV );

		////---
		//
		////Game::m_stateControl.ChangeState( std::make_shared<TestState>(TestState(&m_device, &m_spriteRenderer)) );
	}

	//------------------------------------------------------------------------
	// Check current client rect and compares with the client rect the Window
	// current holds, if differs, the buffers are resized.
	// The rects are updated regardless if size differs or not.
	//------------------------------------------------------------------------
	void ResizeIfNeed(){

		// get current client size

		RECT newCliRECT = {0};
		::GetClientRect( m_hWnd, &newCliRECT );

		// check if size differs from current holded size

		win::Rect newCliRect(newCliRECT); // convert
		if( newCliRect.w != m_cliRect.w && newCliRect.w > 8
			|| newCliRect.h != m_cliRect.h && newCliRect.h > 8 ){

				//if( newCliRect.w < 8 ) newCliRect.w = 8;
				//if( newCliRECT.right < 8 ) newCliRect.h = 8;

				m_device.GetContext()->OMSetRenderTargets( 0, NULL, NULL); // If I dont do this, it will crash sometimes!!! really the fuck behavior

				m_swapChain.Resize(newCliRect.w, newCliRect.h, m_device.GetDevice());

				m_device.GetContext()->OMSetRenderTargets( 1, &m_swapChain.m_pBackBufferRTV, NULL);


				m_spriteRenderer.m_camera.m_viewPort.Width = (float)newCliRect.w;
				m_spriteRenderer.m_camera.m_viewPort.Height = (float)newCliRect.h; //Init( newCliRect.w, newCliRect.h,  &m_device, m_swapChain.m_pBackBufferRTV);
				// NOTE: recreating the camera binds is failing because the bind viewport command is being allocated on the
				// same address ._., can u beliv this shit..I basically cant have a binder substituted by destroying and
				// reallocating because

				m_spriteRenderer.m_camera.m_mProjection = DirectX::XMMatrixOrthographicLH((float)newCliRect.w, (float)newCliRect.h, 0.0f, 1.0f );

				m_spriteRenderer.m_camera.Update();

				m_spriteRenderer.m_queue.ResetState( dx::E_RS_ViewPort );
				m_spriteRenderer.m_queue.ResetState( dx::E_VS_CBuffer1 );
		}

		// apply the changes to the rect data holded

		m_cliRect = newCliRect;
		::GetWindowRect(m_hWnd, &newCliRECT); // use the same structure for convenience
		m_rect.FromRECT(newCliRECT);
	}

	virtual LRESULT CALLBACK WndProcHandler( HWND hWnd_p, UINT Msg_p, WPARAM wParam_p, LPARAM lParam_p ){

		switch( Msg_p ){

		case WM_MOUSEMOVE:
			m_mouseInput.OnWM_MOUSEMOVE(lParam_p);
			return 0;
		case WM_MOUSEHWHEEL:
			m_mouseInput.OnWM_MOUSEWHEEL(wParam_p);
			return 0;
		case WM_LBUTTONDOWN:
			m_mouseInput.OnWM_LBUTTONDOWN();
			return 0;
		case WM_MBUTTONDOWN:
			m_mouseInput.OnWM_MBUTTONDOWN();
			return 0;
		case WM_RBUTTONDOWN:
			m_mouseInput.OnWM_RBUTTONDOWN();
			return 0;
		case WM_LBUTTONUP:
			m_mouseInput.OnWM_LBUTTONUP();
			return 0;
		case WM_MBUTTONUP:
			m_mouseInput.OnWM_MBUTTONUP();
			return 0;
		case WM_RBUTTONUP:
			m_mouseInput.OnWM_RBUTTONUP();
			return 0;

		case WM_CREATE:{

			DXGI_MODE_DESC modeDesc;
			modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			modeDesc.Width = GetSystemMetrics( SM_CXSCREEN );
			modeDesc.Height = GetSystemMetrics( SM_CYSCREEN );
			modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			modeDesc.RefreshRate.Numerator = modeDesc.RefreshRate.Denominator = 0;

			dx::SwapChain::GpuAndMode gpuAndMode;
			gpuAndMode.modeMatchPriority.iFormat = 60;
			gpuAndMode.modeMatchPriority.iScaling = 0;
			gpuAndMode.modeMatchPriority.iRefreshRate = gpuAndMode.modeMatchPriority.iResolution = gpuAndMode.modeMatchPriority.iScanlineOrder = 0;

			IDXGIFactory1 * pFactory = NULL;
			int result = m_swapChain.GetCapableAdapterAndOutputAndModeClosestMatch( modeDesc, gpuAndMode, pFactory );
			if( !result ) return -1;

			D3D_FEATURE_LEVEL featureLVL = D3D_FEATURE_LEVEL_10_0;

			DXGI_MODE_DESC windowMode = {0};
			windowMode = gpuAndMode.mode; // initialize defaults
			if( !FULLSCREEN ){

				windowMode.Height = WINDOW_H;
				windowMode.Width = WINDOW_W;
				windowMode.Scaling = DXGI_MODE_SCALING_STRETCHED;
			}

			//windowedMode.RefreshRate.Numerator = 60.0;
			//windowedMode.RefreshRate.Denominator = 1.0;

			m_device.InitDevice( gpuAndMode.pAdapter, true, pFactory, &featureLVL, 1  );
			m_swapChain.CreateTheSwapChain( m_device.GetDevice(), pFactory, hWnd_p, false, true, 1,0, windowMode, N_RENDERBUFFERS, gpuAndMode.pOutput );
			m_swapChain.CreateRTVFromBackBuffer(m_device.GetDevice());

			if(FULLSCREEN)
				m_swapChain.SwitchFullscreenMode();


			//---
			m_spriteRenderer.Init(&m_device);
			m_spriteRenderer.LoadShader( &m_device, "vs_Sprite", "ps_Sprite");

			if(FULLSCREEN)
				m_spriteRenderer.m_camera.BuildPipeState( 1440, 900, &m_device, m_swapChain.m_pBackBufferRTV );
			else
				m_spriteRenderer.m_camera.BuildPipeState( WINDOW_W, WINDOW_H, &m_device, m_swapChain.m_pBackBufferRTV );

			//---

			//Game::m_stateControl.ChangeState( std::make_shared<TestState>(TestState(&m_device, &m_spriteRenderer)) );


					   }return 0;
		case WM_CLOSE:
			if(FULLSCREEN)
				m_swapChain.SwitchFullscreenMode(); // otherwise debug outputs shows live states
			DestroyWindow(hWnd_p);
			return 0;

		case WM_DESTROY:

			PostQuitMessage(0);
			return 0;

		case WM_SIZE:

			ResizeIfNeed();

			BREAKHERE;
			return 0;

		default:
			return DefWindowProc( hWnd_p, Msg_p, wParam_p, lParam_p);
		}

		//return 0;
	}
};
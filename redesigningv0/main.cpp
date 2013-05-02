//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/05
	created:	5:2:2013   18:13
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace win\main.cpp
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace win
	file base:	main
	file ext:	cpp
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

#define FULLSCREEN	false
#define N_RENDERBUFFERS 3
#define WINDOW_W 1280
#define WINDOW_H 720

// system includes

#include <crtdbg.h>

// private includes
#include "namespace win/win_IDs.h"
#include "namespace win/Window.h"
#include "namespace render/dx/SwapChain.h"
#include "namespace render/dx/Device.h"
#include "namespace sprite/SpriteRenderer.h"
#include "namespace sprite/Sprite.h"

#include "namespace win/FileLogger.h"
#include "namespace win/HighPerformanceTimer.h"
#include "namespace win/KeyboardAsyncInput.h"


class TestWindow : public win::Window{

public:

	dx::SwapChain m_swapChain;
	dx::Device m_device;
	sprite::SpriteRenderer m_spriteRenderer;


	TestWindow( HINSTANCE hInst_p ):Window(hInst_p){}

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

			

			//m_swapChain.m_pSwapChain->Present(0, 0);

			m_spriteRenderer.Init(&m_device);


		
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

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//entrypoint
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
INT WINAPI WinMain( HINSTANCE hInstance_p, HINSTANCE hPrevInstance_p, LPSTR lpCmdLine_p, INT nShowCmd_p ){


		UNREFERENCED_PARAMETER( lpCmdLine_p );
		UNREFERENCED_PARAMETER( hPrevInstance_p );
		UNREFERENCED_PARAMETER( nShowCmd_p );

#ifdef DEBUG
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


		TestWindow myWindow( hInstance_p );

		myWindow.RegisterWndClass( TEXT("className"), NULL, LoadIcon( hInstance_p, TEXT("RC_GSP_ICONS")), CS_PARENTDC );

		win::Rect winRect( 100, 100, WINDOW_W, WINDOW_H );
		myWindow.Create(winRect, TEXT("className"), NULL, TEXT("title"), WS_VISIBLE|WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE );

		
		myWindow.m_spriteRenderer.LoadShader( &myWindow.m_device, "vs_Sprite", "ps_Sprite");
		if(FULLSCREEN)
		myWindow.m_spriteRenderer.m_camera.BuildPipeState( 1440, 900, &myWindow.m_device, myWindow.m_swapChain.m_pBackBufferRTV );
		else
		myWindow.m_spriteRenderer.m_camera.BuildPipeState( WINDOW_W, WINDOW_H, &myWindow.m_device, myWindow.m_swapChain.m_pBackBufferRTV );

		sprite::Sprite mySprite(	&myWindow.m_device, "Contents/samusSetTry1.png",
									468.0f, 234.0f, 0.0f, 0.0f,
									sprite::E_ALPHA_BLENDED, sprite::E_NONE, &myWindow.m_spriteRenderer );

		sprite::Sprite myOtherSprite(	&myWindow.m_device, "Contents/samusSetTry1.png",
										468.0f, 234.0f, 0.0f, 0.0f,
										sprite::E_ALPHA_BLENDED, sprite::E_NONE, &myWindow.m_spriteRenderer );
		myOtherSprite.m_trafo.m_vCurrentPosition = DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::XMVectorSet(0.0f, -234.0f, 0.0f, 0.0f));

		MSG Msg; ZeroMemory( &Msg, sizeof(MSG));
		win::UniqueHigPerfTimer().Init();
		win::UniqueFileLogger().SetDigitsCount( 6, 6);


		bool bGoingRight = true;
		while( Msg.message != WM_QUIT ){

			//myWindow.m_swapChain.m_pSwapChain->Present(1, 0);		

			win::UniqueHigPerfTimer().Update();
			win::UniqueFileLogger()<<win::UniqueHigPerfTimer().GetDeltaSeconds()<<SZ_NEWLINE;

			while( PeekMessage( &Msg, NULL, NULL, NULL, PM_REMOVE )	){

				//TranslateMessage( &Msg );
				DispatchMessage( &Msg );

				win::UniqueFileLogger()<<"WM"<<SZ_NEWLINE;
			}

			win::UniqueKeyboardAsyncInput().UpdateKeyStates();

			//========================================================================
			// GLENN loop
			static double frameDeltaAccumulated = 0.0;
			static const double fixedStep = 1.0/60.0;//60.0;//24.0;//1/3000.0;

			double currentFrameDeltaSec = win::UniqueHigPerfTimer().GetDeltaSeconds();
			if( currentFrameDeltaSec > 0.25 )//fixedStep )//0.25
				currentFrameDeltaSec = 0.25;//fixedStep; //0.25  avoid spiral of death
			
			// NOTE: I comparing to the fixedStep cause I beliv that way the game will freeze and return to the exact point
			// where it freezed, not "losing" any step. im not so sure

			//------------------------------------------------------------------------
			// FRANKS FIX - Hodgman version
			/*static double buffer = 0.0;

			const double actualDelta = currentFrameDeltaSec + buffer;
			double frameCount = floor( actualDelta * 60.0 + 0.5 );
			frameCount = max(1.0, frameCount);
			currentFrameDeltaSec = frameCount/60.0;
			buffer = actualDelta - currentFrameDeltaSec;*/

			//// FRANKS VERSION
			//static const double minFrameRate = 15.0;
			//currentFrameDeltaSec = min(currentFrameDeltaSec, 1/minFrameRate);

			//// this buffer keeps track of the extra bits of time
			//static double deltaBuffer = 0;

			//// add in whatever time we currently have saved in the buffer
			//currentFrameDeltaSec += deltaBuffer;

			//// calculate how many frames the delta is telling us will have passed
			//static const double refreshRate = (double)myWindow.m_swapChain.m_BackBufferDisplayMode.RefreshRate.Numerator/(double)myWindow.m_swapChain.m_BackBufferDisplayMode.RefreshRate.Denominator;
			//int frameCount = (int)(currentFrameDeltaSec * refreshRate + 0.5);

			//// if less then a full frame, increase delta to cover the extra
			//if (frameCount <= 0)	frameCount = 1;

			//// save off the delta, we will need it later to update the buffer
			//const double oldDelta = currentFrameDeltaSec;

			//// recalculate delta to be an even frame rate multiple
			//currentFrameDeltaSec = frameCount / (double)refreshRate;

			//// update delta buffer so we keep the same time on average
			//deltaBuffer = oldDelta - currentFrameDeltaSec;
			//------------------------------------------------------------------------

			frameDeltaAccumulated += currentFrameDeltaSec;

			while( frameDeltaAccumulated >= fixedStep ){

				frameDeltaAccumulated -= fixedStep;
			//========================================================================
			
			//========================================================================
			// SPIRO loop:
			//LONGLONG currentCount = win::UniqueHigPerfTimer().GetCountAccum();
			// 
			//while( currentCount - updatedCount > 33333ULL ){// updates each 0.033333 seconds
			//	updatedCount += 33333ULL;
			//	
			//========================================================================
			

				mySprite.m_trafo.Step();
				myOtherSprite.m_trafo.Step();

				if( win::UniqueKeyboardAsyncInput().KeySinglePress(VK_A)){

					myOtherSprite.m_trafo.m_vCurrentPosition = DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::g_XMNegIdentityR0);		
				}
				if( win::UniqueKeyboardAsyncInput().KeySinglePress(VK_D)){

					myOtherSprite.m_trafo.m_vCurrentPosition = DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::g_XMIdentityR0);		
				}

				if( bGoingRight ){//win::UniqueKeyboardAsyncInput().IsKeyDown(VK_LEFT)){
					
					const DirectX::FXMVECTOR vVel = DirectX::XMVectorSet( (float)(100.0*fixedStep), 0.0f, 0.0f, 0.0f );
					mySprite.m_trafo.m_vCurrentPosition =  DirectX::XMVectorAdd( mySprite.m_trafo.m_vCurrentPosition, vVel);


					//myOtherSprite.m_trafo.m_vCurrentPosition =  DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::XMVectorNegate(vVel));

					if(  DirectX::XMVectorGetX( mySprite.m_trafo.m_vCurrentPosition ) > 500.0f ) bGoingRight = false;
				}
				else{//win::UniqueKeyboardAsyncInput().IsKeyDown(VK_RIGHT)){

					const DirectX::FXMVECTOR vVel = DirectX::XMVectorSet( -(float)(100.0*fixedStep), 0.0f, 0.0f, 0.0f );
					mySprite.m_trafo.m_vCurrentPosition =  DirectX::XMVectorAdd( mySprite.m_trafo.m_vCurrentPosition, vVel);

					//myOtherSprite.m_trafo.m_vCurrentPosition =  DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::XMVectorNegate(vVel));

					if(  DirectX::XMVectorGetX( mySprite.m_trafo.m_vCurrentPosition ) < -500.0f ) bGoingRight = true;
				}
			}

			//------------------------------------------------------------------------
			// interpolate:
			//  a*(1-t) + b*t
			//  ==
			//  a + (b-a)*t

			const double alpha = frameDeltaAccumulated / fixedStep;

			mySprite.Update( alpha );
			myOtherSprite.Update( alpha );
			//------------------------------------------------------------------------
			myWindow.m_spriteRenderer.m_camera.Update();
			//static FLOAT clearcolor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			//myWindow.m_device.GetContext()->ClearRenderTargetView( myWindow.m_swapChain.m_pBackBufferRTV, clearcolor);
			myWindow.m_spriteRenderer.Render( &mySprite );
			myWindow.m_spriteRenderer.Render( &myOtherSprite );
			myWindow.m_spriteRenderer.Raster( myWindow.m_device.GetContext());

			myWindow.m_swapChain.m_pSwapChain->Present(1, 0);
		}

		return 0;
}
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

// system includes

#include <crtdbg.h>

// private includes
#include "namespace sprite/Sprite.h"

#include "namespace win/FileLogger.h"
#include "namespace win/HighPerformanceTimer.h"
#include "namespace win/KeyboardAsyncInput.h"


#include "testing/gameWindow.h"

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//entrypoint
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
INT WINAPI WinMain( HINSTANCE hInstance_p, HINSTANCE hPrevInstance_p, LPSTR lpCmdLine_p, INT nShowCmd_p ){

		UNREFERENCED_PARAMETER( lpCmdLine_p );
		UNREFERENCED_PARAMETER( hPrevInstance_p );
		UNREFERENCED_PARAMETER( nShowCmd_p );

#ifdef _DEBUG
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

		TestGameWindow myGameWindow( hInstance_p );

		myGameWindow.m_stateControl.ChangeState( std::make_shared<TestState>(TestState(&myGameWindow.m_device, &myGameWindow.m_spriteRenderer, &myGameWindow.m_mouseInput)) );

		/*myGameWindow.RegisterWndClass( TEXT("className"), NULL, LoadIcon( hInstance_p, TEXT("RC_GSP_ICONS")), CS_PARENTDC );

		win::Rect winRect( 100, 100, WINDOW_W, WINDOW_H );
		myGameWindow.Create(winRect, TEXT("className"), NULL, TEXT("title"), WS_VISIBLE|WS_OVERLAPPEDWINDOW, WS_EX_CLIENTEDGE );

		
		myGameWindow.m_spriteRenderer.LoadShader( &myGameWindow.m_device, "vs_Sprite", "ps_Sprite");



		if(FULLSCREEN)
			myGameWindow.m_spriteRenderer.m_camera.BuildPipeState( 1440, 900, &myGameWindow.m_device, myGameWindow.m_swapChain.m_pBackBufferRTV );
		else
			myGameWindow.m_spriteRenderer.m_camera.BuildPipeState( WINDOW_W, WINDOW_H, &myGameWindow.m_device, myGameWindow.m_swapChain.m_pBackBufferRTV );
*/


		/*sprite::Sprite mySprite(	&myGameWindow.m_device, "Contents/samusSetTry1.png",
									468.0f, 234.0f, 0.0f, 0.0f,
									sprite::E_ALPHA_BLENDED, sprite::E_NONE, &myGameWindow.m_spriteRenderer );
		sprite::Sprite myOtherSprite(	&myGameWindow.m_device, "Contents/samusSetTry1.png",
										468.0f, 234.0f, 0.0f, 0.0f,
										sprite::E_ALPHA_BLENDED, sprite::E_NONE, &myGameWindow.m_spriteRenderer );

		myOtherSprite.m_trafo.m_vCurrentPosition = DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::XMVectorSet(0.0f, -234.0f, 0.0f, 0.0f));

*/
		//---
		//int * pX = new int[1024];

		MSG Msg; ZeroMemory( &Msg, sizeof(MSG));
		win::UniqueHigPerfTimer().Init();
		win::UniqueFileLogger().SetDigitsCount( 6, 6);

		myGameWindow.Init();

		//bool bGoingRight = true;
		while( Msg.message != WM_QUIT ){

			//myGameWindow.m_swapChain.m_pSwapChain->Present(1, 0);		

			//win::UniqueHigPerfTimer().Update();
			//win::UniqueFileLogger()<<win::UniqueHigPerfTimer().GetDeltaSeconds()<<SZ_NEWLINE;

			myGameWindow.m_mouseInput.Update();

			while( PeekMessage( &Msg, NULL, NULL, NULL, PM_REMOVE )	){

				//TranslateMessage( &Msg );
				DispatchMessage( &Msg );

				win::UniqueFileLogger()<<"WM"<<SZ_NEWLINE;
			}

			win::UniqueKeyboardAsyncInput().UpdateKeyStates();

			myGameWindow.FixedStepLoop();

			//myGameWindow.m_spriteRenderer.Raster( myGameWindow.m_device.GetContext());

			myGameWindow.m_swapChain.m_pSwapChain->Present(1, 0);


			// uncomment bellow here

			////========================================================================
			//// GLENN loop
			//static double frameDeltaAccumulated = 0.0;
			//static const double fixedStep = 1.0/60.0;//60.0;//24.0;//1/3000.0;

			//double currentFrameDeltaSec = win::UniqueHigPerfTimer().GetDeltaSeconds();
			//if( currentFrameDeltaSec > 0.25 )//fixedStep )//0.25
			//	currentFrameDeltaSec = 0.25;//fixedStep; //0.25  avoid spiral of death
			//
			//// NOTE: I comparing to the fixedStep cause I beliv that way the game will freeze and return to the exact point
			//// where it freezed, not "losing" any step. im not so sure

			////------------------------------------------------------------------------
			//// FRANKS FIX - Hodgman version
			///*static double buffer = 0.0;

			//const double actualDelta = currentFrameDeltaSec + buffer;
			//double frameCount = floor( actualDelta * 60.0 + 0.5 );
			//frameCount = max(1.0, frameCount);
			//currentFrameDeltaSec = frameCount/60.0;
			//buffer = actualDelta - currentFrameDeltaSec;*/

			////// FRANKS VERSION
			////static const double minFrameRate = 15.0;
			////currentFrameDeltaSec = min(currentFrameDeltaSec, 1/minFrameRate);

			////// this buffer keeps track of the extra bits of time
			////static double deltaBuffer = 0;

			////// add in whatever time we currently have saved in the buffer
			////currentFrameDeltaSec += deltaBuffer;

			////// calculate how many frames the delta is telling us will have passed
			////static const double refreshRate = (double)myGameWindow.m_swapChain.m_BackBufferDisplayMode.RefreshRate.Numerator/(double)myGameWindow.m_swapChain.m_BackBufferDisplayMode.RefreshRate.Denominator;
			////int frameCount = (int)(currentFrameDeltaSec * refreshRate + 0.5);

			////// if less then a full frame, increase delta to cover the extra
			////if (frameCount <= 0)	frameCount = 1;

			////// save off the delta, we will need it later to update the buffer
			////const double oldDelta = currentFrameDeltaSec;

			////// recalculate delta to be an even frame rate multiple
			////currentFrameDeltaSec = frameCount / (double)refreshRate;

			////// update delta buffer so we keep the same time on average
			////deltaBuffer = oldDelta - currentFrameDeltaSec;
			////------------------------------------------------------------------------

			//frameDeltaAccumulated += currentFrameDeltaSec;

			//while( frameDeltaAccumulated >= fixedStep ){

			//	frameDeltaAccumulated -= fixedStep;
			////========================================================================
			//
			////========================================================================
			//// SPIRO loop:
			////LONGLONG currentCount = win::UniqueHigPerfTimer().GetCountAccum();
			//// 
			////while( currentCount - updatedCount > 33333ULL ){// updates each 0.033333 seconds
			////	updatedCount += 33333ULL;
			////	
			////========================================================================
			//

				//mySprite.m_trafo.Step();
				//myOtherSprite.m_trafo.Step();

			//	if( win::UniqueKeyboardAsyncInput().KeySinglePress(VK_A)){

			//		myOtherSprite.m_trafo.m_vCurrentPosition = DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::g_XMNegIdentityR0);		
			//	}
			//	if( win::UniqueKeyboardAsyncInput().KeySinglePress(VK_D)){

			//		myOtherSprite.m_trafo.m_vCurrentPosition = DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::g_XMIdentityR0);		
			//	}

			//	if( bGoingRight ){//win::UniqueKeyboardAsyncInput().IsKeyDown(VK_LEFT)){
			//		
			//		const DirectX::FXMVECTOR vVel = DirectX::XMVectorSet( (float)(100.0*fixedStep), 0.0f, 0.0f, 0.0f );
			//		mySprite.m_trafo.m_vCurrentPosition =  DirectX::XMVectorAdd( mySprite.m_trafo.m_vCurrentPosition, vVel);


			//		//myOtherSprite.m_trafo.m_vCurrentPosition =  DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::XMVectorNegate(vVel));

			//		if(  DirectX::XMVectorGetX( mySprite.m_trafo.m_vCurrentPosition ) > 500.0f ) bGoingRight = false;
			//	}
			//	else{//win::UniqueKeyboardAsyncInput().IsKeyDown(VK_RIGHT)){

			//		const DirectX::FXMVECTOR vVel = DirectX::XMVectorSet( -(float)(100.0*fixedStep), 0.0f, 0.0f, 0.0f );
			//		mySprite.m_trafo.m_vCurrentPosition =  DirectX::XMVectorAdd( mySprite.m_trafo.m_vCurrentPosition, vVel);

			//		//myOtherSprite.m_trafo.m_vCurrentPosition =  DirectX::XMVectorAdd( myOtherSprite.m_trafo.m_vCurrentPosition, DirectX::XMVectorNegate(vVel));

			//		if(  DirectX::XMVectorGetX( mySprite.m_trafo.m_vCurrentPosition ) < -500.0f ) bGoingRight = true;
			//	}
			//}

			////------------------------------------------------------------------------
			//// interpolate:
			////  a*(1-t) + b*t
			////  ==
			////  a + (b-a)*t

			//const double alpha = frameDeltaAccumulated / fixedStep;

			//mySprite.Update( 1.0);//alpha );
			//myOtherSprite.Update( 1.0);//alpha );
			////------------------------------------------------------------------------
			//myGameWindow.m_spriteRenderer.m_camera.Update();
			////static FLOAT clearcolor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			////myGameWindow.m_device.GetContext()->ClearRenderTargetView( myGameWindow.m_swapChain.m_pBackBufferRTV, clearcolor);
			//myGameWindow.m_spriteRenderer.Render( &mySprite );
			//myGameWindow.m_spriteRenderer.Render( &myOtherSprite );
			//myGameWindow.m_spriteRenderer.Raster( myGameWindow.m_device.GetContext());

			//myGameWindow.m_swapChain.m_pSwapChain->Present(1, 0);
		}

		return 0;
}
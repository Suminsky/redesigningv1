#include <PCHF.h>
#include "GameWindow2D_1.h"



using namespace win;
using namespace game;
using namespace dx;
using namespace std;

GameWindow2D_1::GameWindow2D_1( double dTimeStep_p, HINSTANCE hInst_p,
								int winW_p, int winH_p, const char * szWinName_p )
	:
Window(hInst_p),
Game( dTimeStep_p ),
m_spriteText(&m_spriteRenderer.m_tex2D_cache),
m_fileLog(szWinName_p)
{

	// create window

	RegisterWndClass( TEXT("GameWindow2D_1"), NULL, LoadIcon( hInst_p, TEXT("RC_GSP_ICONS")), CS_PARENTDC );

	UINT scW, scH;
	GetCurrentScreenResolution( scW, scH );

	Rect winRect( scW/2 - winW_p/2, scH/2 - winH_p/2, winW_p, winH_p );
	Create(winRect, TEXT("GameWindow2D_1"), NULL, TEXT(szWinName_p),
		WS_VISIBLE|WS_CAPTION|WS_SYSMENU,
		WS_EX_CLIENTEDGE );

	//set raster and scissor state

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;

	ID3D11RasterizerState *pRS = nullptr;
	m_device.GetDevice()->CreateRasterizerState( &rasterDesc, &pRS );
	m_device.GetContext()->RSSetState( pRS );
	pRS->Release();

	// NOTE: setting scissor to true and not setting a scissor fucks things up..I have no idea why

	D3D11_RECT scissor;
	scissor.left = 0;
	scissor.right = winW_p;
	scissor.top = 0;
	scissor.bottom = winH_p;

	m_device.GetContext()->RSSetScissorRects( 1, &scissor );
}

LRESULT CALLBACK GameWindow2D_1::VWndProcHandler( HWND hWnd_p, UINT Msg_p, WPARAM wParam_p, LPARAM lParam_p )
{
	switch( Msg_p ){

	case WM_CREATE:		return OnWM_CREATE( hWnd_p );

	case WM_CLOSE:		return OnWM_CLOSE( hWnd_p );

	case WM_DESTROY:	return OnWM_DESTROY();

	case WM_SIZE:		return OnWM_SIZE( hWnd_p );	

	case WM_SYSCOMMAND:

		// "In WM_SYSCOMMAND messages, the four low-order bits of the wParam parameter are used internally
		// by the system. To obtain the correct result when testing the value of wParam, an application must
		// combine the value 0xFFF0 with the wParam value by using the bitwise AND operator." - msdn

		// process alt+enter switching full screen mode
		// punk commented out
		if( (wParam_p & 0xFFF0) == SC_KEYMENU ){

			if( lParam_p == VK_RETURN ){

				//UINT W, H;
				//GetCurrentScreenResolution( W, H );
				//Resize(W, H);
				//m_swapChain.Resize( WINDOW_W, WINDOW_H, m_device.GetDevice() );
				

				//if( ChangeResolution( 1280, 720 ))
				m_swapChain.SwitchFullscreenMode();

				//// windows FS:
				//ChangeStyle( WS_POPUP|WS_VISIBLE, WS_EX_TOPMOST );
				//Resize( 1440, 900 );
				//MoveTo(0,0);

				//return 0;
			}

			return 0; // return 0 to prevent app from pausing due sys menu
		}

		break;
	}

	return DefWindowProc( hWnd_p, Msg_p, wParam_p, lParam_p);
}

void GameWindow2D_1::ResizeIfNeed()
{
	// get current client size

	RECT newCliRECT = {0};
	::GetClientRect( m_hWnd, &newCliRECT );

	// check if size differs from current holded size

	Rect newCliRect(newCliRECT); // convert

	if( newCliRect.w > 8 && newCliRect.h > 8 )
		if( newCliRect.h != m_cliRect.h || newCliRect.w != m_cliRect.w ){

			m_device.GetContext()->OMSetRenderTargets( 0, NULL, NULL); // If I dont do this, it will crash just sometimes! really the fuck behavior

			m_swapChain.Resize(newCliRect.w, newCliRect.h, m_device.GetDevice());

			//UniqueFileLogger()<<"new cli size: "<<newCliRect.w<<" "<<newCliRect.h<<SZ_NEWLINE;
			//UniqueFileLogger()<<"new bbuff size: "<<(int)m_swapChain.m_BackBufferDisplayMode.Width<<" "<<(int)m_swapChain.m_BackBufferDisplayMode.Height<<SZ_NEWLINE;

			m_spriteRenderer.m_camera.BuildPipeState( newCliRect.w, newCliRect.h, &m_device, m_swapChain.m_pBackBufferRTV );
			m_spriteRenderer.m_camera.Update();

			m_spriteRenderer.m_queue.ResetState( E_RS_ViewPort );
			m_spriteRenderer.m_queue.ResetState( E_VS_CBuffer1 );
			m_spriteRenderer.m_queue.ResetState( E_OM_RenderTargetDepthStencil ); // precisa disso pq realoca no mesmo endereÁo!

			// set scissor so that sprites dont bleed to outside the
			// background image (a centralized 1280 x 720 rect )
			
			//D3D11_RECT scissor;
			// // TODO: this is external info...
			//scissor.left = (newCliRect.w - 1366)/2;
			//scissor.right = 1366 + scissor.left;
			//scissor.top = (newCliRect.h - 768)/2;
			//scissor.bottom = 768 + scissor.top;

			//m_device.GetContext()->RSSetScissorRects( 1, &scissor );

			m_stateControl.OnResize( newCliRect.w, newCliRect.h );
		}

		// apply the changes to the rect data holded

		UpdateRects();
}

LRESULT CALLBACK GameWindow2D_1::OnWM_CREATE( HWND hWnd_p )
{

		// create dx swap chain and device

		DXGI_MODE_DESC modeDesc;
		modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		modeDesc.Width = GetSystemMetrics( SM_CXSCREEN );
		modeDesc.Height = GetSystemMetrics( SM_CYSCREEN );
		modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		modeDesc.RefreshRate.Numerator = modeDesc.RefreshRate.Denominator = 0;

		SwapChain::GpuAndMode gpuAndMode;
		gpuAndMode.modeMatchPriority.iFormat = 60;
		gpuAndMode.modeMatchPriority.iScaling = 0;
		gpuAndMode.modeMatchPriority.iRefreshRate = gpuAndMode.modeMatchPriority.iResolution = gpuAndMode.modeMatchPriority.iScanlineOrder = 0;

		IDXGIFactory1 * pFactory = NULL;
		int result = m_swapChain.GetCapableAdapterAndOutputAndModeClosestMatch( modeDesc, gpuAndMode, pFactory );
		if( !result ) return -1;

		D3D_FEATURE_LEVEL featureLVL = D3D_FEATURE_LEVEL_10_0;

		DXGI_MODE_DESC windowMode = {0};
		windowMode = gpuAndMode.mode; // initialize defaults

		// I think I should always do that, cause as recommended on the SDK, the swap chain should always be created windowed..
		windowMode.Width = m_cliRect.w;
		windowMode.Height = m_cliRect.h;
		

		/*UINT scW, scH;
		GetCurrentScreenResolution( scW, scH );
		if( scW == (UINT)m_cliRect.w && scH == (UINT)m_cliRect.h )
			windowMode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		else
			windowMode.Scaling = DXGI_MODE_SCALING_STRETCHED;*/

		m_device.InitDevice( gpuAndMode.pAdapter, true, &featureLVL, 1  );
		gpuAndMode.pAdapter->Release();
		UINT nRenderBuffers = 3;
		m_swapChain.CreateTheSwapChain( m_device.GetDevice(), pFactory, hWnd_p, false, true, 1,0, windowMode, nRenderBuffers, gpuAndMode.pOutput );
		m_swapChain.CreateRTVFromBackBuffer( m_device.GetDevice() );

		
		

		//---

		// initialize sprite and text renderer

		m_spriteRenderer.Init(&m_device, 1024);
		m_spriteRenderer.m_camera.BuildPipeState( m_cliRect.w, m_cliRect.h, &m_device, m_swapChain.m_pBackBufferRTV );
		//m_textRenderer.Initialize(&m_device, m_spriteRenderer);

		//text::GlyphRect chardescs[512];
		//WCHAR chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ·ÈÌÛ˙‚ÍÙ‡„ı¿√’¡…Õ”⁄¬ ‘1234567890/*-+-_=(){}[]<>/|\\\"`'~^!?,.:;@#$%&";
		//text::CreateFontFiles( "arial_18", L"Arial", 18.0f, Gdiplus::FontStyleRegular, true, m_device, chardescs, chars, sizeof(chars)/sizeof(WCHAR) );

		//text::BmpFont font;
		//font.InitFromFile( "Arial_30.fnt", m_spriteRenderer.m_tex2D_cache );
		//text::BmpFont font2;
		//font2.InitFromFile( "Arial_14.fnt", m_spriteRenderer.m_tex2D_cache );
		//text::BmpFont font3;
		//font3.InitFromFile( "arial_18.fnt", m_spriteRenderer.m_tex2D_cache );

		//m_textRenderer.AddFont( font );
		//m_textRenderer.AddFont( font2 );
		//m_textRenderer.AddFont( font3 );

		return 0;
}

LRESULT CALLBACK GameWindow2D_1::OnWM_CLOSE( HWND hWnd_p )
{
	Destroy();
	DestroyWindow(hWnd_p);
	return 0;
}

LRESULT CALLBACK GameWindow2D_1::OnWM_DESTROY()
{
	PostQuitMessage(0);
	return 0;
}

LRESULT CALLBACK GameWindow2D_1::OnWM_SIZE( HWND hWnd_p )
{
	if( IsIconic(hWnd_p) )
		return 0;

	ResizeIfNeed();

	//m_spriteRenderer.Raster(m_device.GetContext()); // unhandled exception on release launched from vs
	m_swapChain.m_pSwapChain->Present(0, 0);

	BREAKHERE;
	return 0;
}
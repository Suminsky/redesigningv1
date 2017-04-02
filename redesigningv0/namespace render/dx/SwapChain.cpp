#include "PCHF.h"

#include "SwapChain.h"

#include <assert.h>
#include <vector>

// private headers

#include "../../namespace gen/gen_macros.h"

void dx::SwapChain::CreateRTVFromBackBuffer( ID3D11Device * pDevice_p )
{

	if( m_pBackBufferRTV )
		m_pBackBufferRTV->Release();

	// get swc bbuff

	ID3D11Texture2D *pBBuffTexture = NULL;
	m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&pBBuffTexture );

	assert( pBBuffTexture );

	//DBG(
	//D3D11_TEXTURE2D_DESC bbuffTex2DDesc;
	//pBBuffTexture->GetDesc( &bbuffTex2DDesc );
	//m_BackBufferDisplayMode.Width = bbuffTex2DDesc.Width;
	//m_BackBufferDisplayMode.Height = bbuffTex2DDesc.Height;
	//)

	// create a rt view using the swc output params:

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = m_BackBufferDisplayMode.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = (m_nMSAA > 1 )?D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;


	if( FAILED(
		pDevice_p->CreateRenderTargetView( pBBuffTexture, &rtvDesc, &m_pBackBufferRTV )
		)){

			throw std::exception("dx:swapchain create RTV failed");
	}

	pBBuffTexture->Release();
}

void dx::SwapChain::Resize( UINT width_p, UINT height_p, ID3D11Device * pDevice_p )
{
	assert(m_pSwapChain);

	// needs to release references first

	if( m_pBackBufferRTV ){
		m_pBackBufferRTV->Release();
		m_pBackBufferRTV = nullptr;
	}

	HRESULT hr = m_pSwapChain->ResizeBuffers( m_nBuffers, width_p, height_p, DXGI_FORMAT_UNKNOWN/*m_BackBufferDisplayMode.Format*/, m_iFlags );
	if( FAILED(hr) ){

		assert( DXGI_ERROR_INVALID_CALL );
		throw std::exception("dxgi failed to resize");
	}

	DXGI_SWAP_CHAIN_DESC desc;
	m_pSwapChain->GetDesc( &desc );
	m_BackBufferDisplayMode = desc.BufferDesc;

	// recreate the references
	CreateRTVFromBackBuffer( pDevice_p );
}

void dx::SwapChain::CreateTheSwapChain( /*in: */ ID3D11Device *pDevice_p, IDXGIFactory1 *pFactory_p, const HWND hWnd_p,
	/*reqired */ const BOOL bChangeResolutionToMatchCliRectWhenGoingFullscreen_p, /*in / holded: */ const BOOL bWindowed_p,
	const UINT nMSAA_p, const UINT iMSAAqlty_p, const DXGI_MODE_DESC OutputMode_p,
	/*used also to check MSAA for the format */ const UINT nBuffer_p, IDXGIOutput *pOutput_p /*not used only holded */ )
{
	//create swap chain with the display mode returned when requesting for given features:
	DXGI_SWAP_CHAIN_DESC swDesc;
	swDesc.BufferDesc = m_BackBufferDisplayMode = OutputMode_p;
	swDesc.BufferCount = m_nBuffers = nBuffer_p;//n of buffers(including front, accordingly to the sdk)
	swDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swDesc.Flags = m_iFlags = bChangeResolutionToMatchCliRectWhenGoingFullscreen_p ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : NULL;
	swDesc.OutputWindow = hWnd_p;
	swDesc.SampleDesc.Count = m_nMSAA = nMSAA_p;
	swDesc.SampleDesc.Quality = m_iMSAAQuality = iMSAAqlty_p;
	swDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swDesc.Windowed = m_bWindowed = bWindowed_p;

	HRESULT err = pFactory_p->CreateSwapChain( pDevice_p, &swDesc, &m_pSwapChain );

	if( err != S_OK ){

		assert(err != DXGI_ERROR_INVALID_CALL);
		assert(err != DXGI_ERROR_DEVICE_HUNG);
		assert(err != E_INVALIDARG);

		//case DXGI_STATUS_OCCLUDED..

		throw std::exception("dxgi failed to create swap chain");	
	}

	m_pOutput = pOutput_p;
	//donne(pFactory_p);	

	//denny dxgi to ctrl the hWnd alt+enter(I think only DXGI_MWA_NO_WINDOW_CHANGES have effect ) :
	//	NOTE: prnt screen is only denied when in dx fs( also with DXGI_MWA_NO_WINDOW_CHANGES )
	err = pFactory_p->MakeWindowAssociation( hWnd_p, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN );

	if( err != S_OK ){
		//means DXGIERR_INVALID_CALL(? theres no such a thing) or E_OUTOFMEMORY

		assert (err !=  E_INVALIDARG );

		throw std::exception("dxgi failed to make window association");
	}

	m_pFactory = pFactory_p;
}

bool dx::SwapChain::SwitchFullscreenMode()
{
	HRESULT hr;

	if( m_bWindowed ){

		hr = m_pSwapChain->SetFullscreenState( true, m_pOutput );

		if( FAILED(hr) ) goto FSerrorHandling;

		m_bWindowed = false;
	}
	else{

		hr = m_pSwapChain->SetFullscreenState( false, NULL );

		if( FAILED(hr) ) goto FSerrorHandling;

		m_bWindowed = true;
	}

	// update output, since it may change

	DXGI_SWAP_CHAIN_DESC desc;
	m_pSwapChain->GetDesc( &desc );
	m_BackBufferDisplayMode = desc.BufferDesc;

	// NOTE!!!!
	// even if mode switch is true, and even if the mode changed, the description does not
	// change regarding the scaling!!!

	return true;

FSerrorHandling:

	switch( hr ){

	case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
	case DXGI_STATUS_MODE_CHANGE_IN_PROGRESS:
		return false;
	default:
		std::exception("dxgi fullscreen transition problem");
		return false;
	}
}

int dx::SwapChain::GetCapableAdapterAndOutputAndModeClosestMatch(
	/*in: */
	DXGI_MODE_DESC &desiredFSDisplayMode_p,
	/*in/out: */
	GpuAndMode &gpuAndMode_p, /*in only thepriorities */ /*out: */ IDXGIFactory1 *& pFactory_p )
{
	//check if ColorFormat and resolution are available on any Adapter(vid cards):--------------------------------------
	if( CreateDXGIFactory1( __uuidof(IDXGIFactory1), (void**)&pFactory_p ) != S_OK ){
		throw std::exception("failed to create dxgi factory");
	}

	IDXGIAdapter1 *pAdapter = NULL;
	IDXGIOutput *pAdapterCurrentOutput = NULL;

	std::vector<GpuAndMode> validAdapters;

	HRESULT FOUND = E_FAIL;

	//traverse adapters:
	bool bGotValid = false;
	for(UINT i = 0;
		pFactory_p->EnumAdapters1( i, &pAdapter ) != DXGI_ERROR_NOT_FOUND;
		i++ ){

			for(UINT j = 0;//traverse monitors/displays:
				pAdapter->EnumOutputs( j, &pAdapterCurrentOutput ) != DXGI_ERROR_NOT_FOUND;
				j++ ){

					//check pOutput display mode availability:
					if( !FAILED( FOUND = pAdapterCurrentOutput->FindClosestMatchingMode( &desiredFSDisplayMode_p, &gpuAndMode_p.mode, NULL ))){

						validAdapters.push_back( GpuAndMode(pAdapter, pAdapterCurrentOutput, gpuAndMode_p.mode) );

						bGotValid = true;

						continue;
					}
					pAdapterCurrentOutput->Release();
			}//F traverse outputs
			if( !bGotValid ){
				pAdapter->Release();
			}else
				bGotValid = false;
	}//F traverse adapters

	if( validAdapters.size() == 0 ) return 0;


	// check whats the best match for the required pOutput mode:
	int currentHigScore = 0, itHigScore = 0;
	for( unsigned int it = 0; it < validAdapters.size(); ++ it ){

		if( memcmp( &desiredFSDisplayMode_p, &validAdapters[it].mode, sizeof(DXGI_MODE_DESC) ) == 0 ){

			gpuAndMode_p = validAdapters[it];

			for( unsigned int it2 = 0; it2 < validAdapters.size(); ++it2){
				if( it2 == it ) continue;
				gpuAndMode_p.pAdapter->Release();
				gpuAndMode_p.pOutput->Release();
			}

			return 1;
		}
		else{

			int score = 0;
			if( gpuAndMode_p.modeMatchPriority.iFormat != 0 ){

				if( desiredFSDisplayMode_p.Format == validAdapters[it].mode.Format){

					validAdapters[it].modeMatchPriority.iFormat = gpuAndMode_p.modeMatchPriority.iFormat;
					score +=  gpuAndMode_p.modeMatchPriority.iFormat;
				}
			}
			if( gpuAndMode_p.modeMatchPriority.iResolution != 0 ){

				if(desiredFSDisplayMode_p.Width != validAdapters[it].mode.Width
					|| desiredFSDisplayMode_p.Height != validAdapters[it].mode.Height){

						validAdapters[it].modeMatchPriority.iResolution = gpuAndMode_p.modeMatchPriority.iResolution;
						score +=  gpuAndMode_p.modeMatchPriority.iResolution;
				}
			}
			if( gpuAndMode_p.modeMatchPriority.iScaling != 0 ){

				if( desiredFSDisplayMode_p.Scaling == validAdapters[it].mode.Scaling){

					validAdapters[it].modeMatchPriority.iScaling = gpuAndMode_p.modeMatchPriority.iScaling;
					score +=  gpuAndMode_p.modeMatchPriority.iScaling;
				}
			}
			if( gpuAndMode_p.modeMatchPriority.iScanlineOrder != 0 ){

				if( desiredFSDisplayMode_p.ScanlineOrdering == validAdapters[it].mode.ScanlineOrdering){

					validAdapters[it].modeMatchPriority.iScanlineOrder = gpuAndMode_p.modeMatchPriority.iScanlineOrder;
					score +=  gpuAndMode_p.modeMatchPriority.iScanlineOrder;
				}
			}
			if( gpuAndMode_p.modeMatchPriority.iRefreshRate != 0 ){

				if(memcmp( &desiredFSDisplayMode_p.RefreshRate, &validAdapters[it].mode.RefreshRate, sizeof(DXGI_RATIONAL))==0){

					validAdapters[it].modeMatchPriority.iRefreshRate = gpuAndMode_p.modeMatchPriority.iRefreshRate;
					score +=  gpuAndMode_p.modeMatchPriority.iRefreshRate;
				}
			}

			if( score > currentHigScore ){

				currentHigScore = score;
				itHigScore = it;						
			}
		}
	}

	gpuAndMode_p = validAdapters[itHigScore];

	for( int it2 = 0; it2 < (int)validAdapters.size(); ++it2){
		if( it2 == itHigScore ) continue;
		gpuAndMode_p.pAdapter->Release();
		gpuAndMode_p.pOutput->Release();
	}

	return 2;
}
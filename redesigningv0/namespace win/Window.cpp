#include "Window.h"

#include <exception>

#include "../namespace gen/gen_data.h"

using namespace win;

void win::Window::RegisterWndClass( TCHAR * szClassName_p, HBRUSH bgBrush_p, const HICON hIcon_p, /* application icon(include all sizes in the .ico file) */ UINT style_p )
{
	WNDCLASSEX wcX;

	wcX.lpszClassName	= szClassName_p;
	wcX.hInstance		= m_hAppInstance;
	wcX.hbrBackground	= bgBrush_p;	//NULL; //(HBRUSH)COLOR_BACKGROUND + 1;(NULL cause now Im proceessing wm_erasebg
	wcX.hIcon			= hIcon_p;
	wcX.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wcX.lpszMenuName	= NULL;
	wcX.style			= style_p;	//CS_PARENTDC; //|CS_HREDRAW|CS_VREDRAW;//|CS_CLASSDC X multithread, I gotta test//HREDRAW/VREDRAW causes flickering due erasebg(but only at the second window????)
	wcX.cbClsExtra		= 0;
	wcX.cbWndExtra		= 0;
	wcX.lpfnWndProc		= WndProcProxy;

	wcX.cbSize			= sizeof( WNDCLASSEX );
	wcX.hIconSm			= NULL; //will search from wcX.hIcon

	if( !RegisterClassEx( &wcX ) ){

		DBG(WMMERROR( GetLastError() );)
			throw std::exception("register wnd class fail");
	}
}

bool win::Window::Create( Rect & cliRect_p, /* desired client rect */ const TCHAR * szRegisteredWndClassName_p, HWND hParent, /* OPT parent */ const TCHAR *szTitle_p, /* displayed name in the caption bar */ const DWORD dwWS_p, const DWORD dwWSEX_p )
{
	assert(m_hWnd==NULL);

	// adjust a window RECT to conform with desired client RECT

	m_cliRect = cliRect_p; // save

	RECT adjustRect;
	m_cliRect.ToRECT( adjustRect );
	TESTBOOLNULL(AdjustWindowRectEx( &adjustRect, dwWS_p, false, dwWSEX_p ));

	m_rect.FromRECT( adjustRect ); // save


	// create the window

	//m_dwWS = dwWS_p|WS_CLIPCHILDREN; // save // clipchildren prevent parent updating over child, preventing further WM_PAINT to childs

	m_hWnd = CreateWindowEx(	dwWSEX_p,
		szRegisteredWndClassName_p,
		szTitle_p,
		dwWS_p,
		m_rect.x, m_rect.y, m_rect.w, m_rect.h,
		hParent, NULL,
		m_hAppInstance,
		(LPVOID)this ); // pass this as user data

	TESTBOOLNULL(m_hWnd);

	if( !m_hWnd ) return false;//throw std::exception("create wnd fail");

	//SetWindowLongPtr( m_hWnd, GWLP_USERDATA, (LONG)this );
	// returns last set value, witch can be 0 (so setlasterror to zero to test)


	//save the rest of window attributes

	m_dwWS = dwWS_p;
	m_dwWSEX = dwWSEX_p;
	m_szTitle_p = (TCHAR*)szTitle_p;


	DBG(
		RECT test;
	GetClientRect( m_hWnd, &test );
	)

		DBG(
		HWND hParentTest = GetParent( m_hWnd );
	hParentTest = hParentTest;
	BREAKHERE;
	)

		return true;
}

bool win::Window::Resize( int ClientW_p, int ClientH_p )
{
	// get rect to accommodate client area

	Rect newClientRect_p( m_cliRect );
	newClientRect_p.AnchoredResize( ClientW_p, ClientH_p, gen::Anchor(gen::EHORZALIGN_CENTER, gen::EVERTALIGN_CENTER));

	RECT adjustRect;
	newClientRect_p.ToRECT(adjustRect);
	TESTBOOLNULL(AdjustWindowRectEx( &adjustRect, m_dwWS, m_bMenu, m_dwWSEX ));

	// resize

	Rect NewRect(adjustRect);


	if( !SetWindowPos(	m_hWnd,
		NULL,
		NewRect.x, NewRect.y, NewRect.w, NewRect.h,
		SWP_NOZORDER ) ) return false;

	// save new params

	m_cliRect = newClientRect_p;
	m_rect = NewRect;

	DBG(
		RECT test;
	GetClientRect(m_hWnd, &test );
	)

		return true;
}

bool win::Window::MoveTo( int ClientX_p, int ClientY_p )
{
	// get rect to accommodate client area

	Rect newClientRect_p( ClientX_p, ClientY_p, m_cliRect.w, m_cliRect.h );

	RECT adjustRect;
	newClientRect_p.ToRECT(adjustRect);
	TESTBOOLNULL(AdjustWindowRectEx( &adjustRect, m_dwWS, m_bMenu, m_dwWSEX ));

	// reposition

	Rect NewRect(adjustRect);

	if( !SetWindowPos(	m_hWnd,
		NULL,
		NewRect.x, NewRect.y, NewRect.w, NewRect.h,
		SWP_NOZORDER|SWP_NOSIZE ) ) return false;

	// save new params
	m_cliRect = newClientRect_p;
	m_rect = NewRect;

	DBG(
		RECT test;
	GetClientRect(m_hWnd, &test );
	)

		return true;
}

bool win::Window::ChangeStyle( DWORD dwWS_p, DWORD dwWSEX_p )
{
	// ws

	if(	!SetWindowLongPtr( m_hWnd, GWL_STYLE, dwWS_p ) ){

		DBG(WMMERROR( GetLastError() );)
			return false;
	}

	m_dwWS = dwWS_p;

	// wsex

	if(	!SetWindowLongPtr( m_hWnd, GWL_EXSTYLE, dwWSEX_p ) ){

		DBG(WMMERROR( GetLastError() );)
			return false;
	}

	m_dwWSEX = dwWSEX_p;

	return true;
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/05
	created:	5:2:2013   13:15
	filename: 	c:\Users\Gateway\Documents\Visual Studio 2010\Projects\redesigningv0\redesigningv0\Window.h
	file path:	c:\Users\Gateway\Documents\Visual Studio 2010\Projects\redesigningv0\redesigningv0
	file base:	Window
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	Win32 window, handles window creation and message handling (trough overriding).

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

// system/standard headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

// private headers

#include "../namespace gen/gen_macros.h"
#include "win_genData.h"
#include "win_macros.h"


namespace win{

	class Window{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		Window( HINSTANCE hAppInstance_p )
			: m_hWnd(NULL), m_szTitle_p(NULL), m_hAppInstance(hAppInstance_p), m_bMenu(false){};

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~Window(){};

		//------------------------------------------------------------------------
		// Registers a WndClass (common properties shared among wnds of same class)
		//------------------------------------------------------------------------
		void RegisterWndClass(  TCHAR * szClassName_p,
			HBRUSH bgBrush_p,
			const HICON hIcon_p,		// application icon(include all sizes in the .ico file)
			UINT style_p								) const;

		//------------------------------------------------------------------------
		// Creates a Win32 window, initializing m_hWnd
		//------------------------------------------------------------------------
		bool Create(	Rect & cliRect_p,							// desired client rect
			const TCHAR * szRegisteredWndClassName_p,
			HWND hParent,								// OPT parent
			const TCHAR *szTitle_p,						// displayed name in the caption bar
			const DWORD dwWS_p, const DWORD dwWSEX_p );

		//------------------------------------------------------------------------
		// Resizes the window, anchor centralized
		//------------------------------------------------------------------------
		bool Resize( int ClientW_p, int ClientH_p );

		//------------------------------------------------------------------------
		// Repositions the window
		//------------------------------------------------------------------------
		bool MoveTo( int ClientX_p, int ClientY_p );

		//------------------------------------------------------------------------
		// change monitor resolution
		//------------------------------------------------------------------------
		bool ChangeResolution( UINT W_p, UINT H_p );
		bool ResetMonitorResolution();

		//------------------------------------------------------------------------
		// Updates the window style
		//------------------------------------------------------------------------
		bool ChangeStyle( DWORD dwWS_p, DWORD dwWSEX_p );

		//------------------------------------------------------------------------
		// Set window visibility
		//------------------------------------------------------------------------
		inline void SetVisibility( bool bVisible_p){

			ShowWindow( m_hWnd, bVisible_p ? SW_SHOW : SW_HIDE );
		}

		//------------------------------------------------------------------------
		// send close message
		//------------------------------------------------------------------------
		inline void Close(){

			PostMessage( m_hWnd, WM_CLOSE, 0, 0 );
			//SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
		}

		//------------------------------------------------------------------------
		// Set window "state" functions
		//------------------------------------------------------------------------
		inline void Minimize(){
			ShowWindow( m_hWnd, SW_MINIMIZE );
		}
		inline void Maximize(){
			ShowWindow( m_hWnd, SW_MAXIMIZE );
		}
		inline void Restore(){
			ShowWindow( m_hWnd, SW_RESTORE );
		}

		//------------------------------------------------------------------------
		// Active window is THE top level one (on the z order) that have focus
		// (like if its selected, the one that gets a colored caption/title and a
		// border thing when the user clicks on it)
		//------------------------------------------------------------------------
		bool IsActive(){
			return (GetActiveWindow() == m_hWnd);
		}

		//------------------------------------------------------------------------
		// Sets a menu bar to the window
		//------------------------------------------------------------------------
		bool SetMenuBar( HMENU hMenu_p ){

			if( !SetMenu( m_hWnd, hMenu_p ) ){

				DBG(WMMERROR( GetLastError()));

				return false;
			}

			m_bMenu = true;

			return true;
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		inline HWND GetHWND(){ return m_hWnd;}
		inline const TCHAR* GetTitle(){ return m_szTitle_p; }
		inline Rect GetCLientRect(){ return m_cliRect; }
		inline Rect GetWindowRect(){ return m_rect; }

		//------------------------------------------------------------------------
		// setters
		//------------------------------------------------------------------------
		inline void UpdateTitle( const TCHAR * szNewTitle_p ){

			TESTBOOLNULL(SetWindowText( m_hWnd, szNewTitle_p ));

			m_szTitle_p = szNewTitle_p;
		}

	protected:

		HWND m_hWnd;

		const TCHAR * m_szTitle_p;		
		Rect m_rect, m_cliRect;
		DWORD m_dwWS, m_dwWSEX;
		bool m_bMenu;

		HINSTANCE m_hAppInstance;	// dunno

		Window(); // denied ctor

		//------------------------------------------------------------------------
		// Override to handle windows messages
		//------------------------------------------------------------------------
		virtual LRESULT CALLBACK WndProcHandler( HWND hWnd_p, UINT Msg_p, WPARAM wParam_p, LPARAM lParam_p ) = 0;

		//------------------------------------------------------------------------
		// call this when the HWND changes to automatically set the rects
		//------------------------------------------------------------------------
		void UpdateRects(){

				RECT newRect;
				::GetWindowRect( m_hWnd, &newRect );
				m_rect.FromRECT( newRect );

				::GetClientRect( m_hWnd, &newRect );
				m_cliRect.FromRECT( newRect );

				// NOTE: cli rect returned never contains x, y, its always 0
				POINT cliPos = {0};
				ClientToScreen( m_hWnd, &cliPos );
				//
				m_cliRect.x = cliPos.x;
				m_cliRect.y = cliPos.y;		
			}	

	private:

		//------------------------------------------------------------------------
		// Message Handler
		// Delegates to the virtual wnd proc
		//------------------------------------------------------------------------

		static LRESULT CALLBACK WndProcProxy( HWND hWnd_p, UINT Msg_p, WPARAM wParam_p, LPARAM lParam_p ){

			//assert(hWnd_p);

			Window * pThis = (Window*)GetWindowLongPtr( hWnd_p, GWLP_USERDATA );

			if( pThis ){

				return pThis->WndProcHandler( hWnd_p, Msg_p, wParam_p, lParam_p );
			}
			else if( Msg_p == WM_NCCREATE ){

				// initialize wnd handle
				((Window*)
					((CREATESTRUCT*)lParam_p)->lpCreateParams
				)->m_hWnd = hWnd_p;

				// set this pointer (given on CreateWindow) on windows user data
				SetWindowLongPtr( hWnd_p, GWLP_USERDATA, (LONG) ((CREATESTRUCT*)lParam_p)->lpCreateParams );
				// returns last set value, witch can be 0 (so setlasterror to zero to test)


				// NOTE:
				// if return 1, the CAPTION IS NOT SET!!!!!!!!!!!!!!!!
				// WINDOWS BUG
				SetWindowText( hWnd_p, ((CREATESTRUCT*)lParam_p)->lpszName );

				return 1; // if return 0, it cancel the window creation
			}
			else
				return DefWindowProc( hWnd_p, Msg_p, wParam_p, lParam_p );
		}	
	};
}
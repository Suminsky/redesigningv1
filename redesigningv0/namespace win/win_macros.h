//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/05
	created:	5:2:2013   15:45
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace win\win_macros.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace win
	file base:	win_macros
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	windows macros

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

#include <assert.h>

// message Box Stuff

#define MB( cap, txt, mb ) MessageBox( NULL, TEXT( txt ), TEXT (cap), mb )	// default messagebox
#define _MB( cap, bfr, mb) MessageBox( NULL, bfr, TEXT (cap), mb )			// default messagebox for srting buffer
#define _MBA( cap, bfr, mb) MessageBoxA(NULL, (LPCSTR)bfr, cap, mb)			// mb for ansi string buffer
#define MB_E ( MB_OK|MB_ICONEXCLAMATION )	//MB exclamation
#define MB_I ( MB_OK|MB_ICONINFORMATION )	//MB info

// automatic error MB

#define WMMERROR(errorID){	TCHAR szBuffer[1024]; \
	FormatMessage(	FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)errorID, NULL, \
	szBuffer, 1024, \
	NULL ); \
	_MB( "Wnd Error", szBuffer, MB_E );}

#define WMMERRORA(errorID){	CHAR szBuffer[1024]; \
	FormatMessageA(	FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)errorID, NULL, \
	szBuffer, 1024, \
	NULL ); \
	_MB( "Wnd Error", szBuffer, MB_E );}


// test for functions and display error msg, dbg only

#ifdef _DEBUG
#define TESTBOOLNULL(boolNullFunc){ if( ! boolNullFunc ){\
	WMMERROR( GetLastError() );\
	assert( false );}  }

#define TESTNEGONE(intFunc){	if( intFunc == -1 ){\
	WMMERROR( GetLastError() );\
	assert( false );} }

#else

#define TESTBOOLNULL(boolNullFunc) boolNullFunc
#define TESTNEGONE(intFunc) intFunc

#endif


// adjust rect stuff

#ifdef _DEBUG
#define ADJUSTRECTTOCREATEWINDOW( rect, style, styleEx, bMenu ){	rect.right += rect.left; \
																	rect.bottom += rect.top; \
																	assert( AdjustWindowRectEx(	&rect, \
																	style, \
																	bMenu, \
																	styleEx ) ); \
																	rect.right = rect.right - rect.left; \
																	rect.bottom = rect.bottom - rect.top;	}
#else
#define ADJUSTRECTTOCREATEWINDOW( rect, style, styleEx, bMenu ){	rect.right += rect.left; \
																	rect.bottom += rect.top; \
																	AdjustWindowRectEx(	&rect, \
																	style, \
																	bMenu, \
																	styleEx ); \
																	rect.right = rect.right - rect.left; \
																	rect.bottom = rect.bottom - rect.top;	}
#endif
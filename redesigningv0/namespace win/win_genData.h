#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include "win_macros.h"
#include "../namespace gen/gen_data.h"

namespace win{

	//------------------------------------------------------------------------
	// this allocates mem!
	//------------------------------------------------------------------------
	inline bool OpenFile( const char * szFilename_p, gen::DataStream & buffer_p  )
	{
		HANDLE hFile = CreateFileA( szFilename_p,
									GENERIC_READ, 0, NULL, OPEN_EXISTING,
									FILE_FLAG_SEQUENTIAL_SCAN|FILE_ATTRIBUTE_NORMAL,
									NULL );

		if( hFile == INVALID_HANDLE_VALUE ){

			return false;
		}

		DWORD dwBytesRead = 0;

		LARGE_INTEGER size;
		TESTBOOLNULL(GetFileSizeEx( hFile, &size ));

		buffer_p.m_data = new BYTE[(size_t)size.QuadPart];
		buffer_p.m_size = (int)size.QuadPart;

		TESTBOOLNULL(ReadFile( hFile, (LPVOID)buffer_p.m_data, buffer_p.m_size, &dwBytesRead, NULL ));

		TESTBOOLNULL(CloseHandle( hFile));

		buffer_p.m_currentByteIndex = 0;

		return true;
	}

	inline static bool SaveToFile( const char * szFilename_p,  gen::DataStream & buffer_p  ){

		HANDLE hFile = CreateFileA( szFilename_p, GENERIC_WRITE, 0,
									NULL, OPEN_ALWAYS,
									FILE_FLAG_SEQUENTIAL_SCAN|FILE_ATTRIBUTE_NORMAL,
									NULL );

		if( hFile == INVALID_HANDLE_VALUE ){

			return false;
		}
		if( GetLastError() == ERROR_ALREADY_EXISTS ){

			// so "delete"(truncate its size, the file pointer of the handle is used) it:
			TESTBOOLNULL(SetEndOfFile( hFile ));
		}

		OVERLAPPED fileOffset;
		fileOffset.Offset = fileOffset.OffsetHigh = 0xFFFFFFFF; // == append data
		fileOffset.hEvent = NULL;
		fileOffset.Internal = fileOffset.InternalHigh = NULL;

		DWORD nBytesWritten = 0;

		TESTBOOLNULL(WriteFile( hFile, (LPCVOID)buffer_p.m_data, buffer_p.m_size, (LPDWORD)&nBytesWritten, &fileOffset ));
		TESTBOOLNULL(CloseHandle( hFile ));

		return true;
	}

	//------------------------------------------------------------------------
	// Rect by pos and res (to avoid the misuse of RECT)
	// x, y, w and h always means x, y, w and h!
	//------------------------------------------------------------------------
	struct Rect{

		int x,y,w,h;

		Rect(){};
		Rect( int x_p, int y_p, int w_p, int h_p ):x(x_p), y(y_p), w(w_p), h(h_p){}

		// facilities

		Rect( const RECT & rect_p ){

			x = rect_p.left;
			y = rect_p.top;
			w = rect_p.right - x;
			h = rect_p.bottom - y;
		}

		inline Rect & FromRECT( const RECT & rect_p ){

			x = rect_p.left;
			y = rect_p.top;
			w = rect_p.right - x;
			h = rect_p.bottom - y;

			return *this;
		}

		inline void ToRECT( RECT & newRect_p ){

			newRect_p.left = x;
			newRect_p.top = y;
			newRect_p.right = w + x;			
			newRect_p.bottom = h + y;
		}
		inline RECT ToRECT(){

			RECT rect;
			ToRECT( rect );
			return rect;
		}

		inline void AnchoredResize( int w_p, int h_p, gen::Anchor anchor_p ){

			// cache

			int xdif = (w_p - w);
			int ydif = (h_p - h);

			// horz

			switch( anchor_p.h ){

			//case gen::EHORZALIGN_LEFT:

			case gen::E_HORZALIGN_RIGHT:

				x = x - xdif;

				break;
			case gen::E_HORZALIGN_CENTER:

				x = x - (int)(xdif * 0.5f);

				break;
			}

			// vert

			switch( anchor_p.h ){

			//case gen::EVERTALIGN_TOP:

			case gen::E_VERTALIGN_BOTTOM:

				y = y - ydif;

				break;
			case gen::E_VERTALIGN_CENTER:

				y = y - (int)(ydif * 0.5f);

				break;
			}

			w = w_p;
			h = h_p;
		}
	};
}
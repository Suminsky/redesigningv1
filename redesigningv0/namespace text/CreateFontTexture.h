#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/07
	created:	7:7:2013   18:37
	file:		CreateBmpFontFile.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	just testing

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma comment(lib, "gdiplus.lib")

// standard includes
#include <Windows.h>
#include <gdiplus.h>

// private includes
#include "../namespace render/dx/Device.h"

namespace text{

	struct GlyphRect{

		float X;
		float Y;
		float Width;
		float Height;
	};

	struct BmpFontDesc{

		const char	*	szTextureFilename;
		int				nCharacteres;
		WCHAR		*	characteresIDs;
		GlyphRect	*	charUVRects;
		float			fSpaceWidth;
		float			fNewLineMinHeight;
		int				iTextureWidth;
		int				iTextureHeight;
	};

	struct MorpherUnion{

		unsigned int SIZE;
		unsigned char *m_data;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		TYPE GetChunkAs( unsigned int iByteOffset_p ){

			assert( iByteOffset_p + sizeof(TYPE) <= SIZE );

			return   *((TYPE*) (&(m_data[iByteOffset_p])) );
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		void SetChunkAs( unsigned int iByteOffset_p, TYPE newData_p ){

			assert( iByteOffset_p + sizeof(TYPE) <= SIZE );

			*((TYPE*) (&(m_data[iByteOffset_p])) ) = newData_p;
		}
	};

	//========================================================================
	// creates a texture and a fnt with describes the font chars
	//========================================================================
	bool CreateFontFiles(	const char* szFilenmae_p,
							LPCWSTR szFontNamePlusFamily_p, float fFontSize_p, UINT iFontStyle_p,
							bool bAntiAliased_p,
							dx::Device & device_p,
							GlyphRect * charDescs_out_p,	const WCHAR * szCharsToRaster_p, const UINT nChars_p,
							const UINT iTextureWidth_p = 1024 );

	//========================================================================
	// populates fontDesc with data gathered from given file
	//========================================================================
	bool ReadFontDescFromFile( const char * szFontDescFilename_p, BmpFontDesc & fontDesc_p );
}
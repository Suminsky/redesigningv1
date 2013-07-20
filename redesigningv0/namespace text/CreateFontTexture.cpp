#include "CreateFontTexture.h"

using namespace Gdiplus;

bool text::CreateFontFiles(	const char* szFilenmae_p,
							LPCWSTR szFontNamePlusFamily_p, float fFontSize_p, UINT iFontStyle_p,
							bool bAntiAliased_p,
							dx::Device & device_p,
							GlyphRect * charDescs_out_p,	const WCHAR * szCharsToRaster_p, const UINT nChars_p,
							const UINT iTextureWidth_p /*= 1024*/ ){
	// Init GDI+

	ULONG_PTR token = NULL;

	{

	GdiplusStartupInput startupInput( NULL, TRUE, TRUE );
	GdiplusStartupOutput startupOutput;
	if( GdiplusStartup( &token, &startupInput, &startupOutput ) != Gdiplus::Ok ) return false;

	// Create the font

	Gdiplus::Font font( szFontNamePlusFamily_p, fFontSize_p, iFontStyle_p, UnitPixel, NULL );
	if( font.GetLastStatus() != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	// Create a temporary Bitmap and Graphics for figuring out the rough size required
	// for drawing all of the characters
		
	int size = static_cast<int>( fFontSize_p * nChars_p * 2 ) + 1;
	Bitmap sizeBitmap(size, size, PixelFormat32bppARGB);

	if( sizeBitmap.GetLastStatus() ){ GdiplusShutdown(token);return false; }

	Graphics sizeGraphics( &sizeBitmap );
	if( sizeGraphics.GetLastStatus() != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	TextRenderingHint hint = bAntiAliased_p ? TextRenderingHintAntiAliasGridFit : TextRenderingHintSingleBitPerPixelGridFit;

	if( sizeGraphics.SetTextRenderingHint( hint ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	float charHeight = font.GetHeight( &sizeGraphics );// * 1.5f;

	RectF sizeRect;
	if( sizeGraphics.MeasureString( szCharsToRaster_p, -1, &font, PointF(0, 0), &sizeRect ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }
	int numRows = static_cast<int>( sizeRect.Width / iTextureWidth_p ) + 1;
	int iTextureHeight = static_cast<int>( numRows * charHeight ) + 1;

	// Create a temporary Bitmap and Graphics for drawing the characters one by one

	int tempSize = static_cast<int>( fFontSize_p * 2 );
	Bitmap drawSingleCharBitmap( tempSize, tempSize, PixelFormat32bppARGB );
	if( drawSingleCharBitmap.GetLastStatus() != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	Graphics drawSingleCharGraphics(&drawSingleCharBitmap);
	if( drawSingleCharGraphics.GetLastStatus() != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }
	if( drawSingleCharGraphics.SetTextRenderingHint(hint) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	// Create a temporary Bitmap + Graphics for creating a full character set

	Bitmap textureBitmap ( iTextureWidth_p, iTextureHeight, PixelFormat32bppARGB );
	if( textureBitmap.GetLastStatus() != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	Graphics textureGraphics (&textureBitmap);
	if( textureGraphics.GetLastStatus() != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }
	if( textureGraphics.Clear( Color(0, 255, 255, 255) ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }
	if( textureGraphics.SetCompositingMode( CompositingModeSourceCopy ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	// Solid brush for text rendering

	SolidBrush brush( Color(255, 255, 255, 255) );
	if( brush.GetLastStatus() != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	// Draw all of the characters, and copy them to the full character set

	WCHAR charString [2];
	charString[1] = 0;
	int currentX = 0;
	int currentY = 0;

	//LANGID language = MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN );
	//StringFormat format( 0, language );
	
	for(UINT64 i = 0; i < nChars_p; ++i){

		charString[0] = szCharsToRaster_p[i];

		// Draw the character

		if( drawSingleCharGraphics.Clear( Color(0, 255, 255, 255) ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }
		if( drawSingleCharGraphics.DrawString( charString, 1, &font, PointF(0, 0),/* &format,*/ &brush ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

		// Figure out the amount of blank space before the character

		int minX = 0;
		for(int x = 0; x < tempSize; ++x){
			for(int y = 0; y < tempSize; ++y){

				Color color;
				if( drawSingleCharBitmap.GetPixel( x, y, &color ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }
				
				if(color.GetAlpha() > 0){

					minX = x;
					x = tempSize;
					break;
				}
			}
		}

		// Figure out the amount of blank space after the character

		int maxX = tempSize - 1;
		for(int x = tempSize - 1; x >= 0; --x){
			for(int y = 0; y < tempSize; ++y){

				Color color;
				if( drawSingleCharBitmap.GetPixel(x, y, &color) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }
				if(color.GetAlpha() > 0){

					maxX = x;
					x = -1;
					break;
				}
			}
		}

		//////========================================================================
		////// 
		//////========================================================================
		////int minY = 0;
		////for(int y = 0; y < tempSize; ++y){
		////	for(int x = 0; x < tempSize; ++x){

		////		Color color;
		////		if( drawSingleCharBitmap.GetPixel( x, y, &color ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

		////		if(color.GetAlpha() > 0){

		////			minY = y;
		////			y = tempSize;
		////			break;
		////		}
		////	}
		////}

		////// Figure out the amount of blank space after the character

		////int maxY = tempSize - 1;
		////for(int y = tempSize - 1; y >= 0; --y){
		////	for(int x = 0; x < tempSize; ++x){

		////		Color color;
		////		if( drawSingleCharBitmap.GetPixel(x, y, &color) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }
		////		if(color.GetAlpha() > 0){

		////			maxY = y;
		////			y = -1;
		////			break;
		////		}
		////	}
		////}

		int charWidth = maxX - minX + 1;
		////int charH = maxY - minY + 1;

		// Figure out if we need to move to the next row

		if (currentX + charWidth >= (int)iTextureWidth_p){

			currentX = 0;
			currentY += static_cast<int>(charHeight + 0.5f);// + 1;
		}

		// Fill out the structure describing the character position

		charDescs_out_p[i].X =		(float)currentX /  (float)iTextureWidth_p;
		charDescs_out_p[i].Y =		(float)currentY /  (float)iTextureHeight;
		charDescs_out_p[i].Width =  (float)charWidth /  (float)iTextureWidth_p;
		charDescs_out_p[i].Height = (float)charHeight / (float)iTextureHeight;

		// Copy the character over

		int height = static_cast<int>(charHeight + 0.5f);
		if( textureGraphics.DrawImage( &drawSingleCharBitmap, currentX, currentY, minX, 0, charWidth, height, UnitPixel ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

		////charDescs_out_p[i].Y =		(float)(minY + currentY) /  (float)iTextureHeight;
		////charDescs_out_p[i].Height =  (float)charH /  (float)iTextureHeight;

		currentX += charWidth + 1;
	}

	// Figure out the width of a space character

	charString[0] = ' ';
	charString[1] = 0;
	if( drawSingleCharGraphics.MeasureString( charString, 1, &font, PointF(0, 0), &sizeRect ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	////float spaceWidth = sizeRect.Width;

	// Lock the bitmap for direct memory access

	BitmapData bmData;
	Rect rect(0, 0, iTextureWidth_p, iTextureHeight);
	if( textureBitmap.LockBits( &rect, ImageLockModeRead, PixelFormat32bppARGB, &bmData ) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	dx::Texture2DResource::CreationParams tex2Dparams;

	// create a D3D texture, initialized with the bitmap data

	tex2Dparams.desc.tex2Ddesc.Width = iTextureWidth_p;
	tex2Dparams.desc.tex2Ddesc.Height = iTextureHeight;
	tex2Dparams.desc.tex2Ddesc.MipLevels = 1;
	tex2Dparams.desc.tex2Ddesc.ArraySize = 1;
	tex2Dparams.desc.tex2Ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex2Dparams.desc.tex2Ddesc.SampleDesc.Count = 1;
	tex2Dparams.desc.tex2Ddesc.SampleDesc.Quality = 0;
	tex2Dparams.desc.tex2Ddesc.Usage = D3D11_USAGE_DYNAMIC;
	tex2Dparams.desc.tex2Ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex2Dparams.desc.tex2Ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tex2Dparams.desc.tex2Ddesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = bmData.Scan0;
	data.SysMemPitch = iTextureWidth_p * 4;
	data.SysMemSlicePitch = 0;

	tex2Dparams.pInitialData = &data;
	ID3D11Texture2D * pTex2D;

	tex2Dparams.desc.szFilename = szFilenmae_p;
	device_p.m_pCacheTex2D->Acquire( tex2Dparams, pTex2D );

	if( textureBitmap.UnlockBits(&bmData) != Gdiplus::Ok ){ GdiplusShutdown(token);return false; }

	tex2Dparams.desc.szFilename += ".png";

	HRESULT hr = D3DX11SaveTextureToFileA( device_p.GetContext(), pTex2D, D3DX11_IFF_PNG, tex2Dparams.desc.szFilename.c_str() );
	if( FAILED(hr) ){

		return false;
	}

	tex2Dparams.desc.szFilename = szFilenmae_p;
	tex2Dparams.desc.szFilename += ".fnt";
	HANDLE hFile = CreateFileA( tex2Dparams.desc.szFilename.c_str(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN|FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE ){

		throw std::exception("could not create file");
	}
	if( GetLastError() == ERROR_ALREADY_EXISTS ){

		// so "delete"(truncate its size, the file pointer of the handle is used) it:
		SetEndOfFile( hFile );
	}

	OVERLAPPED fileOffset;
	fileOffset.Offset = fileOffset.OffsetHigh = 0xFFFFFFFF; // == append data
	fileOffset.hEvent = NULL;
	fileOffset.Internal = fileOffset.InternalHigh = NULL;

	BmpFontDesc fontDesc;
	tex2Dparams.desc.szFilename = szFilenmae_p;
	tex2Dparams.desc.szFilename += ".png";
	fontDesc.szTextureFilename = tex2Dparams.desc.szFilename.c_str();
	fontDesc.nCharacteres = nChars_p;
	fontDesc.characteresIDs = (WCHAR*)szCharsToRaster_p;
	fontDesc.charUVRects = charDescs_out_p;
	fontDesc.fSpaceWidth = sizeRect.Width;
	fontDesc.fNewLineMinHeight = charHeight;
	fontDesc.iTextureWidth = iTextureWidth_p;
	fontDesc.iTextureHeight = iTextureHeight;

	DWORD nBytesWritten = 0;

	int szLengh = (int)strlen(fontDesc.szTextureFilename) +1;
	WriteFile( hFile, (LPCVOID)&szLengh,					sizeof(int),							(LPDWORD)&nBytesWritten, &fileOffset );
	WriteFile( hFile, (LPCVOID)fontDesc.szTextureFilename,	szLengh,								(LPDWORD)&nBytesWritten, &fileOffset );
	WriteFile( hFile, (LPCVOID)&fontDesc.nCharacteres,		sizeof(int),							(LPDWORD)&nBytesWritten, &fileOffset );
	WriteFile( hFile, (LPCVOID)fontDesc.characteresIDs,		sizeof(WCHAR)*fontDesc.nCharacteres,	(LPDWORD)&nBytesWritten, &fileOffset );
	assert( sizeof(GlyphRect) == sizeof(float)*4 );
	WriteFile( hFile, (LPCVOID)fontDesc.charUVRects,		sizeof(GlyphRect)*fontDesc.nCharacteres,(LPDWORD)&nBytesWritten, &fileOffset );
	WriteFile( hFile, (LPCVOID)&fontDesc.fSpaceWidth,		sizeof(float),							(LPDWORD)&nBytesWritten, &fileOffset );
	WriteFile( hFile, (LPCVOID)&fontDesc.fNewLineMinHeight,	sizeof(float),							(LPDWORD)&nBytesWritten, &fileOffset );
	WriteFile( hFile, (LPCVOID)&fontDesc.iTextureWidth,		sizeof(int),							(LPDWORD)&nBytesWritten, &fileOffset );
	WriteFile( hFile, (LPCVOID)&fontDesc.iTextureHeight,	sizeof(int),							(LPDWORD)&nBytesWritten, &fileOffset );

	CloseHandle( hFile );
	}

	GdiplusShutdown(token);

	return true;
}


bool text::ReadFontDescFromFile( const char * szFontDescFilename_p, BmpFontDesc & fontDesc_p ){

	HANDLE hFile = CreateFileA( szFontDescFilename_p,
								GENERIC_READ, 0, NULL, OPEN_EXISTING,
								FILE_FLAG_SEQUENTIAL_SCAN|FILE_ATTRIBUTE_NORMAL,
								NULL );

	if( hFile == INVALID_HANDLE_VALUE ){

		//throw std::exception("could not open fnt file");
		return false;
	}

	DWORD dwBytesRead = 0;

	LARGE_INTEGER size;
	GetFileSizeEx( hFile, &size );

	MorpherUnion data;
	data.m_data = new BYTE[(size_t)size.QuadPart];
	data.SIZE = (int)size.QuadPart;
	ReadFile( hFile, (LPVOID)data.m_data, data.SIZE, &dwBytesRead, NULL );
	CloseHandle( hFile);

	int iByteIndex = 0;

	// read texture name lenght

	int nNameLen = data.GetChunkAs<int>(iByteIndex);
	fontDesc_p.szTextureFilename = new char[nNameLen];
	iByteIndex += sizeof(int);

	// read texture name

	memcpy( (void*)fontDesc_p.szTextureFilename, &data.m_data[iByteIndex], nNameLen );
	iByteIndex += nNameLen;

	// read n of characters

	fontDesc_p.nCharacteres = data.GetChunkAs<int>(iByteIndex);
	iByteIndex += sizeof(int);

	fontDesc_p.characteresIDs = new WCHAR[fontDesc_p.nCharacteres];

	// read characters

	memcpy( (void*)fontDesc_p.characteresIDs,  &data.m_data[iByteIndex], fontDesc_p.nCharacteres*sizeof(WCHAR) );
	iByteIndex += fontDesc_p.nCharacteres*sizeof(WCHAR);

	// read glyph rects

	assert( sizeof(GlyphRect) == sizeof(float)*4 );
	fontDesc_p.charUVRects = new GlyphRect[fontDesc_p.nCharacteres];

	memcpy( (void*)fontDesc_p.charUVRects,  &data.m_data[iByteIndex], fontDesc_p.nCharacteres*sizeof(GlyphRect));
	iByteIndex += fontDesc_p.nCharacteres*sizeof(GlyphRect);

	// read space width

	fontDesc_p.fSpaceWidth = data.GetChunkAs<float>(iByteIndex);
	iByteIndex += sizeof(float);

	// read new line height

	fontDesc_p.fNewLineMinHeight = data.GetChunkAs<float>(iByteIndex);
	iByteIndex += sizeof(float);

	// read texture wiidth:

	fontDesc_p.iTextureWidth = data.GetChunkAs<int>(iByteIndex);
	iByteIndex += sizeof(int);

	// read texture height:

	fontDesc_p.iTextureHeight = data.GetChunkAs<int>(iByteIndex);


	delete data.m_data;

	return true;
}
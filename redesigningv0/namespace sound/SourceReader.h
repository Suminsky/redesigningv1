#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/23
	created:	23:6:2013   19:50
	file:		SourceReader.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	Encapsulates MF source reader, responsible for reading and decoding media formats.
				TODO: error codes (enum?)

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// needed libs

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#pragma comment(lib, "Propsys.lib")

// standard includes

#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>

#include <memory>

// private includes

namespace sound{

	class SourceReader{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SourceReader()
			:
		m_pReader(nullptr)
		{}
		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~SourceReader(){

			if( m_pReader ) m_pReader->Release();			
		}

		//------------------------------------------------------------------------
		// Open the given file for reading.
		// Initializes basic info as format and size.
		// 
		// NOTE: I m not initializing MF with the "full" flag, with means (i guess)
		// it doesn t initializes socket stuff. Thats why this class will only work
		// with files, and not with streams from network URLs as it is supposed to.
		//------------------------------------------------------------------------
		bool Initialize( const wchar_t * szFilename_p );

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		WAVEFORMATEX & GetWaveFormat(){ return m_waveFormat; }
		LONGLONG GetDurationMS(){ return m_llDuration_ms; }
		UINT GetMaxSize(){ return m_maxSize; }

		//------------------------------------------------------------------------
		// Reads the entire audio data to the given pointer.
		//------------------------------------------------------------------------
		bool ReadAllWaveData( BYTE * pData_p, UINT nMaxBytesToWrite_p, UINT & nBytesWritten_p );

		//------------------------------------------------------------------------
		// Reads the next sample to the given pointer.
		// Returns true only if all media was read.
		// UNTESTED (TODO)
		//------------------------------------------------------------------------
		bool ReadNextWaveSample( BYTE * pData_p, UINT nMaxBytesToWrite_p, UINT & nBytesWritten_p );

		//------------------------------------------------------------------------
		// Returns "seek pointer" to 0, so it can start reading samples from
		// beginning again.
		//------------------------------------------------------------------------
		bool ResetReadingPosition(){

			PROPVARIANT var = {0};
			var.vt = VT_I8;

			HRESULT hr =
			m_pReader->SetCurrentPosition( GUID_NULL, var );

			if( hr != S_OK ) return false;
			return true;
		}

	private:

		IMFSourceReader * m_pReader;

		BYTE * pUnreadBuffer;
		DWORD nUnreadBufferBytes;
		IMFMediaBuffer * m_pLockedBuffer;

		WAVEFORMATEX m_waveFormat;
		LONGLONG m_llDuration_ms;
		UINT m_maxSize;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		HRESULT GetUncompressedPCMAudio( IMFMediaType **ppPCMAudio_p );
	};

	typedef std::shared_ptr<SourceReader> shared_SourceReader_ptr;
	typedef std::weak_ptr<SourceReader> weak_SourceReader_ptr;
}
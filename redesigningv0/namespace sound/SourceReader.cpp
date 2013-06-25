#include "SourceReader.h"

#include <assert.h>
#include <Mferror.h>

using namespace sound;

bool sound::SourceReader::Initialize( const wchar_t * szFilename_p )
{
	pUnreadBuffer = nullptr;

	// Initialize MF

	HRESULT hr = S_OK;

	hr =
		MFStartup( MF_VERSION, MFSTARTUP_LITE );

	if( hr != S_OK ){

		//assert( hr == MF_E_DISABLED_IN_SAFEMODE ); // needs MFError.h

			return false;
	}

	// Initialize MF Source Reader

	hr =
		MFCreateSourceReaderFromURL( szFilename_p, nullptr, &m_pReader );

	if( hr != S_OK ){

		// reading a file...the possibilities for error are enormous

		return false;
	}

	// Get PCM Audio from media

	IMFMediaType * pPCMAudio = nullptr;

	hr =
		GetUncompressedPCMAudio( &pPCMAudio );
	if( hr != S_OK ){

		// decoding stuff possible errors..
		m_pReader->Release(); m_pReader = nullptr;
		return false;
	}

	// Get waveformatex from media

	UINT32 iSize = 0;
	WAVEFORMATEX * pWaveFormatex = nullptr;
	hr = MFCreateWaveFormatExFromMFMediaType( pPCMAudio, &pWaveFormatex, &iSize, MFWaveFormatExConvertFlag_Normal );
	m_waveFormat = *pWaveFormatex;
	CoTaskMemFree(pWaveFormatex);

	if( hr != S_OK ){

		m_pReader->Release(); m_pReader = nullptr;
		pPCMAudio->Release();

		return false;
	}

	// Get duration and max size of audio data

	PROPVARIANT var;
	hr =
		m_pReader->GetPresentationAttribute( (DWORD)MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var );

	if( hr != S_OK ){

		m_pReader->Release(); m_pReader = nullptr;
		pPCMAudio->Release();

		return false;
	}

	// NOTE: duration is in nanoseconds

	m_maxSize = (int)(
		( var.uhVal.QuadPart * (LONGLONG)m_waveFormat.nAvgBytesPerSec
		+ 10000000 ) // round up to the nearest byte
		/ 10000000
		);

	m_llDuration_ms = var.uhVal.QuadPart /= 10000LL; // 100ns to ms
	PropVariantClear(&var); // TODO, don t know if needed, theres samples using and not using

	return true;
}

HRESULT sound::SourceReader::GetUncompressedPCMAudio( IMFMediaType **ppPCMAudio_p )
{
	HRESULT hr = m_pReader->SetStreamSelection( (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, true );

	if( hr != S_OK ){

		return hr;
	}

	//---

	IMFMediaType * pPartialPCMType = nullptr;

	hr = MFCreateMediaType( &pPartialPCMType );

	if( hr != S_OK ){

		return hr;
	}

	//---

	hr = pPartialPCMType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio );

	if( hr != S_OK ){

		pPartialPCMType->Release();
		return hr;
	}

	hr = pPartialPCMType->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_PCM );

	if( hr != S_OK ){

		pPartialPCMType->Release();
		return hr;
	}

	//---

	hr = m_pReader->SetCurrentMediaType( (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, pPartialPCMType );

	if( hr != S_OK ){

		pPartialPCMType->Release();
		return hr;
	}

	//---

	IMFMediaType * pUncompressedPCMType = nullptr;

	hr = m_pReader->GetCurrentMediaType( (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pUncompressedPCMType );

	if( hr != S_OK ){

		pPartialPCMType->Release();
		return hr;
	}

	//---

	hr = m_pReader->SetStreamSelection( (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, true );

	if( hr != S_OK ){

		pUncompressedPCMType->Release();
		pPartialPCMType->Release();
		return hr;
	}

	*ppPCMAudio_p = pUncompressedPCMType;
	(*ppPCMAudio_p)->AddRef();

	// clean

	pUncompressedPCMType->Release();
	pPartialPCMType->Release();

	return hr;
}

bool sound::SourceReader::ReadAllWaveData( BYTE * pData_p, UINT nMaxBytesToWrite_p, UINT & nBytesWritten_p )
{
	HRESULT hr = S_OK;
	nBytesWritten_p = 0;

	IMFSample *pMediaSample = nullptr;
	IMFMediaBuffer *pMediaBuffer = nullptr;

	bool bSucceed = true;

	while( hr == S_OK ){

		DWORD dwFlags = 0;
		bool bLocked = false;
		if( pUnreadBuffer == nullptr ){

			hr =
				m_pReader->ReadSample(	(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
				0,						// MF_SOURCE_READER_CONTROL_FLAG (s)
				nullptr,				// Receives the zero-based index of the stream.
				&dwFlags,				// MF_SOURCE_READER_FLAG
				nullptr,				// Receives the time stamp of the sample, or the time
				// of the stream event indicated in pdwStreamFlags.
				&pMediaSample
				);

			if( FAILED(hr) ){
		
				assert(false); //TODO
				bSucceed = false;
				break;
			}

			if( dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED )
			{
				assert(false); //TODO
				//printf("Type change - not supported by WAVE file format.\n");
				bSucceed = false;
				break;
			}
			if( dwFlags & MF_SOURCE_READERF_ENDOFSTREAM )
			{
				//printf("End of input file.\n");
				break;
			}

			if( pMediaSample == NULL )
			{
				//printf("No sample\n");
				continue;
			}

			hr = pMediaSample->ConvertToContiguousBuffer( &pMediaBuffer );

			if( FAILED(hr) ){
				assert(false); //TODO
				bSucceed = false;
				break;
			}

			//DWORD nBytes = 0;
			//BYTE * pBuffer = nullptr;
			hr = pMediaBuffer->Lock(	&pUnreadBuffer,//(BYTE**)&audioBuffer_p.pAudioData,
										NULL,	// size of data that can be written to he buffer (max lenght)
										&nUnreadBufferBytes	// current lenght
										);

			if( FAILED(hr) ){
				assert(false); //TODO
				bSucceed = false;
				break;
			}

			bLocked = true;
		}

		// #6# Writes the audio data to the output file.

		if( nBytesWritten_p + nUnreadBufferBytes >  nMaxBytesToWrite_p ){

			bSucceed = false;
			break;
		}

		memcpy( (void*)&pData_p[nBytesWritten_p], (void*)pUnreadBuffer, nUnreadBufferBytes );

		pUnreadBuffer = nullptr;
		nBytesWritten_p += nUnreadBufferBytes;

		if( bLocked ){

			// #7# Calls IMFMediaBuffer::Unlock to unlock the buffer object.
			hr = pMediaBuffer->Unlock();
			if( FAILED(hr) ){
				assert(false); //TODO
				bSucceed = false;
				break;
			}

			// clean

			pMediaBuffer->Release();pMediaBuffer = nullptr;
			pMediaSample->Release(); pMediaSample = nullptr;
		}		
	}

	if( pMediaBuffer ){ pMediaBuffer->Release(); }
	if( pMediaSample ){ pMediaSample->Release(); }

	return bSucceed;
}

bool sound::SourceReader::ReadNextWaveSample( BYTE * pData_p, UINT nMaxBytesToWrite_p, UINT & nBytesWritten_p )
{
	// returns true only if all media was read
	
	HRESULT hr = S_OK;
	nBytesWritten_p = 0;


	IMFSample *pMediaSample = nullptr;
	IMFMediaBuffer *pMediaBuffer = nullptr;
	

	DWORD dwFlags = 0;

	hr =
		m_pReader->ReadSample(
		(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		0,						// MF_SOURCE_READER_CONTROL_FLAG (s)
		nullptr,				// Receives the zero-based index of the stream.
		&dwFlags,				// MF_SOURCE_READER_FLAG
		nullptr,				// Receives the time stamp of the sample, or the time
								// of the stream event indicated in pdwStreamFlags.
		&pMediaSample
		);

	if( FAILED(hr) ){

		assert( hr != MF_E_INVALIDREQUEST );
		assert(false); //TODO
		return false;
	}

	if( dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED )
	{
		//printf("Type change - not supported by WAVE file format.\n");
		assert(false); //TODO
		return false;
	}
	if( dwFlags & MF_SOURCE_READERF_ENDOFSTREAM )
	{
		//printf("End of input file.\n");
		return true;
	}

	if( pMediaSample == NULL )
	{
		//printf("No sample\n");
		assert(false); //TODO
		return false;
	}

	hr = pMediaSample->ConvertToContiguousBuffer( &pMediaBuffer );

	if( FAILED(hr) ){
		assert(false); //TODO
		return false;
	}

	DWORD nBytes = 0;
	BYTE * pBuffer = nullptr;
	hr = pMediaBuffer->Lock(	&pBuffer,//(BYTE**)&audioBuffer_p.pAudioData,
								NULL,	// size of data that can be written to the buffer (max lenght)
								&nBytes	// current lenght
								);

	if( FAILED(hr) ){
		pMediaSample->Release();
		return false;
	}


	if( nBytesWritten_p + nBytes >  nMaxBytesToWrite_p ){

		// So he failed to get the sample..and will just skip it?
		// The right thing to do would be save the amount of data written,
		// and keep the current locked buffer, so in the next read it reads the
		// sample buffer and get the remaining data..not sure
		// This is also a real time method, its not a initialization one, so
		// its supposed to be fast..TODO
		// NEWS: an xaudio buffer need to have complete samples, so you cant just pas
		// random data to it (sample pieces)
		// 
		pMediaBuffer->Unlock();
		pMediaBuffer->Release();
		pMediaSample->Release();

		return false;
	}

	memcpy( (void*)&pData_p[nBytesWritten_p], (void*)pBuffer, nBytes );

	nBytesWritten_p += nBytes;

	hr = pMediaBuffer->Unlock();
	if( FAILED(hr) ){
		pMediaSample->Release();
		pMediaBuffer->Release();
		assert(false); //TODO
		return false;
	}

	// clean

	pMediaBuffer->Release();pMediaBuffer = nullptr;
	pMediaSample->Release();pMediaSample = nullptr;

	return false;
}

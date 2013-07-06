#include "sound.h"

using namespace sound;

namespace sound{

	void SetXAudioBufferFromWaveData( XAUDIO2_BUFFER & xBuffer_p, WaveData & waveData_p, bool bLoop_p /*= false*/, bool bConcludeVoice_p /*= true*/  ){

		//bConcludeVoice_p = bConcludeVoice_p;
		xBuffer_p.AudioBytes = waveData_p.nBytes;
		xBuffer_p.pAudioData = waveData_p.pData;
		xBuffer_p.Flags = bConcludeVoice_p ? XAUDIO2_END_OF_STREAM : 0;
		xBuffer_p.LoopCount = bLoop_p ? XAUDIO2_LOOP_INFINITE : 0;
	}
}

sound::Sounder::~Sounder()
{
	// Not sure if I should call DestroyVoice: NEW: yes, otherwise erro is reported on XAudio release(voice still have senders)
	// the stop is the one not necessary I think (NEW: yes, it is not necessary, however, its good to stop before destroying)

	if( m_pFXSourceVoice ){
		m_pFXSourceVoice->Stop();
		m_pFXSourceVoice->DestroyVoice();
	}
	if( m_pMusicSourceVoice ){
		m_pMusicSourceVoice->Stop();
		m_pMusicSourceVoice->DestroyVoice();
	}

	if( m_pMasterVoice ) m_pMasterVoice->DestroyVoice();
	if( m_pXAudio ) m_pXAudio->Release();

	//CoUninitialize();
	//if( m_pMasterVoice )
	MFShutdown();
}

bool sound::Sounder::Init()
{
	// NOTE win8: second param MUST be zero
	// "Note  Specifying a processor should generally be avoided because it
	// can interfere with the scheduler's ability to schedule threads
	// effectively across processors. Instead, pass the XAUDIO2_DEFAULT_PROCESSOR"
	//(void)HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0); 
	//CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );	

	UINT32 flags = 0;

#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	HRESULT hr = 
		XAudio2Create( &m_pXAudio, flags, XAUDIO2_DEFAULT_PROCESSOR );

	if( hr == S_OK ){

		/*DBG(
		XAUDIO2_DEBUG_CONFIGURATION debug = {0};
		debug.BreakMask = XAUDIO2_LOG_ERRORS;
		debug.LogFileline = true;
		debug.LogFunctionName = true;
		debug.LogTiming = true;
		debug.TraceMask = XAUDIO2_LOG_MEMORY|XAUDIO2_LOG_WARNINGS;
		m_pXAudio->SetDebugConfiguration(&debug );
		)*/

		// init media foundation
		hr = MFStartup(MF_VERSION);

		if( hr == S_OK ){
			
			return true;
		}
	}

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert (hr == XAUDIO2_E_DEVICE_INVALIDATED );

	if( m_pXAudio ){

		m_pXAudio->Release();
		m_pXAudio = nullptr;
	}

	return false;
}

bool sound::Sounder::EnumAdapters()
{
	// TODO: implement this similar to the way I did on swap chain adapter enums,
	// with a custom priority mechanism..like GameDevice role 1, globaldefault role 2,
	// 4 channels role 4, 2 channels role 2, 0 channels role 1
	// 
	UINT32 nDeviceCount = 0;
	HRESULT hr = 
		m_pXAudio->GetDeviceCount( &nDeviceCount );

	if ( FAILED(hr) ){

		assert( hr != XAUDIO2_E_INVALID_CALL );
		assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
		assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

		// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
		assert (hr == XAUDIO2_E_DEVICE_INVALIDATED );

		return false;
	}

	for( UINT32 itIndex = 0; itIndex < nDeviceCount; ++itIndex )
	{
		XAUDIO2_DEVICE_DETAILS details;
		hr = m_pXAudio->GetDeviceDetails( itIndex, &details );
		if (FAILED(hr)){

			assert( hr != XAUDIO2_E_INVALID_CALL );
			assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
			assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

			// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
			assert (hr == XAUDIO2_E_DEVICE_INVALIDATED );

			return false;
		}

		// display name is details.DisplayName
		// internal device ID is details.DeviceID
		// the 'index' is used for creating the mastering voice
	}

	return true;
}

bool sound::Sounder::CreateMasterVoice( UINT32 nInputChannels_p /*= XAUDIO2_DEFAULT_CHANNELS*/,
	UINT32 nInputSamplerRate_p /*= XAUDIO2_DEFAULT_SAMPLERATE*/, UINT32 iDeviceIndex_p /*= 0*/,
	XAUDIO2_EFFECT_CHAIN * pEffectChain_p /*= nullptr */ )
{
	assert( nInputChannels_p <= XAUDIO2_MAX_AUDIO_CHANNELS );
	assert( nInputSamplerRate_p <= XAUDIO2_MAX_SAMPLE_RATE );

	HRESULT hr =
		m_pXAudio->CreateMasteringVoice(	&m_pMasterVoice,
		nInputChannels_p,
		nInputSamplerRate_p,
		0,				//must be zero
		iDeviceIndex_p, // 0 = default device
		pEffectChain_p	// effect chain
		);
	if( hr == S_OK ) return true;

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert (hr == XAUDIO2_E_DEVICE_INVALIDATED
		||
		hr == ERROR_NOT_FOUND );

	return false;
}
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool sound::Sounder::CreateFXSourceVoice( const WAVEFORMATEX & waveData_p, UINT32 iFlags_p /*= 0*/,
	float fMaxFrquenceRatio_p /*= XAUDIO2_DEFAULT_FREQ_RATIO*/, IXAudio2VoiceCallback * pCallback_p /*= nullptr*/,
	XAUDIO2_VOICE_SENDS * pSendList_p /*= nullptr*/, XAUDIO2_EFFECT_CHAIN * pEffectChain_p /*= nullptr */ )
{
	// after created, it can play any XAUDIO2_BUFFER send to it, IF it conforms with WAVEFORMATEX

	HRESULT hr =
		m_pXAudio->CreateSourceVoice(	&m_pFXSourceVoice, &waveData_p, iFlags_p, fMaxFrquenceRatio_p, pCallback_p,
		pSendList_p, pEffectChain_p );

	//m_pFXSourceVoice->

	if( hr == S_OK ) return true;

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert ( hr == XAUDIO2_E_DEVICE_INVALIDATED
		||
		hr == ERROR_NOT_FOUND );

	return false;
}

bool sound::Sounder::LoadSoundFX( XAUDIO2_BUFFER * pAudioBuffer_p, XAUDIO2_BUFFER_WMA * pWMABuffer_p /*= nullptr */ )
{
	// Note  The audio sample data to which buffer points is still 'owned'
	// by the app and must remain allocated and accessible until the sound stops playing.
	
	HRESULT hr =
		m_pFXSourceVoice->SubmitSourceBuffer( pAudioBuffer_p, pWMABuffer_p );

	if( hr == S_OK ) return true;

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert (hr == XAUDIO2_E_DEVICE_INVALIDATED
		||
		hr == ERROR_NOT_FOUND );

	return false;

	// The pMediaBuffer pointer can be reused or freed immediately after calling this method,
	// but the actual audio data referenced by pMediaBuffer must remain valid until the buffer
	// has been fully consumed by XAudio2 (which is indicated by the
	// IXAudio2VoiceCallback::OnBufferEnd callback).
	// Up to XAUDIO2_MAX_QUEUED_BUFFERS buffers can be queued on a voice at any one time.
	// SubmitSourceBuffer takes effect immediately when called from an XAudio2 callback
	// with an OperationSet of XAUDIO2_COMMIT_NOW.
}

bool sound::Sounder::PlayFX( UINT32 iOperationSet_p /*= XAUDIO2_COMMIT_NOW*/, UINT32 iFlags_p /*= 0 */, XAUDIO2_BUFFER * pAudioBuffer_p /*= nullptr*/ )
{
	//m_pFXSourceVoice->Stop();

	XAUDIO2_VOICE_STATE state = {0};
	m_pFXSourceVoice->GetState(&state);

	if( state.BuffersQueued > 0 ){
		m_pFXSourceVoice->Stop();
		m_pFXSourceVoice->FlushSourceBuffers();
		m_pFXSourceVoice->SubmitSourceBuffer( pAudioBuffer_p );
	}
	else{
		m_pFXSourceVoice->Stop();
		m_pFXSourceVoice->SubmitSourceBuffer( pAudioBuffer_p );
	}

	HRESULT hr =
		m_pFXSourceVoice->Start(iFlags_p, iOperationSet_p);

	if( hr == S_OK ) return true;

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert (hr == XAUDIO2_E_DEVICE_INVALIDATED
		||
		hr == ERROR_NOT_FOUND );

	return false;
}

bool sound::Sounder::StopFX( UINT32 iOperationSet_p /*= XAUDIO2_COMMIT_NOW*/, UINT32 iFlags_p /*= 0 */ )
{

	HRESULT hr = m_pFXSourceVoice->Stop( iFlags_p, iOperationSet_p );
	//hr = m_pFXSourceVoice->FlushSourceBuffers();

	if( hr == S_OK ) return true;

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert (hr == XAUDIO2_E_DEVICE_INVALIDATED
		||
		hr == ERROR_NOT_FOUND );

	return false;
}
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool sound::Sounder::CreateMusicSourceVoice( const WAVEFORMATEX & waveData_p, UINT32 iFlags_p /*= 0*/,
											float fMaxFrquenceRatio_p /*= XAUDIO2_DEFAULT_FREQ_RATIO*/, IXAudio2VoiceCallback * pCallback_p /*= nullptr*/,
											XAUDIO2_VOICE_SENDS * pSendList_p /*= nullptr*/, XAUDIO2_EFFECT_CHAIN * pEffectChain_p /*= nullptr */ )
{
	// after created, it can play any XAUDIO2_BUFFER send to it, IF it conforms with WAVEFORMATEX

	HRESULT hr =
		m_pXAudio->CreateSourceVoice(	&m_pMusicSourceVoice, &waveData_p, iFlags_p, fMaxFrquenceRatio_p, pCallback_p,
		pSendList_p, pEffectChain_p );

	//m_pFXSourceVoice->

	if( hr == S_OK ) return true;

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert ( hr == XAUDIO2_E_DEVICE_INVALIDATED
		||
		hr == ERROR_NOT_FOUND );

	return false;
}

bool sound::Sounder::LoadSoundMusic( XAUDIO2_BUFFER * pAudioBuffer_p, XAUDIO2_BUFFER_WMA * pWMABuffer_p /*= nullptr */ )
{
	// Note  The audio sample data to which buffer points is still 'owned'
	// by the app and must remain allocated and accessible until the sound stops playing.

	HRESULT hr =
		m_pMusicSourceVoice->SubmitSourceBuffer( pAudioBuffer_p, pWMABuffer_p );

	if( hr == S_OK ) return true;

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert (hr == XAUDIO2_E_DEVICE_INVALIDATED
		||
		hr == ERROR_NOT_FOUND );

	return false;

	// The pMediaBuffer pointer can be reused or freed immediately after calling this method,
	// but the actual audio data referenced by pMediaBuffer must remain valid until the buffer
	// has been fully consumed by XAudio2 (which is indicated by the
	// IXAudio2VoiceCallback::OnBufferEnd callback).
	// Up to XAUDIO2_MAX_QUEUED_BUFFERS buffers can be queued on a voice at any one time.
	// SubmitSourceBuffer takes effect immediately when called from an XAudio2 callback
	// with an OperationSet of XAUDIO2_COMMIT_NOW.
}

bool sound::Sounder::PlayMusic( UINT32 iOperationSet_p /*= XAUDIO2_COMMIT_NOW*/, UINT32 iFlags_p /*= 0 */, XAUDIO2_BUFFER * pAudioBuffer_p /*= nullptr*/ )
{
	//m_pFXSourceVoice->Stop();

	XAUDIO2_VOICE_STATE state = {0};
	m_pMusicSourceVoice->GetState(&state);

	if( state.BuffersQueued > 0 ){
		m_pMusicSourceVoice->Stop();
		m_pMusicSourceVoice->FlushSourceBuffers();
		m_pMusicSourceVoice->SubmitSourceBuffer( pAudioBuffer_p );
	}
	else{
		m_pMusicSourceVoice->Stop();
		m_pMusicSourceVoice->SubmitSourceBuffer( pAudioBuffer_p );
	}

	HRESULT hr =
		m_pMusicSourceVoice->Start(iFlags_p, iOperationSet_p);

	if( hr == S_OK ) return true;

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert (hr == XAUDIO2_E_DEVICE_INVALIDATED
		||
		hr == ERROR_NOT_FOUND );

	return false;
}

bool sound::Sounder::StopMusic( UINT32 iOperationSet_p /*= XAUDIO2_COMMIT_NOW*/, UINT32 iFlags_p /*= 0 */ )
{

	HRESULT hr = m_pMusicSourceVoice->Stop( iFlags_p, iOperationSet_p );
	//hr = m_pFXSourceVoice->FlushSourceBuffers();

	if( hr == S_OK ) return true;

	assert( hr != XAUDIO2_E_INVALID_CALL );
	assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
	assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

	// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
	assert (hr == XAUDIO2_E_DEVICE_INVALIDATED
		||
		hr == ERROR_NOT_FOUND );

	return false;
}

bool sound::Sounder::LoadAudioFile( const WCHAR * szFileURL_p, WaveData & waveData_p )
{
	IMFSourceReader * pMFSourceReader = nullptr;
	//IMFAttributes * pAttributes = nullptr;

	HRESULT hr = MFCreateSourceReaderFromURL( szFileURL_p, nullptr, &pMFSourceReader ); // requires MF initialization

	if( hr != S_OK ){

		if( pMFSourceReader ) pMFSourceReader->Release();

		return false;
	}

	// #1# The ConfigureAudioStream function initializes the source reader. This function receives a pointer to
	// the IMFMediaType interface, which is used to get a description of the decoded audio format, including
	// sample rate, number of channels, and bit depth (bits per sample).

	IMFMediaType * pPCMAudio = nullptr;
	hr = SourceReader_ConfigureToDecodeAudioStream( pMFSourceReader, &pPCMAudio );

	if( hr != S_OK ){

		pMFSourceReader->Release();
		if( pPCMAudio ) pPCMAudio->Release();

		return false;
	}

	// #2#  MFCreateWaveFormatExFromMFMediaType converts the media type to a WAVEFORMATEX structure.
	// pWaveData_p: the caller must release the memory allocated for the structure by calling CoTaskMemFree.
	// MFWaveFormatExConvertFlag_Normal: Convert the media type to a WAVEFORMATEX structure if possible,
	// or a WAVEFORMATEXTENSIBLE structure otherwise.

	UINT32 iSize = 0;

	WAVEFORMATEX * pWaveFormatex = nullptr;
	hr = MFCreateWaveFormatExFromMFMediaType(	pPCMAudio, &pWaveFormatex, &iSize, MFWaveFormatExConvertFlag_Normal );
	waveData_p.format = *pWaveFormatex;
	CoTaskMemFree(pWaveFormatex);

	if( hr != S_OK ){

		pMFSourceReader->Release();
		pPCMAudio->Release();

		return false;
	}

	// #3# Get the audio data

	// all this unbelievable shit is to calculate the size of total  audio data!!!

	LONGLONG duration = 0;
	SourceReader_GetDuration( pMFSourceReader, duration );

	//double dDuration = duration / 10000.0;
	//double dDuration = duration / 10000000.0 * 1000.0; // win 
	//double dDuration = duration / (10000000.0 * 1000.0); // fail
	////dDuration *= 1000.0;

	duration /= 10000LL; // nano to msec
	//duration = duration / 10000000LL; // nano to sec (fails if less than a sec (becames 0)
	//duration *= 1000; // sec to msec

	waveData_p.llDuration_ms = duration;
	//DWORD header[] = { 
	//	// RIFF header
	//	FCC('RIFF'), 
	//	0, 
	//	FCC('WAVE'),  
	//	// Start of 'fmt ' chunk
	//	FCC('fmt '), 
	//	iSize 
	//};
	//header[0] = header[1];
	//DWORD dataHeader[] = { FCC('data'), 0 };
	//dataHeader[0] = dataHeader[1];
	waveData_p.nBytes = CalculateMaxAudioDataSize( pPCMAudio, iSize + sizeof(DWORD)*5 + sizeof(DWORD)*2, (DWORD)duration );

	//pMFSourceReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var);

	waveData_p.pData = new BYTE[waveData_p.nBytes];
	UINT32 nBytesWritten = 0;

	hr = SourceReader_ReadAudioData( pMFSourceReader, waveData_p, waveData_p.nBytes, nBytesWritten );

	//if( waveData_p.nBytes != nBytesWritten )
	//	BREAKHERE;

	waveData_p.nBytes = nBytesWritten;

	//clean

	pMFSourceReader->Release();
	pPCMAudio->Release();

	return true;
}
//------------------------------------------------------------------------
HRESULT sound::Sounder::SourceReader_ConfigureToDecodeAudioStream( IMFSourceReader *pSourceReader_p, IMFMediaType **ppPCMAudio_p )
{
	// #1# Calls the IMFSourceReader::SetStreamSelection method to select the audio stream and deselect all other streams.
	// This step can improve performance, because it prevents the source reader from holding onto video frames that
	// the application does not use.

	HRESULT hr = pSourceReader_p->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, true );
	if( hr != S_OK ) return hr;

	// #2# Creates a partial media type that specifies PCM audio. The function creates the partial type as follows:
	// Calls MFCreateMediaType to create an empty media type object.
	//	Sets the MF_MT_MAJOR_TYPE attribute to MFMediaType_Audio.
	//	Sets the MF_MT_SUBTYPE attribute to MFAudioFormat_PCM.

	IMFMediaType * pPartialPCMType = nullptr;

	hr = MFCreateMediaType( &pPartialPCMType );
	if( hr != S_OK ){

		if( pPartialPCMType ) pPartialPCMType->Release();
		return hr;
	}

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

	// #3# Calls IMFSourceReader::SetCurrentMediaType to set the partial type on the source reader. If the source
	// file contains encoded audio, the source reader automatically loads the necessary audio decoder.

	hr = pSourceReader_p->SetCurrentMediaType( (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, pPartialPCMType );
	if( hr != S_OK ){

		pPartialPCMType->Release();
		return hr;
	}

	// #4# Calls IMFSourceReader::GetCurrentMediaType to get the actual PCM media type. This method returns a
	// media type with all of the format details filled in, such as the audio sample rate and the number of channels.

	IMFMediaType * pUncompressedPCMType = nullptr;

	hr = pSourceReader_p->GetCurrentMediaType( (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pUncompressedPCMType);
	if( hr != S_OK ){

		if(pUncompressedPCMType) pUncompressedPCMType->Release();
		pPartialPCMType->Release();
		return hr;
	}

	// #5# Calls IMFSourceReader::SetStreamSelection to enable the audio stream.

	hr = pSourceReader_p->SetStreamSelection( (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, true );
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

HRESULT sound::Sounder::SourceReader_ReadAudioData( IMFSourceReader *pSourceReader_p, WaveData & waveData_p, UINT32 nMaxBytesToWrite_p , UINT32 & nBytesWritten_p )
{
	IMFSample *pMediaSample = nullptr;
	IMFMediaBuffer *pMediaBuffer = nullptr;
	HRESULT hr = S_OK;
	nBytesWritten_p = 0;

	while( hr == S_OK ){

		DWORD dwFlags = 0;

		// #1# Calls IMFSourceReader::ReadSample to read audio from the source file. The dwFlags parameter
		// receives a bitwise OR of flags from the MF_SOURCE_READER_FLAG enumeration. The pMediaSample parameter
		// receives a pointer to the IMFSample interface, which is used to access the audio data.
		// In some cases a call to ReadSample does not generate data, in which case the IMFSample pointer is NULL.

		hr = pSourceReader_p->ReadSample(	(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			0,						// MF_SOURCE_READER_CONTROL_FLAG (s)
			nullptr,				// Receives the zero-based index of the stream.
			&dwFlags,				// MF_SOURCE_READER_FLAG
			nullptr,				// Receives the time stamp of the sample, or the time
			// of the stream event indicated in pdwStreamFlags.
			&pMediaSample
			);
		if( FAILED(hr) ) break;

		// #2# Checks dwFlags for the following flags:
		//	MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED. This flag indicates a format change in the source file.
		//	WAVE files do not support format changes.
		//	MF_SOURCE_READERF_ENDOFSTREAM. This flag indicates the end of the stream.

		if( dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED )
		{
			//printf("Type change - not supported by WAVE file format.\n");
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

		// #3# Calls IMFSample::ConvertToContiguousBuffer to get a pointer to a buffer object.

		hr = pMediaSample->ConvertToContiguousBuffer( &pMediaBuffer );

		if( FAILED(hr) ) break;

		// #4# Calls IMFMediaBuffer::Lock to get a pointer to the buffer memory.
		DWORD nBytes = 0;
		BYTE * pBuffer = nullptr;
		hr = pMediaBuffer->Lock(	&pBuffer,//(BYTE**)&audioBuffer_p.pAudioData,
									NULL,	// size of data that can be written to he buffer (max lenght)
									&nBytes	// current lenght
									);

		if( FAILED(hr) ) break;

		// #6# Writes the audio data to the output file.

		if( nBytesWritten_p + nBytes >  nMaxBytesToWrite_p ){

			BREAKHERE;
			break;
		}

		memcpy( (void*)&waveData_p.pData[nBytesWritten_p], (void*)pBuffer, nBytes );
		nBytesWritten_p += nBytes;

		// #7# Calls IMFMediaBuffer::Unlock to unlock the buffer object.

		hr = pMediaBuffer->Unlock();
		if( FAILED(hr) )break;

		// clean

		pMediaBuffer->Release();pMediaBuffer = nullptr;
		pMediaSample->Release();pMediaSample = nullptr;

	}

	if( pMediaBuffer ){ pMediaBuffer->Release(); }
	if( pMediaSample ){ pMediaSample->Release(); }

	return hr;
}

HRESULT sound::Sounder::SourceReader_GetDuration( IMFSourceReader *pReader, LONGLONG & i64Duration_p )
{
	PROPVARIANT var;
	HRESULT hr = pReader->GetPresentationAttribute( (DWORD)MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var );

	if( SUCCEEDED(hr) ){

		hr = PropVariantToInt64( var, &i64Duration_p );
		PropVariantClear(&var);
	}
	return hr;
}

DWORD sound::Sounder::CalculateMaxAudioDataSize( IMFMediaType *pAudioType, /* The PCM audio format. */ DWORD cbHeader,
	/* The size of the WAVE file header. */ DWORD msecAudioData /* Maximum duration, in milliseconds. */ )
{
	UINT32 cbBlockSize = 0;         // Audio frame size, in bytes.
	UINT32 cbBytesPerSecond = 0;    // Bytes per second.

	// Get the audio block size and number of bytes/second from the audio format.

	cbBlockSize = MFGetAttributeUINT32(pAudioType, MF_MT_AUDIO_BLOCK_ALIGNMENT, 0);
	cbBytesPerSecond = MFGetAttributeUINT32(pAudioType, MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 0);

	// Calculate the maximum amount of audio data to write.
	// This value equals (duration in seconds x bytes/second), but cannot
	// exceed the maximum size of the data chunk in the WAVE file.

	// Size of the desired audio clip in bytes:
	DWORD cbAudioClipSize = (DWORD)MulDiv(cbBytesPerSecond, msecAudioData, 1000);

	// Largest possible size of the data chunk:
	DWORD cbMaxSize = MAXDWORD - cbHeader;

	// Maximum size altogether.
	cbAudioClipSize = min(cbAudioClipSize, cbMaxSize);

	// Round to the audio block size, so that we do not write a partial audio frame.
	cbAudioClipSize = (cbAudioClipSize / cbBlockSize) * cbBlockSize;

	return cbAudioClipSize;
}

#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/15
	created:	15:6:2013   0:00
	file:		sound.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	testing xaudio2...
				TODO: IMPORTANT: linking against june 2010 sdk..the new one requires a dll only available on win 8
				IXAudio2::GetDeviceCount() or IXAudio2::GetDeviceDetails() must be guarded since these APIs do not
				exist in the Windows 8 version

				Marble Maze specifies AudioCategory_GameMedia for background music so that users can listen to music
				from a different application as they play the game. When a music app is playing, Windows mutes any
				voices that are created by the AudioCategory_GameMedia option. The user still hears game-play sounds
				because they are created by the AudioCategory_GameEffects option.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#ifdef SOUND
#define IFSOUND(stuff) stuff
#define SNDCOMMA() ,
#else
#define IFSOUND(stuff)
#define SNDCOMMA()
#endif // _DEBUG

// media foundation libs and headers
//#pragma comment(lib, "XAudio2.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#pragma comment(lib, "Propsys.lib")

#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>

// NOTE: explicit link against dx sdk XAudio2 if win 7, if not, link against win 8 XAudio2

#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h"
//#pragma comment(lib,"ole32.lib") // com stuff

// standard includes

#include <memory>
#include <assert.h>
#include <Propvarutil.h>

// private includes

#include "../namespace gen/gen_macros.h"

namespace sound{

	//------------------------------------------------------------------------
	// holds raw audio data.
	// a file will be parsed into this structure.
	//------------------------------------------------------------------------
	struct WaveData{

		WAVEFORMATEX format;
		UINT32 nBytes;
		BYTE * pData; // PCMData?
		LONGLONG llDuration_ms;
	};

	extern void SetXAudioBufferFromWaveData( XAUDIO2_BUFFER & xBuffer_p, WaveData & waveData_p, bool bLoop_p = false, bool bConcludeVoice_p = true  );

	class Sounder{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		Sounder()
			:
		m_pXAudio(nullptr),
		m_pMasterVoice(nullptr),
		m_pFXSourceVoice(nullptr),
		m_pMusicSourceVoice(nullptr)
		{}

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~Sounder();

		//------------------------------------------------------------------------
		// initializes the XAudio engine
		//------------------------------------------------------------------------
		bool Init();

		//------------------------------------------------------------------------
		// ATTENTION: THIS METHOD MAKE CRT DEBUG NOT REPORT ANY LEAKS!!!!!!!!!!!!!!
		//------------------------------------------------------------------------
		bool EnumAdapters();

		//------------------------------------------------------------------------
		// win7: device as index (from enumAdapters)
		//------------------------------------------------------------------------
		bool CreateMasterVoice( UINT32 nInputChannels_p = XAUDIO2_DEFAULT_CHANNELS,
								UINT32 nInputSamplerRate_p =  XAUDIO2_DEFAULT_SAMPLERATE,
								UINT32 iDeviceIndex_p = 0,
								XAUDIO2_EFFECT_CHAIN * pEffectChain_p = nullptr );

		//------------------------------------------------------------------------
		// to use ADPCM, cast the structure needed to WAVEFORMATEX 
		// "Note  You can use the lowest possible MaxFrequencyRatio value to reduce XAudio2's memory usage."
		//------------------------------------------------------------------------
		bool CreateFXSourceVoice( const WAVEFORMATEX & waveData_p,
								UINT32 iFlags_p = 0,
								float fMaxFrquenceRatio_p = XAUDIO2_DEFAULT_FREQ_RATIO,
								IXAudio2VoiceCallback * pCallback_p = nullptr,
								XAUDIO2_VOICE_SENDS * pSendList_p = nullptr,
								XAUDIO2_EFFECT_CHAIN * pEffectChain_p = nullptr
								);
		bool CreateMusicSourceVoice(	const WAVEFORMATEX & waveData_p,
										UINT32 iFlags_p = 0,
										float fMaxFrquenceRatio_p = XAUDIO2_DEFAULT_FREQ_RATIO,
										IXAudio2VoiceCallback * pCallback_p = nullptr,
										XAUDIO2_VOICE_SENDS * pSendList_p = nullptr,
										XAUDIO2_EFFECT_CHAIN * pEffectChain_p = nullptr
										);

		//------------------------------------------------------------------------
		// NOTE:  "The audio sample data to which buffer points is still 'owned' by the app and must remain
		// allocated and accessible until the sound stops playing"
		//------------------------------------------------------------------------
		bool LoadSoundFX( XAUDIO2_BUFFER * pAudioBuffer_p, XAUDIO2_BUFFER_WMA * pWMABuffer_p = nullptr );
		bool LoadSoundMusic( XAUDIO2_BUFFER * pAudioBuffer_p, XAUDIO2_BUFFER_WMA * pWMABuffer_p = nullptr );

		//------------------------------------------------------------------------
		// operation set is used to make lots of operations in parallell, so they will
		// only occur when calling xaudio2 commit.
		// flags must be 0
		//------------------------------------------------------------------------
		bool PlayFX( UINT32 iOperationSet_p = XAUDIO2_COMMIT_NOW, UINT32 iFlags_p = 0, XAUDIO2_BUFFER * pAudioBuffer_p = nullptr  );
		bool StopFX(UINT32 iOperationSet_p = XAUDIO2_COMMIT_NOW, UINT32 iFlags_p = 0 );

		bool PlayMusic( UINT32 iOperationSet_p = XAUDIO2_COMMIT_NOW, UINT32 iFlags_p = 0, XAUDIO2_BUFFER * pAudioBuffer_p = nullptr  );
		bool StopMusic(UINT32 iOperationSet_p = XAUDIO2_COMMIT_NOW, UINT32 iFlags_p = 0 );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool LoadAudioFile( const WCHAR * szFileURL_p, WaveData & waveData_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool SetVolume( float fPercent_p, UINT32 iOperationSet_p = XAUDIO2_COMMIT_NOW ){

			HRESULT hr =
			m_pMasterVoice->SetVolume( fPercent_p, iOperationSet_p );

			if( hr == S_OK ) return true;

			return false;
		}
		float GetVolume(){

			float volume = 0.0f;
			m_pMasterVoice->GetVolume( &volume );

			return volume;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Scream(){

			HRESULT hr = m_pXAudio->CommitChanges(XAUDIO2_COMMIT_ALL);

			if( hr != S_OK )
				BREAKHERE;
		}

		IXAudio2 * m_pXAudio;

	private:

		//------------------------------------------------------------------------
		// TODO: encapsulate source reader
		// media types describes the formats of a media stream
		// major format: audio, video
		// subformat: mp3, wave, mpeg
		//------------------------------------------------------------------------
		HRESULT SourceReader_ConfigureToDecodeAudioStream( IMFSourceReader *pSourceReader_p, IMFMediaType **ppPCMAudio_p );
		HRESULT SourceReader_ReadAudioData( IMFSourceReader *pSourceReader_p, WaveData & waveData_p, UINT32 nMaxBytesToWrite_p, UINT32 & nBytesWritten_p );
		HRESULT SourceReader_GetDuration(IMFSourceReader *pReader, LONGLONG & i64Duration_p );
		DWORD CalculateMaxAudioDataSize(
			IMFMediaType *pAudioType,    // The PCM audio format.
			DWORD cbHeader,              // The size of the WAVE file header.
			DWORD msecAudioData          // Maximum duration, in milliseconds.
			);

		IXAudio2MasteringVoice * m_pMasterVoice; // those should be holded by the user (bg music master, snd fx master, ambient master)
		IXAudio2SourceVoice * m_pFXSourceVoice;	 // those should be holded by the user, one per sound file..I guess
		IXAudio2SourceVoice * m_pMusicSourceVoice;
	};

	typedef std::shared_ptr<Sounder> shared_Sounder_ptr;
	typedef std::weak_ptr<Sounder> weak_Sounder_ptr;
}
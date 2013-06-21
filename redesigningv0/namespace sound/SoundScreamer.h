#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/21
	created:	21:6:2013   12:39
	file:		SoundScreamer.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// media foundation libs and headers

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Propsys.lib")

// standard includes

//  MS Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>

//  DX June 2010 XAudio2
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h"
#include <memory>
#include <assert.h>
#include <Propvarutil.h>

// private includes

#include "../namespace gen/gen_macros.h"

namespace sound{

	//template< int NTRACKS = 32 >
	//struct FXTracks{

	//	IXAudio2SourceVoice * pFXVoices[NTRACKS];
	//	UINT m_iCurrentIndex;
	//};

	//template< int NTRACKS = 4 >
	//struct MusicTracks{

	//	IXAudio2SourceVoice * pFXVoices[NTRACKS];
	//	UINT m_iCurrentIndex;
	//};
	

	// NOTE: "WAVEFORMATEX structure contains the expected format for all
	// audio buffers submitted to the source voice. XAudio2 supports PCM
	// and ADPCM voice types.""
	
	//------------------------------------------------------------------------
	// This class is supposed to group voices that have the same wave format
	// and send to the same voice(s) on a graph.
	// It works like a poll for paralleling playing sounds.
	//------------------------------------------------------------------------
	template< int NTRACKS = 32 >
	class SourceVoiceTracks{

		WAVEFORMATEX m_waveFormat;
		IXAudio2SourceVoice * pFXVoices[NTRACKS];
		UINT m_iLastIndex;
	};

	class SndFX{

		XAUDIO2_BUFFER audioBuffer;
		UINT nTracksUsing;
	};

	//========================================================================
	// 
	//========================================================================
	template< int N_MASTERVOICES = 1 >
	class SoundScreamer{

		

	public:

		//------------------------------------------------------------------------
		// plays the sounds
		//------------------------------------------------------------------------
		void Scream(){}

	private:

		IXAudio2MasteringVoice * m_pMasterVoices[N_MASTERVOICES];


	};	
}
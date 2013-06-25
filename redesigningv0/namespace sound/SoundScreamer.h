#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/21
	created:	21:6:2013   12:39
	file:		SoundPlayer.h
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


	// NOTE: "WAVEFORMATEX structure contains the expected format for all
	// audio buffers submitted to the source voice. XAudio2 supports PCM
	// and ADPCM voice types.""
	//------------------------------------------------------------------------
	// holds raw audio data.
	// a file will be parsed into this structure.
	//------------------------------------------------------------------------
	struct WaveData{

		WAVEFORMATEX format;
		UINT32 nBytes; // not necessarily amount allocated, but amount of audio data written to the buffer
		BYTE * pData; // PCMData?
		LONGLONG llDuration_ms;
	};



	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	class SoundData{

	private:

		XAUDIO2_BUFFER audioBuffer;

		UINT nTracksUsing;
		UINT nTracksPlaying;
	};

	//========================================================================
	// 
	//========================================================================
	template< int N_MASTERVOICES = 1 >
	class SoundPlayer{

	public:

		//------------------------------------------------------------------------
		// plays the sounds
		//------------------------------------------------------------------------
		void Scream(){}

	private:

		IXAudio2MasteringVoice * m_pMasterVoices[N_MASTERVOICES];


	};	
}
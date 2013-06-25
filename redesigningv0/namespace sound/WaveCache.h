#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/23
	created:	23:6:2013   19:25
	file:		WaveCache.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	Holds all wave data loaded from file.

				NOTE: "WAVEFORMATEX structure contains the expected format for all
				audio buffers submitted to the source voice. XAudio2 supports PCM
				and ADPCM voice types.""

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <vector>
#include <string>
#include <assert.h>

// private includes
#include "SourceReader.h"

namespace sound{

	//------------------------------------------------------------------------
	// Holds raw audio data.
	// A file will be parsed into this structure.
	//------------------------------------------------------------------------
	struct WaveData{

		WAVEFORMATEX format;
		UINT32 nBytes; // not necessarily amount allocated, but amount of audio data written to the buffer
		BYTE * pData; // PCMData?
		LONGLONG llDuration_ms;
	};

	//========================================================================
	// 
	//========================================================================
	class WaveCache{

	public:

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~WaveCache(){

			DestroyWavesData();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		int AddWave( const wchar_t * szWaveFileName_p );

		//------------------------------------------------------------------------
		// gets
		//------------------------------------------------------------------------
		WaveData & GetWave( const wchar_t * szWaveFileName_p );
		WaveData & GetWave( int iIndex_p ){

			assert( iIndex_p < m_waves.size() );

			return m_waves[iIndex_p];
		}

	private:

		std::vector<WaveData> m_waves;
		std::vector<std::wstring> m_fileNames;

		void DestroyWavesData(){

			for( int it = 0, size = (int)m_waves.size(); it < size; ++it ){

				delete m_waves[it].pData;
			}
		}
	};

	typedef std::shared_ptr<WaveCache> shared_WaveCache_ptr;
	typedef std::weak_ptr<WaveCache> weak_WaveCache_ptr;
}
#include "WaveCache.h"


using namespace sound;

int sound::WaveCache::AddWave( const wchar_t * szWaveFileName_p )
{
	SourceReader m_reader;

	assert( m_reader.Initialize( szWaveFileName_p ) == true );

	WaveData data = {0};

	data.format = m_reader.GetWaveFormat();
	data.llDuration_ms = m_reader.GetDurationMS();
	data.nBytes = m_reader.GetMaxSize();

	data.pData = new BYTE[data.nBytes];

	UINT nBytesWritten = 0;
	assert( m_reader.ReadAllWaveData( data.pData, data.nBytes, nBytesWritten ) == true );

	// just for testing
	if( nBytesWritten != data.nBytes ){

		data.nBytes = nBytesWritten;
	}

	// Add to cache

	m_waves.push_back(data);
	m_fileNames.push_back(szWaveFileName_p);

	return (int)m_waves.size()-1;
}

WaveData & sound::WaveCache::GetWave( const wchar_t * szWaveFileName_p )
{
	// search

	for( int it = 0, size = (int)m_fileNames.size(); it < size; ++it ){

		if( m_fileNames[it] == szWaveFileName_p ){

			return m_waves[it];
		}
	}

	// didn t find, add

	int it =
		AddWave( szWaveFileName_p );

	return m_waves[it];
}

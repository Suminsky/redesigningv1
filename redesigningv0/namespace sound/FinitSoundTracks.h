#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/23
	created:	23:6:2013   15:30
	file:		FinitSoundTracks.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	This class is supposed to group voices that have the same wave format,
				and same every configuration, with means they hold from 1 to the number of
				needed parallel sounds necessary to be played (played at the same time).
				"Paralleling playing sounds with the same config".
				The point is not having unnecessary voice for each existent audio buffer,
				since voices can be reused.
				Not recommended for looping audio, as it will be override at some point.

				TODO: check if I really need all that start and stop thing..also why stop
				with operation set dont work..

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h"

// private includes

#pragma warning( push )
#pragma warning( disable : 4351 ) // VS specific, array default initialization

namespace sound{

	template< int NTRACKS = 32 >
	class FinitSoundTracks{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		FinitSoundTracks()
			:
		m_pVoices(){}

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~FinitSoundTracks(){

			if( m_pVoices[0] ){ // means voices are initialized

				for( int it = 0; it < NTRACKS; ++it ){

					m_pVoices[it]->Stop();
					m_pVoices[it]->DestroyVoice();
				}
			}
		}

		//------------------------------------------------------------------------
		// initializes all the voices with the same configuration
		//------------------------------------------------------------------------
		bool InitializeTracks(	IXAudio2 * pXAudio_p, WAVEFORMATEX & waveFormat_p,
							UINT32 iFlags_p = 0, float fMaxFrquenceRatio_p = XAUDIO2_DEFAULT_FREQ_RATIO,
							IXAudio2VoiceCallback	* pCallback_p		= nullptr,
							XAUDIO2_VOICE_SENDS		* pSendList_p		= nullptr,
							XAUDIO2_EFFECT_CHAIN	* pEffectChain_p	= nullptr ){

			m_iAfterLastIndexUsed = 0;
			m_waveFormat = waveFormat_p;

			HRESULT hr = S_OK;

			for( int it = 0; it < NTRACKS; ++it ){

				hr = pXAudio_p->CreateSourceVoice(	&m_pVoices[it], &m_waveFormat,
													iFlags_p, fMaxFrquenceRatio_p, pCallback_p,
													pSendList_p, pEffectChain_p );
				if( hr == S_OK ) continue;

				assert( hr != XAUDIO2_E_INVALID_CALL );
				assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
				assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

				// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
				assert ( hr == XAUDIO2_E_DEVICE_INVALIDATED
						 ||
						 hr == ERROR_NOT_FOUND );

				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------
		// load an audio buffer to the available voice queue
		//------------------------------------------------------------------------
		bool LoadAudioToQueue( XAUDIO2_BUFFER * pAudioBuffer_p, XAUDIO2_BUFFER_WMA * pWMABuffer_p = nullptr ){

			HRESULT hr =
				m_pVoices[m_iAfterLastIndexUsed]->SubmitSourceBuffer( pAudioBuffer_p, pWMABuffer_p );

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
		// play methods
		//------------------------------------------------------------------------
		bool PlayQueued( UINT32 iOperationSet_p = 1, UINT32 iFlags_p = 0 ){

			XAUDIO2_VOICE_STATE state = {0};
			m_pVoices[m_iAfterLastIndexUsed]->GetState(&state);

			if( state.BuffersQueued == 0 ) return true;

			m_pVoices[m_iAfterLastIndexUsed]->Stop();

			HRESULT hr =
			m_pVoices[m_iAfterLastIndexUsed]->Start( iFlags_p, iOperationSet_p );

			NextIndex();

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
		bool PlayAudio( XAUDIO2_BUFFER * pAudioBuffer_p, UINT32 iOperationSet_p = 1, UINT32 iFlags_p = 0 ){

			XAUDIO2_VOICE_STATE state = {0};
			m_pVoices[m_iAfterLastIndexUsed]->GetState(&state);

			if( state.BuffersQueued > 0 ){

				m_pVoices[m_iAfterLastIndexUsed]->Stop();
				m_pVoices[m_iAfterLastIndexUsed]->FlushSourceBuffers();
				m_pVoices[m_iAfterLastIndexUsed]->SubmitSourceBuffer( pAudioBuffer_p );
			}
			else{

				m_pVoices[m_iAfterLastIndexUsed]->Stop();
				m_pVoices[m_iAfterLastIndexUsed]->SubmitSourceBuffer( pAudioBuffer_p );
			}

			HRESULT hr =
			m_pVoices[m_iAfterLastIndexUsed]->Start( iFlags_p, iOperationSet_p );

			NextIndex();

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
		bool StopTracks( UINT32 iOperationSet_p = 1, UINT32 iFlags_p = 0 ){

			HRESULT hr = S_OK;

			for( int it = 0; it < NTRACKS; ++it ){

				hr =
				m_pVoices[it]->Stop( iFlags_p, iOperationSet_p );

				if( hr == S_OK ) continue;

				assert( hr != XAUDIO2_E_INVALID_CALL );
				assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
				assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

				// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
				assert (hr == XAUDIO2_E_DEVICE_INVALIDATED
						||
						hr == ERROR_NOT_FOUND );

				return false;
			}

			return true;
		}

		//------------------------------------------------------------------------
		// vol ctrl
		//------------------------------------------------------------------------
		bool SetVolume( float fVol_p, UINT32 iOperationSet_p = 1 ){

			HRESULT hr = S_OK;

			for( int it = 0; it < NTRACKS; ++it ){

				hr = m_pVoices[it]->SetVolume( fVol_p, iOperationSet_p );

				if( hr == S_OK ) continue;

				assert( hr != XAUDIO2_E_INVALID_CALL );
				assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
				assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

				// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
				assert ( hr == XAUDIO2_E_DEVICE_INVALIDATED
					||
					hr == ERROR_NOT_FOUND );

				return false;
			}

			return true;
		}

		float GetVolume() const {

			float fVol;
			HRESULT hr = return m_pVoices[0]->GetVolume(&fVol);

			if( hr == S_OK ) return fVol;

			assert( hr != XAUDIO2_E_INVALID_CALL );
			assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
			assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

			// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
			assert ( hr == XAUDIO2_E_DEVICE_INVALIDATED
				||
				hr == ERROR_NOT_FOUND );

			return -1.0f;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void DestroyTracks(){

			if( m_pVoices[0] ){ // means voices are initialized

				for( int it = 0; it < NTRACKS; ++it ){

					//m_pVoices[it]->Stop();
					m_pVoices[it]->DestroyVoice();
				}

				m_pVoices[0] = nullptr;
			}
		}

	private:

		WAVEFORMATEX			m_waveFormat;
		IXAudio2SourceVoice *	m_pVoices[NTRACKS];
		UINT					m_iAfterLastIndexUsed;

		//------------------------------------------------------------------------
		// circular index increment
		//------------------------------------------------------------------------
		void NextIndex(){

			++m_iAfterLastIndexUsed;
			m_iAfterLastIndexUsed %= NTRACKS;
		}
	};
}
#pragma warning( pop )
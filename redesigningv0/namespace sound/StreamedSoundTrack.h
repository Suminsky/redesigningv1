#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/24
	created:	24:6:2013   17:43
	file:		StreamedSoundTrack.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	TODO: error codes, .inl for the implementation
				TODO: Delayed "initialization": stream till all music is on mem, than switch to just having the
				entire shit looping

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// private includes
#include "SourceReader.h"

// standard includes
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h" // ta dando pau se coloca antes do SourceReader.h

namespace sound{

	template< int NBUFFERS = 3, int NBUFFBYTESIZE = 65536 > //4096>// // 3 x 64KB
	class StreamedSoundTrack{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		StreamedSoundTrack()
			:
		m_pVoice(nullptr)
		{}

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		~StreamedSoundTrack(){

			if( m_pVoice ) m_pVoice->DestroyVoice();
		}

		//------------------------------------------------------------------------
		// Fills all buffers with data
		//------------------------------------------------------------------------
		bool Initialize(	IXAudio2 * pXAudio_p, const wchar_t * szFilename_p,
							UINT32 iFlags_p = 0, float fMaxFrquenceRatio_p = XAUDIO2_DEFAULT_FREQ_RATIO,
							IXAudio2VoiceCallback	* pCallback_p		= nullptr,
							XAUDIO2_VOICE_SENDS		* pSendList_p		= nullptr,
							XAUDIO2_EFFECT_CHAIN	* pEffectChain_p	= nullptr ){

			m_bPlaying = false;

			// create reader

			if(
				m_reader.Initialize( szFilename_p )
				!= true ) return false;

			// create voice

			HRESULT hr =
			pXAudio_p->CreateSourceVoice(	&m_pVoice, &m_reader.GetWaveFormat(),
											iFlags_p, fMaxFrquenceRatio_p, pCallback_p, pSendList_p, pEffectChain_p );
			if( hr != S_OK ){

				assert( hr != XAUDIO2_E_INVALID_CALL );
				assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
				assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

				// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
				assert ( hr == XAUDIO2_E_DEVICE_INVALIDATED
						||
						hr == ERROR_NOT_FOUND );

				return false;
			}

			//// fill buffers

			//XAUDIO2_BUFFER audioBuffer = {0};
			//UINT32 nBytesWritten = 0;
			//for( m_iCurrentBuffer = 0; m_iCurrentBuffer < NBUFFERS; ++m_iCurrentBuffer ){

			//	bool bStreamComplete = 
			//		m_reader.ReadAllWaveData(	m_buffers[m_iCurrentBuffer],
			//									NBUFFBYTESIZE,
			//									nBytesWritten );

			//	audioBuffer.AudioBytes = nBytesWritten;
			//	audioBuffer.pAudioData = m_buffers[m_iCurrentBuffer];
			//	audioBuffer.Flags = bStreamComplete ? XAUDIO2_END_OF_STREAM : 0;

			//	hr =
			//	m_pVoice->SubmitSourceBuffer( &audioBuffer );

			//	if( hr != S_OK ) return false;

			//	if( bStreamComplete ){
			//		
			//		m_reader.ResetReadingPosition();
			//		break;
			//	}
			//}

			//m_iCurrentBuffer %= NBUFFERS;
			m_iCurrentBuffer = 0;

			return true;
		}

		void DestroyVoice(){

			if( m_pVoice ){
				
				m_pVoice->DestroyVoice();
				m_pVoice = nullptr;
			}
		}

		//------------------------------------------------------------------------
		// Fill voice till it queued NBUFFERS
		//------------------------------------------------------------------------
		bool Stream(){

			XAUDIO2_VOICE_STATE voiceState = {0};
			m_pVoice->GetState( &voiceState );

			XAUDIO2_BUFFER audioBuffer = {0};
			UINT32 nBytesWritten = 0;

			while( voiceState.BuffersQueued < NBUFFERS ){//since it never feel due small buffers, its stuck

				bool bStreamComplete = 
					m_reader.ReadNextWaveSample(	m_buffers[m_iCurrentBuffer],
													NBUFFBYTESIZE,
													nBytesWritten );

				if( nBytesWritten == 0 ){ // error on submitting
					if( bStreamComplete ){
						m_reader.ResetReadingPosition();
						break;
					}
					else{
						//MessageBox(0, "Music sample size bigger than supported buffer..STUCK", "sound error", MB_E );
						continue;
					}
				}

				audioBuffer.AudioBytes = nBytesWritten;
				audioBuffer.pAudioData = m_buffers[m_iCurrentBuffer];
				audioBuffer.Flags = bStreamComplete ? XAUDIO2_END_OF_STREAM : 0;

				HRESULT hr =
				m_pVoice->SubmitSourceBuffer( &audioBuffer );
				if( hr != S_OK ){
					assert ( hr == XAUDIO2_E_DEVICE_INVALIDATED
						||
						hr == ERROR_NOT_FOUND );
					return false;
				}

				if( bStreamComplete ){

					m_reader.ResetReadingPosition();
					break;
				}

				m_pVoice->GetState( &voiceState );

				++m_iCurrentBuffer;
				m_iCurrentBuffer %= NBUFFERS;
			}

			return true;
		}

		//------------------------------------------------------------------------
		// play control
		//------------------------------------------------------------------------
		bool Play( UINT32 iOperationSet_p = 1, UINT32 iFlags_p = 0 ){

			HRESULT hr =
				m_pVoice->Start( iFlags_p, iOperationSet_p );

			if( hr == S_OK ){

				m_bPlaying = true;
				return true;
			}

			assert( hr != XAUDIO2_E_INVALID_CALL );
			assert( hr != XAUDIO2_E_XMA_DECODER_ERROR ); // XBOX specific
			assert( hr != XAUDIO2_E_XAPO_CREATION_FAILED ); // an effect failed to instantiate

			// gotta be	XAUDIO2_E_DEVICE_INVALIDATED
			assert (hr == XAUDIO2_E_DEVICE_INVALIDATED
				||
				hr == ERROR_NOT_FOUND );

			return false;
		}
		bool Stop( UINT32 iOperationSet_p = 1, UINT32 iFlags_p = 0 ){

			HRESULT hr =
				m_pVoice->Stop( iFlags_p, iOperationSet_p );

			if( hr == S_OK ){

				m_bPlaying = false;
				return true;
			}

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
		// vol ctrl
		//------------------------------------------------------------------------
		bool SetVolume( float fVol_p, UINT32 iOperationSet_p = 1 ){

			HRESULT hr =

			m_pVoice->SetVolume( fVol_p, iOperationSet_p );

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

		float GetVolume() const {

			float fVol;
			HRESULT hr = return m_pVoice->GetVolume(&fVol);

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
		// getters
		//------------------------------------------------------------------------
		WAVEFORMATEX & GetWaveFormat(){ return m_reader.GetWaveFormat(); }
		LONGLONG GetTotalDurationMS(){ return m_reader.GetDurationMS(); }
		UINT GetMaxSize(){ return m_reader.GetMaxSize(); }

	private:

		IXAudio2SourceVoice * m_pVoice;
		UINT m_iCurrentBuffer;
		SourceReader m_reader;
		bool m_bPlaying;
		BYTE m_buffers[NBUFFERS][NBUFFBYTESIZE];
	};
}
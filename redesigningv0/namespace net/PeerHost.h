#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/05
	created:	5:7:2013   23:48
	file:		PeerHost.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	The dude that will handle a socket. Face it as a socket with stats...for now that is

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "Socket.h"
#include <assert.h>

//#include "../namespace win/FileLogger.h"

namespace net{

	enum E_CONFIG{

		E_CONFIG_DATABUFFERSIZE = 256,
		E_CONFIG_SENDFREQUENCYPERSEC = 10,
		E_CONFIG_TIMEOUTMSEC = 10000
	};

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	struct DataBuffer{

		int currentUsed;
		unsigned char data[E_CONFIG_DATABUFFERSIZE];

		void Set( unsigned char * pData, unsigned int iSize ){

			assert( iSize <= E_CONFIG_DATABUFFERSIZE );

			memcpy( data, pData, iSize );
			
			currentUsed = iSize;
		}
		void Queue( unsigned char * pData, unsigned int iSize ){

			assert( currentUsed + iSize <= E_CONFIG_DATABUFFERSIZE );

			memcpy( &data[currentUsed], pData, iSize );

			currentUsed += iSize;
		}
	};

	struct SequenceInfo{

		int m_iSequenceSend;
		int m_iSequenceReceived;

		int m_iSequenceReliableSend;
		int m_iSequenceReliableReceived;
		int m_iSequenceReliableAcknowledgedReceived;

		SequenceInfo( int iInitial_p ){

			m_iSequenceSend =
			m_iSequenceReceived =

			m_iSequenceReliableSend =
			m_iSequenceReliableReceived =
			m_iSequenceReliableAcknowledgedReceived = iInitial_p;
		}
	};

	//template< int NBUFFERSSIZE = 256 >
	class P2PConnection{

	public:

		enum E_STATE{

			E_STATE_OFF = 0,
			E_STATE_ATEMPTINGCONNECTION,
			E_STATE_CONNECTED,
			E_STATE_DISCONNECTED,
			E_STATE_CRITICALERROR
		};

		Socket_UDP_NonBlocking_IPv4::E_ERROR m_eLastSocketError;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		P2PConnection( Socket_UDP_NonBlocking_IPv4 & hostSocket_p )
			:
		m_socketRef(hostSocket_p),
		m_eState(E_STATE_OFF),
		m_fSendFrequencySec(1.0f/E_CONFIG_SENDFREQUENCYPERSEC),
		m_remoteAddress(0,0),
		m_timeoutSec(E_CONFIG_TIMEOUTMSEC/1000),
		m_eLastSocketError(Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE),
		m_sequences(0)
		{}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Update( double /*dAccum_p*/, double dDelta_p ){

			switch( m_eState ){
			case E_STATE_OFF:
				break;
			case E_STATE_ATEMPTINGCONNECTION:
				OnAttemptingConnection( dDelta_p );
				break;
			case E_STATE_CONNECTED:
				OnConnected( dDelta_p );
				break;
			case E_STATE_CRITICALERROR:
				return;
			}
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Connect( const Address_HostOrder_IPv4 & remoteAddress_p ){

			m_dTimeSinceLastReceiving = 0.0;
			m_dTimeSinceLastSending = 0.0;
			m_remoteAddress = remoteAddress_p;
			static const char s_char[] = {"wanna connect"};
			m_bufferedUserDataTosend.Set( (unsigned char*)s_char, sizeof(s_char));
			m_bufferedReliableDataToSend.currentUsed = 0;
			m_eState = E_STATE_ATEMPTINGCONNECTION;
		}
		void ReConnect(){ // same thing, but uses the already settled address
			m_dTimeSinceLastReceiving = 0.0;
			m_dTimeSinceLastSending = 0.0;
			static const char s_char[] = {"wanna reconnect"};
			m_bufferedUserDataTosend.Set( (unsigned char*)s_char, sizeof(s_char));
			m_eState = E_STATE_ATEMPTINGCONNECTION;
		}
		void Disconnect(){

			m_eState = E_STATE_DISCONNECTED;
			m_dTimeSinceLastReceiving = 0.0;
			m_dTimeSinceLastSending = 0.0;
		}
		void OnAttemptingConnection( double dDelta_p ){

			m_dTimeSinceLastReceiving += dDelta_p;

			// check if data was received

			Socket_UDP_NonBlocking_IPv4::E_ERROR eError = Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE;
			Address_HostOrder_IPv4 remoteAddress(0,0);

			if( m_socketRef.Receive(	(int)m_bufferedRemoteDataReceived.currentUsed, remoteAddress,
										m_bufferedRemoteDataReceived.data, E_CONFIG_DATABUFFERSIZE,
										&eError ) )
			{

				// accept only the expected address

				if( remoteAddress == m_remoteAddress ){

					m_eState = E_STATE_CONNECTED;
					m_dTimeSinceLastReceiving = 0.0;

					return;
				}
				else{

					m_bufferedRemoteDataReceived.currentUsed = 0; // discard data received, if any
				}
			}
			else{

				if( eError != Socket_UDP_NonBlocking_IPv4::E_ERROR_WOULDBLOCK ){

					m_eLastSocketError = eError;

					switch( eError ){
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_TIMETOLIVEEXPIRED:
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_REMOTEUNREACHABLE:
						m_eState = E_STATE_DISCONNECTED;
						return;
					default:
						m_eState = E_STATE_CRITICALERROR;
						return;
					}
				}
			}

			// send connection request

			m_dTimeSinceLastSending += dDelta_p;

			// send on fixed time intervals only

			if( m_dTimeSinceLastSending >= m_fSendFrequencySec ){

				//m_dTimeSinceLastSending -= m_fSendFrequencySec;

				if( m_socketRef.SendTo(m_remoteAddress, m_bufferedUserDataTosend.data, m_bufferedUserDataTosend.currentUsed, &eError ) ){

					m_dTimeSinceLastSending  -= m_fSendFrequencySec;
				}
				else{

					if( eError != Socket_UDP_NonBlocking_IPv4::E_ERROR_WOULDBLOCK ){
						
						m_eLastSocketError = eError;

						switch( eError ){
						case Socket_UDP_NonBlocking_IPv4::E_ERROR_REMOTEUNREACHABLE:
							m_eState = E_STATE_DISCONNECTED;
							return;
						case Socket_UDP_NonBlocking_IPv4::E_ERROR_NETWORKNOTREACHABLE:
						case Socket_UDP_NonBlocking_IPv4::E_ERROR_NOIDEA:
							m_dTimeSinceLastSending -= m_fSendFrequencySec;
							return; // keep trying
						case Socket_UDP_NonBlocking_IPv4::E_ERROR_REMOTECANTREACHHOST:
						default:
							m_eState = E_STATE_CRITICALERROR;
							return;
						}
					}
				} // error handling
			} // fixed interval
		}


		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void OnConnected( double dDelta_p ){

			m_dTimeSinceLastReceiving += dDelta_p;
			m_dTimeSinceLastSending += dDelta_p;

			Socket_UDP_NonBlocking_IPv4::E_ERROR eError = Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE;

			if( m_dTimeSinceLastSending >= m_fSendFrequencySec ){

				// check if theres user data to send, if theres not, send a heart beat to maintain connection

				if( !m_bufferedUserDataTosend.currentUsed
					&&
					!m_bufferedReliableDataToSend.currentUsed ){

					static const char s_char[] = {"heart beat"};
					m_bufferedUserDataTosend.Set( (unsigned char*)s_char, sizeof(s_char));
				}

				if( m_bufferedUserDataTosend.currentUsed ){

					if( m_socketRef.SendTo( m_remoteAddress, m_bufferedUserDataTosend.data, m_bufferedUserDataTosend.currentUsed, &eError ) ){

						m_dTimeSinceLastSending -= m_fSendFrequencySec;
						m_bufferedUserDataTosend.currentUsed = 0;
					}
				}
				if( m_bufferedReliableDataToSend.currentUsed ){

					if( m_socketRef.SendTo( m_remoteAddress, m_bufferedReliableDataToSend.data, m_bufferedReliableDataToSend.currentUsed, &eError ) ){

						m_dTimeSinceLastSending -= m_fSendFrequencySec;
						m_bufferedReliableDataToSend.currentUsed = 0;
					}
				}

				// error handling
				if( eError != Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE
					&&
					eError != Socket_UDP_NonBlocking_IPv4::E_ERROR_WOULDBLOCK ){

					m_eLastSocketError = eError;

					switch( eError ){
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_REMOTEUNREACHABLE:
						m_eState = E_STATE_DISCONNECTED;
						return;
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_NETWORKNOTREACHABLE:
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_NOIDEA:
						m_dTimeSinceLastReceiving  -= m_fSendFrequencySec;
						return; // keep trying
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_REMOTECANTREACHHOST:
					default:
						m_eState = E_STATE_CRITICALERROR;
						return;
					}
				}
			}// send frequency

			//---

			eError = Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE;
			Address_HostOrder_IPv4 remoteAddress(0,0);

			if( m_socketRef.Receive(	(int)m_bufferedRemoteDataReceived.currentUsed, remoteAddress,
										m_bufferedRemoteDataReceived.data, E_CONFIG_DATABUFFERSIZE,
										&eError ) )
			{

				// accept only the expected address

				if( remoteAddress == m_remoteAddress ){

					m_dTimeSinceLastReceiving = 0.0;

					// TODO: heart beats should not be treated as user data
					return;
				}
				else{

					m_bufferedRemoteDataReceived.currentUsed = 0; // discard data received, if any
				}
			}
			else{

				if( eError != Socket_UDP_NonBlocking_IPv4::E_ERROR_WOULDBLOCK ){

					m_eLastSocketError = eError;

					switch( eError ){
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_TIMETOLIVEEXPIRED:
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_REMOTEUNREACHABLE:
						m_eState = E_STATE_DISCONNECTED;
						return;
					default:
						m_eState = E_STATE_CRITICALERROR;
						return;
					}
				}
			}// error handling
		}


		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		E_STATE GetState(){ return m_eState; }

		//------------------------------------------------------------------------
		// new - testing
		//------------------------------------------------------------------------
		bool DataReceived()const{

			return m_bufferedRemoteDataReceived.currentUsed != 0;
		}
		DataBuffer & GetReceivedData(){

			return m_bufferedRemoteDataReceived;
		}


		bool SendData( unsigned char * pDataBuff_p, int iSize_p ){

			// returns false if buffer is full(no space for the new data)
			// this means data on the buffer is not being sent as fast as user
			// is filling (or not being sent at all)

			if( m_bufferedUserDataTosend.currentUsed + iSize_p > E_CONFIG_DATABUFFERSIZE ){
				
				return false;
			}

			m_bufferedUserDataTosend.Queue( pDataBuff_p, iSize_p );
			return true;
		}
		bool SendReliableData( unsigned char * pDataBuff_p, int iSize_p ){

			if( m_bufferedReliableDataToSend.currentUsed + iSize_p > E_CONFIG_DATABUFFERSIZE ){

				return false;
			}

			m_bufferedReliableDataToSend.Queue( pDataBuff_p, iSize_p );
			return true;
		}

	private:

		E_STATE m_eState;
		Socket_UDP_NonBlocking_IPv4 & m_socketRef;
		Address_HostOrder_IPv4 m_remoteAddress;

		double m_dTimeSinceLastReceiving;
		double m_dTimeSinceLastSending;
				
		float m_fSendFrequencySec;
		float m_timeoutSec;			// both connection attempt and "maintenance"

		DataBuffer m_bufferedUserDataTosend;
		DataBuffer m_bufferedRemoteDataReceived;
		DataBuffer m_bufferedReliableDataToSend;

		SequenceInfo m_sequences;

		//------------------------------------------------------------------------
		// avoid warning
		//------------------------------------------------------------------------
		P2PConnection & operator = ( const P2PConnection & other_p );
	};
}
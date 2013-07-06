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

namespace net{

	template< int NSIZE >
	struct DataBuffer{

		unsigned int currentUsed;
		unsigned char data[NSIZE];

		void Set( unsigned char * pData, unsigned int iSize ){

			assert( iSize <= NSIZE );

			memcpy( data, pData, iSize );
			
			currentUsed = iSize;
		}
		void Queue( unsigned char * pData, unsigned int iSize ){

			assert( currentUsed + iSize <= NSIZE );

			memcpy( &data[currentUsed], pData, iSize );

			currentUsed += iSize;
		}
	};

	template< int NBUFFERSSIZE = 256 >
	class P2PConnection{

	public:

		enum E_STAT{

			E_STAT_OFF,
			E_STAT_ATEMPTINGCONNECTION,
			E_STAT_CONNECTED,
			E_STAT_DISCONNECTED,
			E_STAT_CRITICALERROR
		};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		P2PConnection( const Socket_UDP_NonBlocking_IPv4 & hostSocket_p )
			:
		m_socketRef(hostSocket_p),
		m_eStat(E_STAT_OFF),
		m_fSendFrequencySec(1.0f/10.0f),
		m_timeoutSec(10.0f){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Update( double dAccum_p, double dDelta_p ){

			switch( m_eStat ){
			case E_STAT_OFF:
				break;
			case E_STAT_ATEMPTINGCONNECTION:
				OnAttemptingConnection( dDelta_p );
				break;
			case E_STAT_CONNECTED:
				OnConnected( dDelta_p );
				break;
			case E_STAT_CRITICALERROR:
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
			static const s_char[] = {"wanna connect"};
			m_bufferedUserDataTosend.Set( s_char, sizeof(s_char));
			m_eStat = E_STAT_ATEMPTINGCONNECTION;
		}
		void OnAttemptingConnection( double dDelta_p ){

			m_dTimeSinceLastReceiving += dDelta_p;

			// check if data was received

			Socket_UDP_NonBlocking_IPv4::E_ERROR eError = Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE;
			Address_HostOrder_IPv4 remoteAddress(0,0);

			if( m_socketRef.Receive(	m_bufferedRemoteDataReceived.currentUsed, remoteAddress,
										m_bufferedRemoteDataReceived.data, NBUFFERSSIZE,
										&eError ) )
			{

				// accept only the expected address

				if( remoteAddress == m_remoteAddress ){

					m_eStat = E_STAT_CONNECTED;
					m_dTimeSinceLastReceiving = 0.0;

					return;
				}
				else{

					m_bufferedRemoteDataReceived.currentUsed = 0; // discard data received, if any
				}
			}
			else{

				if( eError != Socket_UDP_NonBlocking_IPv4::E_ERROR_WOULDBLOCK ){

					switch( eError ){
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_TIMETOLIVEEXPIRED:
					case Socket_UDP_NonBlocking_IPv4::E_ERROR_REMOTEUNREACHABLE:
						m_eStat = E_STAT_DISCONNECTED;
						return;
					default:
						m_eStat = E_STAT_CRITICALERROR;
						return;
					}
				}
			}

			// send connection request

			m_dTimeSinceLastSending += dDelta_p;

			// send on fixed time intervals only

			if( m_dTimeSinceLastSending >= m_fSendFrequencySec ){

				m_dTimeSinceLastSending -= m_fSendFrequencySec;

				if( m_socketRef.SendTo(m_remoteAddress, m_bufferedUserDataTosend, m_bufferedUserDataTosend.currentUsed, &eError ) ){

					m_dTimeSinceLastSending = 0.0;
				}
				else{

					if( eError != Socket_UDP_NonBlocking_IPv4::E_ERROR_WOULDBLOCK ){

						switch( eError ){
						case Socket_UDP_NonBlocking_IPv4::E_ERROR_REMOTEUNREACHABLE:
							m_eStat = E_STAT_DISCONNECTED;
							return;
						case Socket_UDP_NonBlocking_IPv4::E_ERROR_NETWORKNOTREACHABLE:
						case Socket_UDP_NonBlocking_IPv4::E_ERROR_NOIDEA:
							m_dTimeSinceLastReceiving = 0.0;
							return; // keep trying
						case Socket_UDP_NonBlocking_IPv4::E_ERROR_REMOTECANTREACHHOST:
						default:
							m_eStat = E_STAT_CRITICALERROR;
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

			if( m_dTimeSinceLastSending >= m_fSendFrequencySec ){

				m_dTimeSinceLastSending -= m_fSendFrequencySec;

				// check if theres user data to send, if theres not, send a heart beat to maintain connection

			}
		}

	private:

		E_STAT m_eStat;
		Socket_UDP_NonBlocking_IPv4 & m_socketRef;
		Address_HostOrder_IPv4 m_remoteAddress;

		double m_dTimeSinceLastReceiving;
		double m_dTimeSinceLastSending;
				
		float m_fSendFrequencySec;
		float m_timeoutSec;			// both connection attempt and "maintenance"

		DataBuffer<NBUFFERSSIZE> m_bufferedUserDataTosend;
		DataBuffer<NBUFFERSSIZE> m_bufferedRemoteDataReceived;
	};
}
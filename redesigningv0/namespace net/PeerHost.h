#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/05
	created:	5:7:2013   23:48
	file:		PeerHost.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	The dude that will handle a socket. Face it as a socket with stats...for now that is // OLD, it mutated completely TODO

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes
#include "Socket.h"
#include <assert.h>
#include "Packet.h"

#include "../namespace win/FileLogger.h"

namespace net{

	enum E_CONFIG{

		E_CONFIG_DATABUFFERSIZE = 256,
		E_CONFIG_SENDFREQUENCYPERSEC = 10,
		E_CONFIG_TIMEOUTMSEC = 10000
	};

	struct SequenceInfo{

		int m_iSequenceSend;
		int m_iSequenceReceived;

		int m_iSequenceReliableSend;
		int m_iSequenceLastReliableReceived;
		int m_iSequenceLastReliableAcknowledgedReceived;

		SequenceInfo( int iInitial_p ){

			m_iSequenceSend =
			m_iSequenceReceived =

			m_iSequenceLastReliableReceived = iInitial_p;
			m_iSequenceLastReliableAcknowledgedReceived = iInitial_p; // not being used..

			m_iSequenceReliableSend = iInitial_p + 1; // since the reliable seq is incremented only when an ack is received, it have to
													 //	start greater than m_iSequenceLastReliableReceived (only seq > recv seq are accepted)
		}
	};

	//========================================================================
	// 
	//========================================================================
	class P2PConnection{

	public:

		// remove the burden of template arguments
		typedef DataBuffer<E_CONFIG_DATABUFFERSIZE>			DataBuffer;
		typedef Packet<E_CONFIG_DATABUFFERSIZE>				Packet;
		typedef ReceivingPacket<E_CONFIG_DATABUFFERSIZE>	ReceivingPacket;

		enum E_STATE{

			E_STATE_OFF = 0,
			E_STATE_ATEMPTINGCONNECTION,
			E_STATE_CONNECTED,
			E_STATE_DISCONNECTED,
			E_STATE_CRITICALERROR
		};

		enum{

			E_CHANNEL_UNRELIABLE = 0,
			E_CHANNEL_RELIABLE = 1,
		};

		Socket_UDP_NonBlocking_IPv4::E_ERROR m_eLastSocketError;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		P2PConnection( Socket_UDP_NonBlocking_IPv4 & hostSocket_p, int iPacketID_p )
			:
		m_socketRef(hostSocket_p),
		m_eState(E_STATE_OFF),
		m_fSendFrequencySec(1.0f/E_CONFIG_SENDFREQUENCYPERSEC),
		m_remoteAddress(0,0),
		//m_timeoutSec(E_CONFIG_TIMEOUTMSEC/1000),
		m_eLastSocketError(Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE),
		m_sequences(0),
		m_bufferedUserDataTosend(iPacketID_p), m_bufferedReliableDataToSend(iPacketID_p), m_lastReceivedPacket(iPacketID_p)
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
		// connecting
		//------------------------------------------------------------------------
		void Connect( const Address_HostOrder_IPv4 & remoteAddress_p ){

			m_dTimeSinceLastReceiving = 0.0;
			m_dTimeSinceLastSending = 0.0;
			m_remoteAddress = remoteAddress_p;
			//static const char s_char[] = {"wanna connect"};
			//m_bufferedUserDataTosend.Set( (unsigned char*)s_char, sizeof(s_char));
			m_bufferedRemoteDataReceived.currentUsed = 0;
			m_bufferedUserDataTosend.SetChannel( E_PACKET_CHANNEL_CONNECT );
			m_bufferedReliableDataToSend.SetChannel(E_CHANNEL_RELIABLE); // reliable are always channel 1
			//m_bufferedReliableDataToSend.currentUsed = 0;
			m_eState = E_STATE_ATEMPTINGCONNECTION;
		}
		void ReConnect(){ // same thing, but uses the already settled address
			m_dTimeSinceLastReceiving = 0.0;
			m_dTimeSinceLastSending = 0.0;
			//static const char s_char[] = {"wanna reconnect"};
			//m_bufferedUserDataTosend.Set( (unsigned char*)s_char, sizeof(s_char));
			m_bufferedUserDataTosend.SetChannel( E_PACKET_CHANNEL_CONNECT );
			m_eState = E_STATE_ATEMPTINGCONNECTION;
		}
		void Disconnect(){

			m_eState = E_STATE_DISCONNECTED;
			m_dTimeSinceLastReceiving = 0.0;
			m_dTimeSinceLastSending = 0.0;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		E_STATE GetState(){ return m_eState; }

		//------------------------------------------------------------------------
		// new - testing
		// user interaction
		//------------------------------------------------------------------------
		bool DataReceived()const{

			return m_bufferedRemoteDataReceived.currentUsed != 0;
			//return m_bufferedRemoteDataReceived
		}
		DataBuffer & GetReceivedData(){

			return m_bufferedRemoteDataReceived;
		}


		bool SendData( unsigned char * pDataBuff_p, int iSize_p ){

			// returns false if buffer is full(no space for the new data)
			// this means data on the buffer is not being sent as fast as user
			// is filling (or not being sent at all)

			if( m_bufferedUserDataTosend.GetCurrentUsedBytes() + iSize_p > E_CONFIG_DATABUFFERSIZE ){
				
				return false;
			}

			m_bufferedUserDataTosend.Queue( pDataBuff_p, iSize_p );
			return true;
		}
		bool SendReliableData( unsigned char * pDataBuff_p, int iSize_p ){

			// NOTE: can only send ONE reliable packet until ack is received
			if( m_bufferedReliableDataToSend.GetCurrentUsedDataBytes() ) return false;

			if( m_bufferedReliableDataToSend.GetCurrentUsedBytes() + iSize_p > E_CONFIG_DATABUFFERSIZE ){

				return false;
			}

			m_bufferedReliableDataToSend.Queue( pDataBuff_p, iSize_p );
			return true;
		}

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void OnAttemptingConnection( double dDelta_p ){

			m_dTimeSinceLastReceiving += dDelta_p;

			// check if data was received

			Socket_UDP_NonBlocking_IPv4::E_ERROR eError = Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE;
			Address_HostOrder_IPv4 remoteAddress(0,0);


			if( m_socketRef.Receive(	(int &)m_lastReceivedPacket.m_nReceivedBytes, remoteAddress,
										m_lastReceivedPacket.GetData(), m_lastReceivedPacket.E_TOTAL_SIZE,
										&eError ) )
			{
				// accept only the expected address

				if( remoteAddress == m_remoteAddress ){

					Header header;
					if( m_lastReceivedPacket.IsValid( header ) ){

						m_dTimeSinceLastReceiving = 0.0;

						m_eState = E_STATE_CONNECTED;
						return;
					}
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

				if( m_socketRef.SendTo(m_remoteAddress, m_bufferedUserDataTosend.GetData(), m_bufferedUserDataTosend.GetCurrentUsedBytes(), &eError ) ){

					m_dTimeSinceLastSending  = 0.0;
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
		// sending & receiving
		//------------------------------------------------------------------------
		void OnConnected( double dDelta_p ){

			m_dTimeSinceLastReceiving	+= dDelta_p;
			m_dTimeSinceLastSending		+= dDelta_p;

			if( m_dTimeSinceLastSending >= m_fSendFrequencySec ){

				HandleSending();

			}// send frequency

			//---

			HandleReceiving();
		}

		//------------------------------------------------------------------------
		// on connection stuff
		//------------------------------------------------------------------------
		void HandleSending(){

			Socket_UDP_NonBlocking_IPv4::E_ERROR eError = Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE;

			// check if theres user data to send, if theres not, send a heart beat to maintain connection

			if( !m_bufferedUserDataTosend.GetCurrentUsedDataBytes()
				&&
				!m_bufferedReliableDataToSend.GetCurrentUsedDataBytes() ){

					m_bufferedUserDataTosend.SetChannel( E_PACKET_CHANNEL_KEEPALIVE );
					// the sending seq on a keepalive is not important
					m_bufferedUserDataTosend.UpdateHeader( -1, m_sequences.m_iSequenceLastReliableReceived );

					if( m_socketRef.SendTo( m_remoteAddress, m_bufferedUserDataTosend.GetData(), m_bufferedUserDataTosend.GetCurrentUsedBytes(), &eError ) ){

						m_dTimeSinceLastSending = 0.0;
					}
			}
			else{

				// send unreliable data

				if( m_bufferedUserDataTosend.GetCurrentUsedDataBytes() ){

					// change channel to unreliable (it may be settled to keep alive)

					m_bufferedUserDataTosend.SetChannel( E_CHANNEL_UNRELIABLE );
					m_bufferedUserDataTosend.UpdateHeader( ++m_sequences.m_iSequenceSend, m_sequences.m_iSequenceLastReliableReceived );

					if( m_socketRef.SendTo( m_remoteAddress, m_bufferedUserDataTosend.GetData(), m_bufferedUserDataTosend.GetCurrentUsedBytes(), &eError ) ){

						m_dTimeSinceLastSending = 0.0; //-= m_fSendFrequencySec;
						m_bufferedUserDataTosend.ClearUserData();
					}
				}

				// send reliable data

				if( m_bufferedReliableDataToSend.GetCurrentUsedDataBytes() ){

					// cant increment reliable seq here, it is sent till ack is received, it "have" to keep its seq cause its the same packet

					m_bufferedReliableDataToSend.UpdateHeader( m_sequences.m_iSequenceReliableSend, m_sequences.m_iSequenceLastReliableReceived );

					if( m_socketRef.SendTo( m_remoteAddress, m_bufferedReliableDataToSend.GetData(), m_bufferedReliableDataToSend.GetCurrentUsedBytes(), &eError ) ){

						m_dTimeSinceLastSending = 0.0; //-= m_fSendFrequencySec;
						//m_bufferedReliableDataToSend.ClearUserData(); only clears when ack is received
						win::UniqueFileLogger()<<"net: enviando reliable"<<SZ_NEWLINE;
						win::UniqueFileLogger().FlushToFile();
					}
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
		}
		void HandleReceiving(){

			Socket_UDP_NonBlocking_IPv4::E_ERROR eError = Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE;
			Address_HostOrder_IPv4 remoteAddress(0,0);

			if( m_socketRef.Receive(	(int &)m_lastReceivedPacket.m_nReceivedBytes, remoteAddress,
										m_lastReceivedPacket.GetData(), m_lastReceivedPacket.E_TOTAL_SIZE,
										&eError ) )
			{
				// accept only the expected address

				if( remoteAddress == m_remoteAddress ){

					Header header;
					if( m_lastReceivedPacket.IsValid( header ) ){

						m_dTimeSinceLastReceiving = 0.0;

						// check if reliable

						switch( header.iChannel ){

						case E_PACKET_CHANNEL_KEEPALIVE:
						case E_PACKET_CHANNEL_CONNECT:
							break;

						case E_CHANNEL_UNRELIABLE:

							// drop if older or repeated

							if( header.iSendingSequence > m_sequences.m_iSequenceReceived ){

								m_sequences.m_iSequenceReceived = header.iSendingSequence;

								m_bufferedRemoteDataReceived.Queue( m_lastReceivedPacket.GetUserData(), m_lastReceivedPacket.GetReceivedUserDataBytes() );
							}

							break;

						case E_CHANNEL_RELIABLE:

							if( header.iSendingSequence > m_sequences.m_iSequenceLastReliableReceived ){

								m_sequences.m_iSequenceLastReliableReceived = header.iSendingSequence;

								m_bufferedRemoteDataReceived.Queue( m_lastReceivedPacket.GetUserData(), m_lastReceivedPacket.GetReceivedUserDataBytes() );
							}
							//else : not needed, the last reliable received is send as ack forever, till next new reliable is received

							break;
						}	

						// check if acknowledgment of 'sending reliable seq' was received

						if( header.iAckReceivedSequence == m_sequences.m_iSequenceReliableSend ){

							m_sequences.m_iSequenceLastReliableAcknowledgedReceived = header.iAckReceivedSequence; // not being used... 

							// if ack is received, then now we can stop sending the reliable packet
							
							m_bufferedReliableDataToSend.ClearUserData();
							++m_sequences.m_iSequenceReliableSend;

							win::UniqueFileLogger()<<"net:ack de reliable"<<SZ_NEWLINE;
							win::UniqueFileLogger().FlushToFile();
						}
					}

					return;
				}
				else{

					BREAKHERE;
					//m_bufferedRemoteDataReceived.currentUsed = 0; // discard data received, if any
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
		

		E_STATE m_eState;

		Socket_UDP_NonBlocking_IPv4 &	m_socketRef;
		Address_HostOrder_IPv4			m_remoteAddress;

		double m_dTimeSinceLastReceiving;
		double m_dTimeSinceLastSending;
				
		float m_fSendFrequencySec;
		//float m_timeoutSec;			// both are connection attempt, keepalive and sending frequence

		SequenceInfo m_sequences;

		Packet			m_bufferedUserDataTosend;
		ReceivingPacket m_lastReceivedPacket;
		DataBuffer		m_bufferedRemoteDataReceived;
		Packet			m_bufferedReliableDataToSend;
		
		//------------------------------------------------------------------------
		// avoid warning
		//------------------------------------------------------------------------
		P2PConnection & operator = ( const P2PConnection & other_p );
	};
}
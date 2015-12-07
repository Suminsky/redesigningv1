#include "P2PConnection.h"

using namespace net;

net::P2PConnection::P2PConnection( Socket_UDP_NonBlocking_IPv4 & hostSocket_p, int iPacketID_p )
	:
	m_socketRef(hostSocket_p),
	m_eState(E_STATE_OFF),
	m_fSendFrequencySec(1.0f/E_CONFIG_SENDFREQUENCYPERSEC),
	m_remoteAddress(0,0),
	//m_timeoutSec(E_CONFIG_TIMEOUTMSEC/1000),
	m_eLastSocketError(Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE),
	m_sequences(0),
	m_bufferedUserDataTosend(iPacketID_p), m_bufferedReliableDataToSend(iPacketID_p), m_lastReceivedPacket(iPacketID_p)
{

}

void net::P2PConnection::Connect( const Address_HostOrder_IPv4 & remoteAddress_p )
{
	m_dTimeSinceLastReceiving = 0.0;
	m_dTimeSinceLastSending = 0.0;

	m_remoteAddress = remoteAddress_p;

	m_bufferedRemoteDataReceived.currentUsed = 0;

	m_bufferedUserDataTosend.SetChannel( E_PACKET_CHANNEL_CONNECT );
	m_bufferedReliableDataToSend.SetChannel(E_CHANNEL_RELIABLE);

	m_eState = E_STATE_ATEMPTINGCONNECTION;
}

void net::P2PConnection::ReConnect()
{
	// same thing, but uses the already settled address

	m_dTimeSinceLastReceiving = 0.0;
	m_dTimeSinceLastSending = 0.0;

	m_bufferedRemoteDataReceived.currentUsed = 0;

	m_bufferedUserDataTosend.SetChannel( E_PACKET_CHANNEL_CONNECT );
	// m_bufferedReliableDataToSend do never change its channel

	m_eState = E_STATE_ATEMPTINGCONNECTION;
}

void net::P2PConnection::Disconnect()
{
	// TODO: send disconnect packet?-> now on DisconnectNicely

	m_eState = E_STATE_DISCONNECTED;
	m_dTimeSinceLastReceiving = 0.0;
	m_dTimeSinceLastSending = 0.0;
}

void net::P2PConnection::DisconnectNicely()
{
	m_dTimeSinceLastReceiving = 0.0;
	m_dTimeSinceLastSending = 0.0;

	m_bufferedRemoteDataReceived.currentUsed = 0;
	m_bufferedUserDataTosend.ClearUserData();

	m_bufferedUserDataTosend.SetChannel( E_PACKET_CHANNEL_DISCONNECT );

	m_eState = E_STATE_DISCONNECTINGNICELY;
	m_fDisconnectingTimeOut = E_CONFIG_DISCONNECTIONLINGERTIMEOUT/1000.0;
}

void net::P2PConnection::Update( double /*dAccum_p*/, double dDelta_p )
{
	switch( m_eState ){

	case E_STATE_OFF:
	case E_STATE_DISCONNECTED:
		break;

	case E_STATE_ATEMPTINGCONNECTION:
		OnAttemptingConnection( dDelta_p );
		break;

	case E_STATE_CONNECTED:
		OnConnected( dDelta_p );
		break;

	case E_STATE_DISCONNECTINGNICELY:
		OnAttemptingNiceDisconnection( dDelta_p );
		break;

	case E_STATE_CRITICALERROR:
		return;
	}
}

bool net::P2PConnection::SendData( unsigned char * pDataBuff_p, int iSize_p )
{
	if( m_eState != E_STATE_CONNECTED ) return false;

	// returns false if buffer is full(no space for the new data)
	// this means data on the buffer is not being sent as fast as user
	// is filling (or not being sent at all)

	if( m_bufferedUserDataTosend.GetCurrentUsedBytes() + iSize_p + E_PACKET_USERDATA_HEADER_SIZE > E_CONFIG_DATABUFFERSIZE ){

		return false;
	}

	m_bufferedUserDataTosend.Queue( pDataBuff_p, iSize_p );
	return true;
}

bool net::P2PConnection::SendReliableData( unsigned char * pDataBuff_p, int iSize_p )
{
	if( m_eState != E_STATE_CONNECTED ) return false;

	// NOTE: can only send ONE reliable packet until ack is received, and user data is not buffered
	// on the packet, which means only one call to this func till it is received by the remote,
	// which is..lame
	// If Id buffer reliable data, it could be received on the other side before the buffering, meaning
	// the new data would be considered already received, since the seq IDs are by packet, not by "user data buffered".

	if( m_bufferedReliableDataToSend.GetCurrentUsedDataBytes() ) return false;

	if( m_bufferedReliableDataToSend.GetCurrentUsedBytes() + iSize_p + E_PACKET_USERDATA_HEADER_SIZE  > E_CONFIG_DATABUFFERSIZE ){

		return false;
	}

	m_bufferedReliableDataToSend.Queue( pDataBuff_p, iSize_p );
	return true;
}

//========================================================================
//========================================================================
//========================================================================
//========================================================================
void net::P2PConnection::OnAttemptingNiceDisconnection( double dDelta_p )
{
	// receiving

	m_dTimeSinceLastReceiving += dDelta_p;

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

				// immediately disconnect if disconnect packet received
				// while already sending disconnects

				if( header.iChannel == E_PACKET_CHANNEL_DISCONNECT )
					m_eState = E_STATE_DISCONNECTED;

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




	// sending

	m_dTimeSinceLastSending	+= dDelta_p;

	// send how much time left before disconnecting on the disconnect packets
	
	float timeToDisconnect = m_fDisconnectingTimeOut - (float)m_dTimeSinceLastReceiving;
	SendData( (unsigned char*) &timeToDisconnect, sizeof(int) );

	HandleSending_NoUserData();

	m_bufferedUserDataTosend.ClearUserData();

	// handle timeout

	if( m_dTimeSinceLastReceiving > m_fDisconnectingTimeOut ){

		m_eState = E_STATE_DISCONNECTED;
	}
}

void net::P2PConnection::OnAttemptingConnection( double dDelta_p )
{
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

				if( header.iChannel != E_PACKET_CHANNEL_DISCONNECT ) // UNTESTED test now
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

	m_dTimeSinceLastSending		+= dDelta_p;

	HandleSending_NoUserData();

	// handle timeout

	if( m_dTimeSinceLastReceiving > E_CONFIG_TIMEOUTMSEC/1000.0 ){

		m_eState = E_STATE_DISCONNECTED;
	}
}

void net::P2PConnection::HandleSending_NoUserData()
{
	Socket_UDP_NonBlocking_IPv4::E_ERROR eError = Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE;

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

void net::P2PConnection::OnConnected( double dDelta_p )
{
	m_dTimeSinceLastReceiving	+= dDelta_p;
	m_dTimeSinceLastSending		+= dDelta_p;

	if( m_dTimeSinceLastSending >= m_fSendFrequencySec ){

		HandleSending();

	}// send frequency

	//---

	HandleReceiving();

	// handle timeout

	if( m_dTimeSinceLastReceiving > E_CONFIG_TIMEOUTMSEC/1000.0 ){

		m_eState = E_STATE_DISCONNECTED;
	}
}

void net::P2PConnection::HandleSending()
{
	Socket_UDP_NonBlocking_IPv4::E_ERROR eError = Socket_UDP_NonBlocking_IPv4::E_ERROR_NONE;

	// check if theres user data to send, if theres not, send a heart beat to maintain connection

	if( !m_bufferedUserDataTosend.GetCurrentUsedDataBytes()
		&&
		!m_bufferedReliableDataToSend.GetCurrentUsedDataBytes() ){

			m_bufferedUserDataTosend.SetChannel( E_PACKET_CHANNEL_KEEPALIVE );

			// the sending seq on a keepalive is not important (-1)
			m_bufferedUserDataTosend.UpdateHeader( -1, m_sequences.m_iReliableRemote );

			if( m_socketRef.SendTo( m_remoteAddress, m_bufferedUserDataTosend.GetData(), m_bufferedUserDataTosend.GetCurrentUsedBytes(), &eError ) ){

				m_dTimeSinceLastSending = 0.0;
			}
	}
	else{

		// send unreliable data

		if( m_bufferedUserDataTosend.GetCurrentUsedDataBytes() ){

			// change channel to unreliable (it may be settled to keep alive)

			m_bufferedUserDataTosend.SetChannel( E_CHANNEL_UNRELIABLE );
			m_bufferedUserDataTosend.UpdateHeader( ++m_sequences.m_iLocal, m_sequences.m_iReliableRemote );

			if( m_socketRef.SendTo( m_remoteAddress, m_bufferedUserDataTosend.GetData(), m_bufferedUserDataTosend.GetCurrentUsedBytes(), &eError ) ){

				m_dTimeSinceLastSending = 0.0;
				m_bufferedUserDataTosend.ClearUserData();
			}
		}

		// send reliable data

		if( m_bufferedReliableDataToSend.GetCurrentUsedDataBytes() ){

			// cant increment reliable seq here, it is sent till ack is received, it MUST keep its seq cause its the same packet

			m_bufferedReliableDataToSend.UpdateHeader( m_sequences.m_iReliableLocal, m_sequences.m_iReliableRemote );

			if( m_socketRef.SendTo( m_remoteAddress, m_bufferedReliableDataToSend.GetData(), m_bufferedReliableDataToSend.GetCurrentUsedBytes(), &eError ) ){

				m_dTimeSinceLastSending = 0.0;
				// m_bufferedReliableDataToSend.ClearUserData(); only clears when ack is received
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

void net::P2PConnection::HandleReceiving()
{
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

				case E_PACKET_CHANNEL_DISCONNECT:

					// the amount of time before remote gives up is set on the user data,
					// use this as the amount of time to try acknowledge it
					
					m_fDisconnectingTimeOut = (float)(*m_lastReceivedPacket.GetUserData());

					m_eState = E_STATE_DISCONNECTINGNICELY;

					break;

				case E_CHANNEL_UNRELIABLE:

					// drop if older or repeated

					if( header.iSendingSequence > m_sequences.m_iRemote ){

						m_sequences.m_iRemote = header.iSendingSequence;

						// TODO: what if the buffer is full?

						if( m_bufferedRemoteDataReceived.currentUsed + m_lastReceivedPacket.GetReceivedUserDataBytes() > E_CONFIG_DATABUFFERSIZE )
							return;

						m_bufferedRemoteDataReceived.Queue( m_lastReceivedPacket.GetUserData(), m_lastReceivedPacket.GetReceivedUserDataBytes() );
					}

					break;

				case E_CHANNEL_RELIABLE:

					if( header.iSendingSequence > m_sequences.m_iReliableRemote ){

						m_sequences.m_iReliableRemote = header.iSendingSequence;

						// TODO: what if the buffer is full?

						if( m_bufferedRemoteDataReceived.currentUsed + m_lastReceivedPacket.GetReceivedUserDataBytes() > E_CONFIG_DATABUFFERSIZE )
							return;

						m_bufferedRemoteDataReceived.Queue( m_lastReceivedPacket.GetUserData(), m_lastReceivedPacket.GetReceivedUserDataBytes() );
					}
					//else : not needed, the last reliable received is send as ack forever, till next new reliable is received

					break;
				}	

				// check if acknowledgment of 'sending reliable seq' was received

				if( header.iAckReceivedSequence == m_sequences.m_iReliableLocal ){

					m_sequences.m_iReliAckRemote = header.iAckReceivedSequence; // not being used... 

					// if ack is received, then now we can stop sending the reliable packet

					m_bufferedReliableDataToSend.ClearUserData();
					++m_sequences.m_iReliableLocal;
				}
			}

			return;
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
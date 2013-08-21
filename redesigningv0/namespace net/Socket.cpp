#include "Socket.h"

#include <WinSock2.h>
#include <assert.h>

using namespace net;

bool net::Socket_UDP_NonBlocking_IPv4::Init( unsigned short usPort_p, E_ERROR * peError_p /*= nullptr */ )
{
	// creates the socket

	m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if( m_socket != INVALID_SOCKET ){

		// bind socket to the given port

		SOCKADDR_IN address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl( INADDR_ANY );	// binds to all available addresses on this host
		address.sin_port = htons( usPort_p );  

		if( bind( m_socket, (sockaddr *) &address, sizeof(SOCKADDR_IN) ) == NO_ERROR ){

			// set socket to nonblocking

			u_long iBlockingMode = 1; // if != 0 means non blocking

			if( ioctlsocket( m_socket, FIONBIO, &iBlockingMode ) == NO_ERROR ){

				return true;
			}
		}
	}

	DBG(
		int result = WSAGetLastError();
	
	assert( result != WSANOTINITIALISED );
	assert( result != WSAEAFNOSUPPORT );
	assert( result != WSAEINPROGRESS );
	assert( result != WSAEINVAL );
	//
	assert( result != WSAEACCES ); // socket options does not permit
	assert( result != WSAEADDRNOTAVAIL ); // invalid port number
	assert( result != WSAEFAULT );
	assert( result != WSAENOTSOCK );
	)

	if( peError_p ){

		NDBG( int result = WSAGetLastError(); );

		switch( result ){
		case WSAEMFILE:			*peError_p = E_ERROR_TOOMANYSOCKETSOPEN;		break;
		case WSAENOBUFS:		*peError_p = E_ERROR_NOSYSRESOURCESAVAILABLE;	break;
			//
		case WSAEADDRINUSE:		*peError_p = E_ERROR_EXCLUSIVEADDRESSINUSE;		break;
			//
		case WSAENETDOWN:		*peError_p = E_ERROR_SHITGOTFUCKED;				break;
		default:				*peError_p = E_ERROR_NOIDEA;					break;
		}
	}

	return false;
}

bool net::Socket_UDP_NonBlocking_IPv4::Destroy( E_ERROR * peError_p /*= nullptr */ )
{
	if( closesocket( m_socket ) == NO_ERROR ){

		m_socket = INVALID_SOCKET;

		return true;

	}else{

		DBG(
			int result = WSAGetLastError();
		
		assert( result != WSANOTINITIALISED );
		assert( result != WSAENOTSOCK );
		assert( result != WSAEINPROGRESS );
		assert( result != WSAEINTR );
		assert( result != WSAEINVAL );
		)
		

		if( peError_p ){

			NDBG( int result = WSAGetLastError(); );

			switch( result ){
			case WSAEWOULDBLOCK:	*peError_p = E_ERROR_WOULDBLOCK;				break;
			case WSAENETDOWN:		*peError_p = E_ERROR_SHITGOTFUCKED;				break;
			default:				*peError_p = E_ERROR_NOIDEA;					break;
			}
		}

		return false;
	}
}

bool net::Socket_UDP_NonBlocking_IPv4::Receive(	int & nBytesRead_p, Address_HostOrder_IPv4 & remoteAddress_out_p, BYTE * pDataBuffer_p, UINT nMaxBytesToRead_p, E_ERROR * peError_p /*= nullptr */ )
{
	sockaddr_in senderAddress;
	int iSenderAddressSize = sizeof(senderAddress);

	nBytesRead_p =
		recvfrom( m_socket, (char*)pDataBuffer_p, nMaxBytesToRead_p, 0, (sockaddr*)&senderAddress, &iSenderAddressSize );

	if( nBytesRead_p != SOCKET_ERROR ){

		remoteAddress_out_p.Set( senderAddress );

		return true;
	}
	else{

		nBytesRead_p = 0;

		DBG(
			int result = WSAGetLastError();
		
		assert( result != WSANOTINITIALISED );
		assert( result != WSAEFAULT ); // buffer invalid address or size of from not enough
		assert( result != WSAEINTR );
		assert( result != WSAEINPROGRESS );
		assert( result != WSAEINVAL );
		assert( result != WSAEISCONN ); // the socket is connected, it cant be with this function
		assert( result != WSAETIMEDOUT ); // conection droped
		assert( result != WSAEOPNOTSUPP ); // stream OOB stuff
		assert( result != WSAEADDRNOTAVAIL ); // invalid port number
		assert( result != WSAESHUTDOWN ); // socket was shutdown()
		assert( result != WSAENOTSOCK );
		)
		if( peError_p ){

			NDBG( int result = WSAGetLastError(); );

			switch( result ){
			case WSAENETRESET:		*peError_p = E_ERROR_TIMETOLIVEEXPIRED;				break;
			case WSAEWOULDBLOCK:	*peError_p = E_ERROR_WOULDBLOCK;					break;
			case WSAEMSGSIZE:		*peError_p = E_ERROR_MESSAGETRUNCATEDDUEBUFFERSIZE;	break;
			case WSAECONNRESET:		*peError_p = E_ERROR_REMOTEUNREACHABLE;				break;
								// "On a UDP-datagram socket this error indicates a previous
								// send operation resulted in an ICMP Port Unreachable message."
			case WSAENETDOWN:		*peError_p = E_ERROR_SHITGOTFUCKED;					break;
			default:				*peError_p = E_ERROR_NOIDEA;						break;
			}
		}

		return false;
	}
}

bool net::Socket_UDP_NonBlocking_IPv4::SendTo( const Address_HostOrder_IPv4 & remoteAddress_in_p, const BYTE * pData_p, UINT nDataBytes_p, E_ERROR * peError_p /*= nullptr */ )
{
	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_addr.s_addr =  htonl( remoteAddress_in_p.GetAddress() );
	to.sin_port = htons( remoteAddress_in_p.GetPort() );

	int nBytesSent =
		sendto( m_socket, (char*)pData_p, nDataBytes_p, 0, (sockaddr*) &to, sizeof(to) );

	if( nBytesSent != SOCKET_ERROR ){

		// consider error if not all data was sent

		if( (UINT)nBytesSent != nDataBytes_p ){

			if( peError_p ){

				*peError_p = E_ERROR_NOTALLDATASENT;
			}

			return false;
		}

		return true;
	}
	else{

		DBG(
			int result = WSAGetLastError();
		
		assert( result != WSANOTINITIALISED );
		assert( result != WSAEFAULT ); // buffer invalid address or size of to too small
		assert( result != WSAEINTR );
		assert( result != WSAEINPROGRESS );
		assert( result != WSAEINVAL );
		assert( result != WSAEISCONN ); // the socket is connected, it cant be with this function
		assert( result != WSAETIMEDOUT ); // conection droped
		assert( result != WSAEOPNOTSUPP ); // stream OOB stuff
		assert( result != WSAEAFNOSUPPORT ); // invalid family
		//assert( result != WSAEACCES ); // socket options does not permit (BROADCAST)
		//assert( result != WSAEADDRNOTAVAIL ); // invalid address
		assert( result != WSAESHUTDOWN ); // socket was shutdown()
		assert( result != WSAENOTSOCK );
		assert( result != WSAENETRESET ); // connection has been broken
		assert( result != WSAEMSGSIZE ); // message size is bigger than supported
		assert( result != WSAEDESTADDRREQ ); // receiver address required
		assert( result != WSAETIMEDOUT );
		)
		if( peError_p ){

			NDBG( int result = WSAGetLastError(); );

			switch( result ){

			case WSAEWOULDBLOCK:	*peError_p = E_ERROR_WOULDBLOCK;					break;
			case WSAECONNRESET:		*peError_p = E_ERROR_REMOTECANTREACHHOST;			break;
				// "The remote host was unable to deliver a previously sent UDP
				// datagram and responded with a "Port Unreachable" ICMP packet.
				// The application should close the socket as it is no longer usable.
			case WSAEACCES:
			case WSAEADDRNOTAVAIL:	*peError_p = E_ERROR_ADDRESSINVALID;				break;
			case WSAEHOSTUNREACH:	*peError_p = E_ERROR_REMOTEUNREACHABLE;				break;	// cant reach to right now
			case WSAENETUNREACH:	*peError_p = E_ERROR_NETWORKNOTREACHABLE;			break;	// network cant be reached by this host right now
			case WSAENETDOWN:		*peError_p = E_ERROR_SHITGOTFUCKED;					break;
			case WSAENOBUFS: //system runed out of buffers
			default:				*peError_p = E_ERROR_NOIDEA;						break;
			}
		}

		return false;
	}
}

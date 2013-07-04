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
	);

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

		);

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

#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/30
	created:	30:6:2013   2:31
	file:		Address.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#pragma warning( push )
#pragma warning( disable: 4005 ) //'_WINSOCKAPI_' : macro redefinition, cause Im defining on the prep myself to avoid win.h including old winsock
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_addr is deprecated
#include <WinSock2.h>
#pragma warning( pop )
#include <stdint.h>

// private includes

namespace net{

	class Address_HostOrder_IPv4{

	public:

		union address{

			struct{
				uint8_t	ucD;
				uint8_t	ucC;
				uint8_t	ucB;
				uint8_t	ucA;
			}octets;

			int32_t iInteger;
		};

		//------------------------------------------------------------------------
		// ctors
		//------------------------------------------------------------------------
		Address_HostOrder_IPv4( uint8_t a_p, uint8_t b_p, uint8_t c_p, uint8_t d_p, uint16_t port_p );
		Address_HostOrder_IPv4( int32_t iAddress_p, uint16_t usPort_p );
		Address_HostOrder_IPv4( const char * szAddressDotedForm_p, uint16_t port_p );
		Address_HostOrder_IPv4( const sockaddr_in & address_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Set( const sockaddr_in & address_p ){

			m_address.iInteger = ntohl( address_p.sin_addr.s_addr );
			m_usPort = ntohs( address_p.sin_port );
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		int32_t GetAddress() const{

			return m_address.iInteger;
		}
		uint16_t GetPort() const{

			return m_usPort;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool operator == ( const Address_HostOrder_IPv4 & other_p ){

			if( m_address.iInteger == other_p.GetAddress() 
				&&
				m_usPort == other_p.GetPort() )
				return true;

			return false;
		}

	private:

		address m_address;
		uint16_t m_usPort;
	};
}
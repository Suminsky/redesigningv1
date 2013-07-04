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
#include <memory>
#include <stdint.h>
#include <WinSock2.h>


// private includes

namespace net{

	class Address_HostOrder_IPv4{

	public:

		//------------------------------------------------------------------------
		// ctors
		//------------------------------------------------------------------------
		Address_HostOrder_IPv4( uint8_t a_p, uint8_t b_p, uint8_t c_p, uint8_t d_p, uint16_t port_p ){

			m_address.octets.ucA = a_p;
			m_address.octets.ucB = b_p;
			m_address.octets.ucC = c_p;
			m_address.octets.ucD = d_p;

			m_usPort = port_p;
		}
		Address_HostOrder_IPv4( int32_t iAddress_p, uint16_t usPort_p ){

			m_address.iInteger = iAddress_p;

			m_usPort = usPort_p;
		}
		Address_HostOrder_IPv4( const char * szAddressDotedForm_p, uint16_t port_p ){

			m_address.iInteger = inet_addr( szAddressDotedForm_p );

			m_usPort = port_p;
		}

	private:

		union address{

			struct{
				uint8_t	ucA;
				uint8_t	ucB;
				uint8_t	ucC;
				uint8_t	ucD;
			}octets;

			int32_t iInteger;

		} m_address;

		uint16_t m_usPort;
	};

	typedef std::shared_ptr<Address_HostOrder_IPv4> shared_Address_IPv4_ptr;
	typedef std::weak_ptr<Address_HostOrder_IPv4> weak_Address_IPv4_ptr;
}
#include "PCHF.h"
#include "Address.h"

using namespace net;

net::Address_HostOrder_IPv4::Address_HostOrder_IPv4( uint8_t a_p, uint8_t b_p, uint8_t c_p, uint8_t d_p, uint16_t port_p )
{
	m_address.octets.ucD = d_p;
	m_address.octets.ucC = c_p;
	m_address.octets.ucB = b_p;
	m_address.octets.ucA = a_p;
	
	m_usPort = port_p;
}

net::Address_HostOrder_IPv4::Address_HostOrder_IPv4( int32_t iAddress_p, uint16_t usPort_p )
{
	m_address.iInteger = iAddress_p;

	m_usPort = usPort_p;
}

net::Address_HostOrder_IPv4::Address_HostOrder_IPv4( const char * szAddressDotedForm_p, uint16_t port_p )
{
	//InetPton()
	m_address.iInteger = ntohl(inet_addr( szAddressDotedForm_p ));

	m_usPort = port_p;
}

net::Address_HostOrder_IPv4::Address_HostOrder_IPv4( const sockaddr_in & address_p )
{
	m_address.iInteger = ntohl( address_p.sin_addr.s_addr );
	m_usPort = ntohs( address_p.sin_port );
}

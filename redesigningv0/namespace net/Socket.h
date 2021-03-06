#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/29
	created:	29:6:2013   21:35
	file:		Socket_UDP_NonBlocking_IPv4.cpp
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	Represents/Manages a UDP, non-blocking, IPv4 socket. Since socket modes and protocols arent to be mixed anyway, 
				I think its more convenient to have a class that handles a specific socket case instead of one that
				can became anything (protocols x modes x ip address version).

	� Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../namespace gen/gen_macros.h"
#include "Address.h"

namespace net{

	class Socket_UDP_NonBlocking_IPv4{

	public:

		enum E_ERROR{

			E_ERROR_NONE, // uninitialized
			E_ERROR_TOOMANYSOCKETSOPEN,
			E_ERROR_NOSYSRESOURCESAVAILABLE,
			E_ERROR_EXCLUSIVEADDRESSINUSE,
			E_ERROR_WOULDBLOCK,
			E_ERROR_INVALIDPORT,
			E_ERROR_ADDRESSINVALID,
			E_ERROR_TIMETOLIVEEXPIRED, // ??
			E_ERROR_MESSAGETRUNCATEDDUEBUFFERSIZE,
			E_ERROR_SOCKETISGONE,
			E_ERROR_NOTALLDATASENT,
			E_ERROR_REMOTEUNREACHABLE,
			E_ERROR_REMOTECANTREACHHOST,
			E_ERROR_NETWORKNOTREACHABLE,
			E_ERROR_NOIDEA,
			E_ERROR_SHITGOTFUCKED
		};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		Socket_UDP_NonBlocking_IPv4()
			:
		m_socket(INVALID_SOCKET){}
		
		//------------------------------------------------------------------------
		// creates, binds the socket to a given port and set it to non blocking mode.
		//------------------------------------------------------------------------
		bool Init( unsigned short usPort_p, E_ERROR * peError_p = nullptr );

		//------------------------------------------------------------------------
		// kills and clean/closes the socket, so it can be reused
		// NOTE: returns would block if certain linger options are set. something
		// to do with graceful shutdown. check msdn for info.
		// In the case of wouldblock, the socket cant be reused cause the port is
		// still active.
		//------------------------------------------------------------------------
		bool Destroy( E_ERROR * peError_p = nullptr );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool Receive(	int & nBytesRead_p, Address_HostOrder_IPv4 & remoteAddress_out_p,
						BYTE * pDataBuffer_p, UINT nMaxBytesToRead_p,
						E_ERROR * peError_p = nullptr );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool SendTo(	const Address_HostOrder_IPv4 & remoteAddress_in_p,
						const BYTE * pData_p, UINT nDataBytes_p,
						E_ERROR * peError_p = nullptr );
	private:

		SOCKET m_socket;
		// TODO: should I hold the port?
	};
}
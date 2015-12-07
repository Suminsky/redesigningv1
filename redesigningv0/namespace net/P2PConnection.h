#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/05
	created:	5:7:2013   23:48
	file:		PeerHost.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	The dude that will handle a socket. Face it as a socket with stats...for now that is // OLD, it mutated completely TODO
				
				Note that it receives a socket at initialization, but it cant be used as a connection with a single remote, using multiples
				of those with a single socket (although this is the point, one socket connecting with all remotes).
				Because when a socket receives, it discards the info from the sockets, so if you receive in one connection, and its not
				from the remote connected, the other connections will lose it.
				So a layer bellow is needed, which handles receiving and distributing to other P2PConnection

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
		E_CONFIG_TIMEOUTMSEC = 10000,
		E_CONFIG_DISCONNECTIONLINGERTIMEOUT = 2000
	};

	struct SequenceInfo{

		int m_iLocal;
		int m_iRemote;

		int m_iReliableLocal;
		int m_iReliableRemote;
		int m_iReliAckRemote;

		SequenceInfo( int iInitial_p ){

			m_iLocal =
			m_iRemote =

			m_iReliableRemote = iInitial_p;
			m_iReliAckRemote = iInitial_p; // not being used..

			m_iReliableLocal = iInitial_p + 1; // since the reliable seq is incremented only when an ack is received, it have to
													 //	start greater than m_iReliableRemote (only seq > recv seq are accepted)
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
			E_STATE_DISCONNECTINGNICELY,
			E_STATE_CRITICALERROR
		};

		enum E_CHANNEL{

			E_CHANNEL_UNRELIABLE = 0,
			E_CHANNEL_RELIABLE = 1,
		};

		Socket_UDP_NonBlocking_IPv4::E_ERROR m_eLastSocketError;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		P2PConnection( Socket_UDP_NonBlocking_IPv4 & hostSocket_p, int iPacketID_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Update( double /*dAccum_p*/, double dDelta_p );

		//------------------------------------------------------------------------
		// connecting
		//------------------------------------------------------------------------
		void Connect( const Address_HostOrder_IPv4 & remoteAddress_p );
		void ReConnect();
		void Disconnect();
		void DisconnectNicely();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		E_STATE GetState(){ return m_eState; }

		//------------------------------------------------------------------------
		// new - testing
		// user interaction
		//------------------------------------------------------------------------
		bool DataReceived()const{ return m_bufferedRemoteDataReceived.currentUsed != 0;	}
		DataBuffer & GetReceivedData(){	return m_bufferedRemoteDataReceived; }


		bool SendData( unsigned char * pDataBuff_p, int iSize_p );
		bool SendReliableData( unsigned char * pDataBuff_p, int iSize_p );

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void OnAttemptingConnection( double dDelta_p );
		void OnAttemptingNiceDisconnection( double dDelta_p );

			void HandleSending_NoUserData();

		//------------------------------------------------------------------------
		// sending & receiving
		//------------------------------------------------------------------------
		void OnConnected( double dDelta_p );

			//------------------------------------------------------------------------
			// on connection stuff
			//------------------------------------------------------------------------
			void HandleSending();
			void HandleReceiving();

		
		

		E_STATE m_eState;

		Socket_UDP_NonBlocking_IPv4 &	m_socketRef;
		Address_HostOrder_IPv4			m_remoteAddress;

		double m_dTimeSinceLastReceiving;
		double m_dTimeSinceLastSending;
				
		float m_fSendFrequencySec;
		//float m_timeoutSec;			// both are connection attempt, keepalive and sending frequence

		float m_fDisconnectingTimeOut;

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
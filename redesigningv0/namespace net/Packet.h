#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/06
	created:	6:7:2013   2:10
	file:		Packet.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes

namespace net{

	struct Header{

		int iID;
		int iChannel;
		int iSendingSequence;
		int iAckReceivedSequence;
		int nUserDataChunks;
	};

	struct UserDataHeader{

		int iSize;
	};

	enum E_PACKET{

		E_PACKET_HEADER_SIZE				= sizeof(Header),
		E_PACKET_USERDATA_HEADER_SIZE		= sizeof(UserDataHeader),

		E_PACKET_DATA_OFFSET				= E_PACKET_HEADER_SIZE,

		E_PACKET_HEADER_ID_OFFSET			= 0,
		E_PACKET_HEADER_CHANNEL_OFFSET		= sizeof(int),
		E_PACKET_HEADER_SEQ_OFFSET			= sizeof(int)*2,
		E_PACKET_HEADER_ACK_OFFSET			= sizeof(int)*3,
		E_PACKET_HEADER_NDATACHUNKS_OFFSET	= sizeof(int)*4,

		E_PACKET_CHANNEL_CONNECT			= -1,  // using negative numbers to interpret as keepalive or connection attempt packets
		E_PACKET_CHANNEL_KEEPALIVE			= -2			
	};	

	//========================================================================
	// 
	//========================================================================
	template< int SIZE >
	class Packet{

	public:

		enum{

			E_TOTAL_SIZE = SIZE,
			E_DATA_SIZE = E_TOTAL_SIZE - E_PACKET_HEADER_SIZE	
		};	

		//------------------------------------------------------------------------
		// set the header
		//------------------------------------------------------------------------
		Packet( int iPacketID_p ){

			assert( sizeof(Header) == sizeof(int)*5 );

			Header header;
			header.iID = iPacketID_p;
			header.iChannel = 0;
			header.iSendingSequence = 0;
			header.iAckReceivedSequence = 0;

			SetChunkAs<Header>( 0, header );

			m_nCurrentUsedBytes = E_PACKET_HEADER_SIZE;
		}

		void SetChannel( int channel_p ){

			SetChunkAs<int>( E_PACKET_HEADER_CHANNEL_OFFSET, channel_p );
		}

		//------------------------------------------------------------------------
		// updates seq and ack seq on the header
		//------------------------------------------------------------------------
		void UpdateHeader( int Seq_p, int AckSeq_p ){

			// assuming no padding on the header (assertion on ctor)

			SetChunkAs<int>( E_PACKET_HEADER_SEQ_OFFSET, Seq_p );
			SetChunkAs<int>( E_PACKET_HEADER_ACK_OFFSET, AckSeq_p );
		}

		//------------------------------------------------------------------------
		// add given data to packet
		// the data will be given a header
		//------------------------------------------------------------------------
		void Queue( unsigned char * pData_p, unsigned int iSize_p ){

			assert( m_nCurrentUsedBytes + iSize_p + E_PACKET_USERDATA_HEADER_SIZE <= E_CONFIG_DATABUFFERSIZE );

			// "encapsulate" user data with a header

			UserDataHeader dataHeader;
			dataHeader.iSize = iSize_p;
			SetChunkAs<UserDataHeader>( m_nCurrentUsedBytes, dataHeader );

			m_nCurrentUsedBytes += E_PACKET_USERDATA_HEADER_SIZE;

			memcpy( &m_data[m_nCurrentUsedBytes], pData_p, iSize_p );

			m_nCurrentUsedBytes += iSize_p;

			// increment user data chunk count

			SetChunkAs<int>( E_PACKET_HEADER_NDATACHUNKS_OFFSET, GetChunkAs<int>(E_PACKET_HEADER_NDATACHUNKS_OFFSET)+1 );
		}

		//------------------------------------------------------------------------
		// set entire data on the packet, overwritten/erasing previous user data
		// (ignoring would be more precise)
		//------------------------------------------------------------------------
		void Set( unsigned char * pData, unsigned int iSize ){

			// "erase"

			m_nCurrentUsedBytes = E_PACKET_HEADER_SIZE;

			assert( m_nCurrentUsedBytes + iSize + E_PACKET_USERDATA_HEADER_SIZE <= E_CONFIG_DATABUFFERSIZE );

			// "encapsulate" user data with a header

			UserDataHeader dataHeader;
			dataHeader.iSize = iSize;
			SetChunkAs<UserDataHeader>( m_nCurrentUsedBytes, dataHeader );

			m_nCurrentUsedBytes += E_PACKET_USERDATA_HEADER_SIZE;

			memcpy( &m_data[m_nCurrentUsedBytes], pData, iSize );

			m_nCurrentUsedBytes += iSize;

			SetChunkAs<int>( E_PACKET_HEADER_NDATACHUNKS_OFFSET, 1 );
		}

		//------------------------------------------------------------------------
		// let just the header
		//------------------------------------------------------------------------
		void ClearUserData(){

			m_nCurrentUsedBytes = E_PACKET_HEADER_SIZE;
			SetChunkAs<int>( E_PACKET_HEADER_NDATACHUNKS_OFFSET, 0 );
		}


		unsigned int GetCurrentUsedBytes() const { return m_nCurrentUsedBytes; }
		unsigned int GetCurrentUsedDataBytes() const { return m_nCurrentUsedBytes - E_PACKET_HEADER_SIZE; }
		unsigned char * GetData() { return m_data; }

	private:

		unsigned int m_nCurrentUsedBytes;
		unsigned char m_data[E_TOTAL_SIZE];

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		TYPE GetChunkAs( unsigned int iByteOffset_p ) const {

			assert( iByteOffset_p + sizeof(TYPE) <= E_TOTAL_SIZE );

			return   *((TYPE*) (&(m_data[iByteOffset_p])) ); // retornar como referencia?
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		void SetChunkAs( unsigned int iByteOffset_p, const TYPE & newData_p ){

			assert( iByteOffset_p + sizeof(TYPE) <= E_TOTAL_SIZE );

			*((TYPE*) (&(m_data[iByteOffset_p])) ) = newData_p;
		}

	};

	//========================================================================
	// 
	//========================================================================
	template< int SIZE >
	class ReceivingPacket{

	public:

		enum{

			E_TOTAL_SIZE = SIZE,
			E_DATA_SIZE = E_TOTAL_SIZE - E_PACKET_HEADER_SIZE,	
		};

		unsigned int m_nReceivedBytes;

		//------------------------------------------------------------------------
		// clears
		//------------------------------------------------------------------------
		ReceivingPacket( int iPacketID_p ){

			assert( sizeof(Header) == sizeof(int)*5 );

			m_iID = iPacketID_p;
			m_nReceivedBytes = 0;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		bool IsValid( Header & outHeader_p) const {

			outHeader_p = GetChunkAs<Header>( 0 );

			if( outHeader_p.iID == m_iID ){

				if( outHeader_p.iChannel >= -2 ){

					return true;
				}
			}

			return false;
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		Header GetHeader()const{

			return GetChunkAs<Header>( 0 );
		}
		unsigned char * GetData() { return m_data; }
		unsigned char * GetUserData(){ return &m_data[E_PACKET_DATA_OFFSET]; }
		unsigned int GetReceivedUserDataBytes() const { return m_nReceivedBytes - E_PACKET_HEADER_SIZE; }

	private:

		int m_iID; // ID to check vality of packet
		unsigned char m_data[E_TOTAL_SIZE];

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		TYPE GetChunkAs( unsigned int iByteOffset_p ) const {

			assert( iByteOffset_p + sizeof(TYPE) <= E_TOTAL_SIZE );

			return   *((TYPE*) (&(m_data[iByteOffset_p])) ); // retornar como referencia?
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		void SetChunkAs( unsigned int iByteOffset_p, const TYPE & newData_p ){

			assert( iByteOffset_p + sizeof(TYPE) <= E_TOTAL_SIZE );

			*((TYPE*) (&(m_data[iByteOffset_p])) ) = newData_p;
		}

	};


	//========================================================================
	// 
	//========================================================================
	template< int SIZE >
	struct DataBuffer{

		int currentUsed;
		unsigned char data[SIZE];

		void Set( unsigned char * pData, unsigned int iSize ){

			assert( iSize <= SIZE );

			memcpy( data, pData, iSize );

			currentUsed = iSize;
		}

		void Queue( unsigned char * pData, unsigned int iSize ){

			assert( currentUsed + iSize <= SIZE );

			memcpy( &data[currentUsed], pData, iSize );

			currentUsed += iSize;
		}
	};


	//========================================================================
	// 
	//========================================================================
	/*template< int SIZE >
	struct UserDataBuffer{

	int currentUsed;
	unsigned char data[SIZE];

	void Set( unsigned char * pData, unsigned int iSize ){

	assert( iSize <= SIZE );

	memcpy( data, pData, iSize );

	currentUsed = iSize;
	}

	void Queue( unsigned char * pData, unsigned int iSize ){

	assert( currentUsed + iSize <= SIZE );

	memcpy( &data[currentUsed], pData, iSize );

	currentUsed += iSize;
	}
	};*/
}
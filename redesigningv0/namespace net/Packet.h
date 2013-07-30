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

	template<int HEADERID, int SIZE>
	class Packet{

	public:

		struct Header{

			int iID;
			int iChannel,;
			int iSendingSequence;
			int iReceivedSequence;
		};

		enum{

			E_HEADER_ID = HEADERID,
			E_TOTAL_SIZE = SIZE,
			E_HEADER_SIZE = sizeof(Header),
			E_DATA_SIZE = E_TOTAL_SIZE - E_HEADER_SIZE,
			E_DATA_OFFSET = E_HEADER_SIZE
		};		

		Packet(){

			m_nCurrentUsedBytes = E_HEADER_SIZE;

			Header header;
			header.iID = E_HEADER_ID;
			header.iChannel = 0;
			header.iSendingSequence = 0;
			header.iReceivedSequence = 0;

			SetChunkAs<Header>( 0, header );
		}

		void UpdateHeader( int Seq, int AckSeq ){

		}

	private:

		unsigned int m_nCurrentUsedBytes;
		unsigned char m_data[E_TOTAL_SIZE];

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		TYPE GetChunkAs( unsigned int iByteOffset_p ){

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
}
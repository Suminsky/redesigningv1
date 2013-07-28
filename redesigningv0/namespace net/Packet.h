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
#include <memory>

// private includes

namespace net{

	template<int TOTALSIZE, int HEADERSIZE>
	class Packet{

		enum{

			E_TOTAL_SIZE = TOTALSIZE,
			E_HEADER_SIZE = HEADERSIZE,
			E_DATA_SIZE = E_TOTAL_SIZE - E_HEADER_SIZE,
			E_DATA_OFFSET = E_HEADER_SIZE
		};

		// header
		// user data

	public:

	private:

		int m_iHeaderSize;
		unsigned char m_data[TOTALSIZE];

	};

	typedef std::shared_ptr<Packet> shared_Packet_ptr;
	typedef std::weak_ptr<Packet> weak_Packet_ptr;
}
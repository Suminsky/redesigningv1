#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/29
	created:	29:6:2013   22:06
	file:		net.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#ifdef NETWORK
#define IFNETWORK(stuff) stuff
#define NETCOMMA() ,
#else
#define IFNETWORK(stuff)
#define NETCOMMA()
#endif

#pragma comment( lib, "Ws2_32.lib")

// standard includes


// private includes

namespace net{

	enum E_ERROR{

		E_ERROR_SYSTEMNOTREADY,
		E_ERROR_VERSIONNOTSUPPORTED,
		E_ERROR_BLOCKCALLINPROGRESS,
		E_ERROR_NOIDEA,
		E_ERROR_SHITGOTFUCKED
	};

	//========================================================================
	// Try to initialize winsock with version 2.2 (current last).
	//========================================================================
	extern bool Init( E_ERROR * peError_p = nullptr );

	//========================================================================
	// Must be called to free sys resources
	//========================================================================
	extern bool Destroy( E_ERROR * peError_p = nullptr );
}
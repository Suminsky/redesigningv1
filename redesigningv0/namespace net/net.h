#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/29
	created:	29:6:2013   22:06
	file:		net.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	ATTENTION!!!
				Winsock2.h must be placed before any windows.h include, OR windows.h needs to be included with WIN32_LEAN_AND_MEAN.
				Well, bullshit, with windows kit 8 theres still lots of shit going on and I dont have a clue where exactly is the problem..
				Heres the thing..winsock is just included if _WINSOCKAPI_ is not defined yet, while winsock2 if _WINSOCK2API_ is not defined yet.
				So one just needs to define _WINSOCKAPI_ globably to avoid the old winsock being included..Note that winsock2 define both macros
				to prevent this (but still is ordering dependent, so we need to supress that warning. 

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

		E_ERROR_NONE,
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
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//WIN Keyboard Input Class
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//Provides a class for handling keyboard input
//Only reason(instead of using GetAsyncKey only) is for detecting
//key releases..
//
//© Giuliano Suminsky Pieta(a.k.a. Icebone1000), rights reserved.
//
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once


#include <windows.h>

namespace win{

	class KeyboardAsyncInput{

		SHORT m_keysStatus[256];
		SHORT m_prevKeysStatus[256];

	public:

		//------------------------------------------------------------------------
		// MUST be called once per frame(at the init of the frame)
		//------------------------------------------------------------------------
		void UpdateKeyStates();

		bool IsKeyDown( const USHORT VK ) const;
		//bool IsKeyHolded(USHORT VK);
		bool KeySinglePress( const  USHORT VK ) const;	//returns true if key wasnt pressed last frame
		bool KeyJustReleased( const USHORT VK ) const;
		bool IsKeyToogledON( const USHORT VK ) const;
	};

	//------------------------------------------------------------------------
	// unique keyboard async input
	//------------------------------------------------------------------------
	extern KeyboardAsyncInput & UniqueKeyboardAsyncInput();
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#include "KeyboardAsyncInput.h"

using namespace win;

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void KeyboardAsyncInput::UpdateKeyStates(){

	//save previous:
	memcpy( m_prevKeysStatus, m_keysStatus, sizeof(SHORT)*256 );

	for( USHORT i = 0; i < 256; i++ ){

		m_keysStatus[i] = GetAsyncKeyState( i );		
	}
}
//----------
bool KeyboardAsyncInput::IsKeyDown(const USHORT VK)const{

	return (m_keysStatus[ VK ] & 0x8000) != 0;
}
//---
//bool KeyboardAsyncInput::IsKeyHolded(USHORT VK){
//
//	return (m_keysStatus[ VK ] & 0x8000) & (m_prevKeysStatus[ VK ] & 0x8000 );
//}
//---
bool KeyboardAsyncInput::KeySinglePress(const USHORT VK)const{

	return (m_keysStatus[ VK ] & 0x8000) && !(m_prevKeysStatus[ VK ] & 0x8000 );
}
//--- 
bool KeyboardAsyncInput::KeyJustReleased( const USHORT VK ) const{
	//return (m_prevKeysStatus[ VK ] & 0x8000)? (!(m_keysStatus[VK] & 0x8000)): 0;
	return ( m_keysStatus[VK] & 0x8000) ? false : ( ( m_prevKeysStatus[ VK ] & 0x8000 ) != 0 );
}
//---
bool KeyboardAsyncInput::IsKeyToogledON( const USHORT VK ) const{

	return (GetKeyState( VK ) & 0x0001);
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

namespace win{
	KeyboardAsyncInput & UniqueKeyboardAsyncInput(){

		static KeyboardAsyncInput input;
		return input;
	}
}
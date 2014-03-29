//================================================================================
//Description:
//================================================================================
//
//
//	(c) Giuliano Suminsky Pieta
/*

	All IDs used on this app.

*/
//================================================================================
#pragma once

namespace win{

	//resources/WM_COMMANDs
#define ID_OK		100
#define ID_CANCEL	101
#define ID_APPLY	102
//#define ID_DX		150
//#define ID_SB		121
		//my menus commom IDs
#define IDM_EXIT	1000
#define IDM_ABOUT	1001
	//windows styles:
#define WS_DISABLED				0x08000000L
#define WS_POPUP				0x80000000L
#define WS_CAPTION				0x00C00000L
#define WS_ACTIVECAPTION		0x0001
#define WS_TABSTOP				0x00010000L
#define WS_VSCROLL				0x00200000L
#define WS_VISIBLE				0x10000000L
//#define WS_EX_RIGHTSCROLLBAR	0x00000000L

	//dialog styles:
#define DS_MODALFRAME		0x80L
#define DS_SETFONT			0x40L

	//controls styles:
//button specific:
#define BS_RADIOBUTTON		0x00000004L
#define BS_AUTORADIOBUTTON	0x00000009L
//edit specific:
#define ES_AUTOHSCROLL		0x0080L
#define ES_AUTOVSCROLL      0x0040L
#define ES_READONLY			0x0800L
#define ES_MULTILINE        0x0004L
//scrollbar specific
#define SBS_VERT			0x0001L

	//virtual keys for letters(not defined on windows):
#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_S 0x53
#define VK_T 0x54
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
	//for numbers:
#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

}
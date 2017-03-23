#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2016/11/01
	created:	1:11:2016   22:16
	file:		RawKeyboardInput.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes


// private includes

namespace win{

	class RawKeyboardInput{

	public:

		static bool RegisterForRawInput(bool ignoreLegacyWMs, HWND wnd){

			RAWINPUTDEVICE device;
			device.usUsagePage = 0x01;
			device.usUsage = 0x06;
			device.dwFlags = ignoreLegacyWMs?RIDEV_NOLEGACY:0;// do not generate legacy messages such as WM_KEYDOWN
			device.hwndTarget = wnd;

			return RegisterRawInputDevices(&device, 1, sizeof(device)) != 0;
		}


		LRESULT OnWM_INPUT(LPARAM lParam_p){
		//case WM_INPUT:
		//{
			char buffer[sizeof(RAWINPUT)] = {};
			UINT size = sizeof(RAWINPUT);
			GetRawInputData((HRAWINPUT)lParam_p, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));

			// extract keyboard raw input data
			RAWINPUT* raw = (RAWINPUT*)buffer;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				const RAWKEYBOARD& rawKB = raw->data.keyboard;
				
				UINT virtualKey = rawKB.VKey;
				UINT scanCode = rawKB.MakeCode;
				UINT flags = rawKB.Flags;

				if (virtualKey == 255)
				{
					// discard "fake keys" which are part of an escaped sequence
					return 0;
				}
				else if (virtualKey == VK_SHIFT)
				{
					// correct left-hand / right-hand SHIFT
					virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
				}
				else if (virtualKey == VK_NUMLOCK)
				{
					// correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
					scanCode = (MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC) | 0x100);
				}

				// e0 and e1 are escape sequences used for certain special keys,
				// such as PRINT and PAUSE/BREAK.
				// see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
				const bool isE0 = ((flags & RI_KEY_E0) != 0);
				const bool isE1 = ((flags & RI_KEY_E1) != 0);

				if (isE1)
				{
					// for escaped sequences, turn the virtual key into the correct scan code using MapVirtualKey.
					// however, MapVirtualKey is unable to map VK_PAUSE (this is a known bug),
					// hence we map that by hand.
					if (virtualKey == VK_PAUSE)
						scanCode = 0x45;
					else
						scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
				}


				switch (virtualKey)
				{
					// right-hand CONTROL and ALT have their e0 bit set
				case VK_CONTROL:
					if (isE0)
						virtualKey = VK_RCONTROL;
					else
						virtualKey = VK_LCONTROL;
					break;

				case VK_MENU:
					if (isE0)
						virtualKey = VK_RMENU; // alt
					else
						virtualKey = VK_LMENU;
					break;

					// NUMPAD ENTER has its e0 bit set
				case VK_RETURN:
					if (isE0)
						virtualKey = VK_SEPARATOR;
					break;

					// the standard INSERT, DELETE, HOME, END, PRIOR and NEXT keys
					// will always have their e0 bit set, but the corresponding keys
					// on the NUMPAD will not.
				case VK_INSERT:
					if (!isE0)
						virtualKey = VK_NUMPAD0;
					break;

				case VK_DELETE:
					if (!isE0)
						virtualKey = VK_DECIMAL;
					break;

				case VK_HOME:
					if (!isE0)
						virtualKey = VK_NUMPAD7;
					break;

				case VK_END:
					if (!isE0)
						virtualKey = VK_NUMPAD1;
					break;

				case VK_PRIOR:
					if (!isE0)
						virtualKey = VK_NUMPAD9;
					break;

				case VK_NEXT:
					if (!isE0)
						virtualKey = VK_NUMPAD3;
					break;

					// the standard arrow keys will always have their e0 bit set, but the
					// corresponding keys on the NUMPAD will not.
				case VK_LEFT:
					if (!isE0)
						virtualKey = VK_NUMPAD4;
					break;

				case VK_RIGHT:
					if (!isE0)
						virtualKey = VK_NUMPAD6;
					break;

				case VK_UP:
					if (!isE0)
						virtualKey = VK_NUMPAD8;
					break;

				case VK_DOWN:
					if (!isE0)
						virtualKey = VK_NUMPAD2;
					break;

					// NUMPAD 5 doesn't have its e0 bit set
				case VK_CLEAR:
					if (!isE0)
						virtualKey = VK_NUMPAD5;
					break;
				}

				// a key can either produce a "make" or "break" scancode.
				// this is used to differentiate between down-presses and releases
				// see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
				//const bool wasUp = ((flags & RI_KEY_BREAK) != 0);
				// 

			}
		//}
		}

	private:

	};
}
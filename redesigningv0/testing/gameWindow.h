#pragma once

#define FULLSCREEN	false
#define N_RENDERBUFFERS 3
#define WINDOW_W 1280
#define WINDOW_H 720

// private includes
#include "../namespace win/win_IDs.h"
#include "../namespace win/Window.h"
#include "../namespace render/dx/SwapChain.h"
#include "../namespace render/dx/Device.h"
#include "../namespace sprite/SpriteRenderer.h"
#include "../namespace game/Game.h"

#include "../namespace win/MouseInput.h"

#include "TestState.h"

class TestGameWindow : public win::Window, public game::Game{

public:

	dx::SwapChain m_swapChain;
	dx::Device m_device;
	sprite::SpriteRenderer m_spriteRenderer;
	win::MouseInput m_mouseInput;

	//------------------------------------------------------------------------
	// ctor
	// it creates the window
	//------------------------------------------------------------------------
	TestGameWindow( HINSTANCE hInst_p );

	//------------------------------------------------------------------------
	// Check current client rect and compares with the client rect the Window
	// current holds, if differs, the buffers are resized.
	// The rects are updated regardless if size differs or not.
	//------------------------------------------------------------------------
	void ResizeIfNeed();

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	virtual LRESULT CALLBACK WndProcHandler( HWND hWnd_p, UINT Msg_p, WPARAM wParam_p, LPARAM lParam_p );
};
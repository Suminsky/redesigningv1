#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2015/06/19
	created:	19:6:2015   14:06
	file:		GameWindow2D_1.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	A prefab game window with initial setup ready for a 2d game;
				Includes sprite and text renderer.

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../namespace win/Window.h"

#include "../namespace render/dx/SwapChain.h"
#include "../namespace render/dx/Device.h"

#include "../namespace game/Game.h"

#include "../namespace sprite/SpriteRenderer.h"
#include "../namespace text/TextRenderer.h"

#include "../namespace sound/sound.h"

//#include "../namespace gen/gen_data.h"

class GameWindow2D_1: public win::Window, public game::Game {

public:

	//------------------------------------------------------------------------
	// ctor
	// it creates the window
	//------------------------------------------------------------------------
	GameWindow2D_1( double dTimeStep_p, HINSTANCE hInst_p,
					int winW_p, int winH_p, const char * szWinName_p );
	virtual ~GameWindow2D_1(){}


	dx::Device				m_device;
	dx::SwapChain			m_swapChain;
	sprite::SpriteRenderer	m_spriteRenderer;
	text::TextRenderer		m_textRenderer;

protected:

	//------------------------------------------------------------------------
	// Check current client rect and compares with the client rect the Window
	// current holds, if differs, the buffers are resized.
	// The rects are updated regardless if size differs or not.
	//------------------------------------------------------------------------
	void ResizeIfNeed();

	//------------------------------------------------------------------------
	// def message handlers
	// use if overwriting VWndProcHandler
	//------------------------------------------------------------------------	
	LRESULT CALLBACK OnWM_CREATE( HWND hWnd_p );
	LRESULT CALLBACK OnWM_CLOSE( HWND hWnd_p );
	LRESULT CALLBACK OnWM_DESTROY();
	LRESULT CALLBACK OnWM_SIZE( HWND hWnd_p );

private:
	//------------------------------------------------------------------------
	// wnd procedure
	//------------------------------------------------------------------------
	virtual LRESULT CALLBACK VWndProcHandler( HWND hWnd_p, UINT Msg_p, WPARAM wParam_p, LPARAM lParam_p );
};
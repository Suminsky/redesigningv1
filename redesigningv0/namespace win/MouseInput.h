#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/05/19
	created:	19:5:2013   4:27
	file:		MouseInput.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	super simple mouse input

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <Windowsx.h>

// private includes

namespace win{

	class MouseInput{

	public:

		struct Buttons{

			bool left, middle, right;

			Buttons(): left(false), middle(false), right(false){}
			
			void Reset(){
				left = middle = right = false;
			}
		};
		struct Pos{

			int x, y;

			Pos():x(0), y(0){}
		};

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		MouseInput(): m_iWheelPrevAmount(0), m_iWheelAmount(0), m_iWheelDelta(0), m_bCaptured(false){}

		//------------------------------------------------------------------------
		// call per frame
		// must be called before wndproc or after game logic
		//------------------------------------------------------------------------
		void Update(){

			m_deltaPos.x = m_pos.x - m_prevPos.x;
			m_deltaPos.y = m_pos.y - m_prevPos.y;

			m_prevPos = m_pos;

			//m_downButtons.Reset();
			//m_releasedButtons.Reset();
			m_pressedButtons.Reset();
			m_releasedButtons.Reset();

			m_iWheelDelta = m_iWheelAmount - m_iWheelPrevAmount;
			m_iWheelPrevAmount = m_iWheelAmount;
			
			//m_fWheelDelta = 0.0f;
		}

		//------------------------------------------------------------------------
		// updates position data
		//------------------------------------------------------------------------
		void OnWM_MOUSEMOVE( LPARAM lParam_p ){

			m_pos.x = GET_X_LPARAM(lParam_p);
			m_pos.y = GET_Y_LPARAM(lParam_p);
		}
		static void ConvertMousePosToCentralizedClientSpace( float & x_p, float & y_p, UINT screenW_p, UINT screenH_p ){

			float halfW = (float)screenW_p / 2.0f; // min and max
			float halfH = (float)screenH_p / 2.0f;

			x_p = x_p  - halfW; //interpolation times full range + offset(min)
			y_p = -(y_p  - halfH);
		}
		static void ConvertMousePosFromCentralizedClientSpace( float & x_p, float & y_p, UINT screenW_p, UINT screenH_p ){

			float halfW = (float)screenW_p / 2.0f; // min and max
			float halfH = (float)screenH_p / 2.0f;

			x_p = x_p + halfW; //interpolation times full range + offset(min)
			y_p = (-y_p + halfH);
		}

		//------------------------------------------------------------------------
		// updates button states data
		//------------------------------------------------------------------------
		void OnWM_LBUTTONDOWN(){

			m_downButtons.left = true;
			m_pressedButtons.left = true;
		}
		void OnWM_MBUTTONDOWN(){

			m_downButtons.middle = true;
			m_pressedButtons.middle = true;
		}
		void OnWM_RBUTTONDOWN(){

			m_downButtons.right = true;
			m_pressedButtons.right = true;
		}
		void OnWM_LBUTTONUP(){

			m_downButtons.left = false;
			m_releasedButtons.left = true;
		}
		void OnWM_MBUTTONUP(){

			m_downButtons.middle = false;
			m_releasedButtons.middle = true;
		}
		void OnWM_RBUTTONUP(){

			m_downButtons.right = false;
			m_releasedButtons.right = true;
		}

		//------------------------------------------------------------------------
		// updates mouse wheel data
		//------------------------------------------------------------------------
		void OnWM_MOUSEWHEEL( WPARAM wParam_p ){

			m_iWheelAmount += GET_WHEEL_DELTA_WPARAM(wParam_p);
			//m_fWheelDelta = (float)GET_WHEEL_DELTA_WPARAM(wParam_p)/(float)WHEEL_DELTA;
		}

		//------------------------------------------------------------------------
		// message sent when window lose capture
		//------------------------------------------------------------------------
		void OnWM_CAPTURECHANGED(){

			m_bCaptured = false;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void ToogleCapture( HWND hWnd_p ){

			if( !m_bCaptured ){

				SetCapture( hWnd_p );
				m_bCaptured = true;
			}
			else{

				ReleaseCapture();

				// See On_WM_CAPTURECHANGED
			}
		}
		void Capture( HWND hWnd_p ){

			if( !m_bCaptured ){

				SetCapture( hWnd_p );
				m_bCaptured = true;
			}
		}
		void ReleaseCapture(){

			if( m_bCaptured ){

				::ReleaseCapture();

				// See On_WM_CAPTURECHANGED
			}
		}

		//------------------------------------------------------------------------
		// set cursor position given centralized client space coordinates
		//------------------------------------------------------------------------
		void SetPos_CentralizedClientSpace( float x_p, float y_p, UINT screenW_p, UINT screenH_p, HWND hWnd_p ){

			ConvertMousePosFromCentralizedClientSpace( x_p, y_p, screenW_p, screenH_p );

			POINT mousePoint;
			mousePoint.x = (long)x_p;
			mousePoint.y = (long)y_p;

			ClientToScreen( hWnd_p, &mousePoint );

			SetCursorPos( mousePoint.x, mousePoint.y );
		}


		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		Buttons GetDownButtons()const{return m_downButtons;}
		Buttons GetPressedButtons()const{return m_pressedButtons;}
		Buttons GetReleasedButtons()const{return m_releasedButtons;}
		Pos GetPos()const{return m_pos;}
		Pos GetDelta()const{return m_deltaPos;}
		float GetWheelNormalizedAmount()const{return (float)m_iWheelAmount/(float)WHEEL_DELTA;}
		float GetWheelNormalizedDelta()const{ return (float)m_iWheelDelta/(float)WHEEL_DELTA; }
		bool IsCaptured()const{return m_bCaptured;}

	private:

		Pos m_pos, m_prevPos, m_deltaPos;
		Buttons m_downButtons;
		Buttons m_pressedButtons;
		Buttons m_releasedButtons;
		int m_iWheelDelta;
		int m_iWheelPrevAmount, m_iWheelAmount;
		bool m_bCaptured;
	};
}
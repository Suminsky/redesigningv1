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

	private:
		Pos m_pos, m_prevPos, m_deltaPos;
		Buttons m_downButtons;
		Buttons m_pressedButtons;
		Buttons m_releasedButtons;
		float m_fScrollDelta;

	public:
		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		MouseInput(): m_fScrollDelta(0.0f){}

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
		}

		//------------------------------------------------------------------------
		// updates position data
		//------------------------------------------------------------------------
		void OnWM_MOUSEMOVE( LPARAM lParam_p ){

			m_pos.x = GET_X_LPARAM(lParam_p);
			m_pos.y = GET_Y_LPARAM(lParam_p);
		}
		static void ConvertMousePosToNormalizedScreenSpace( float & x_p, float & y_p, UINT screenW_p, UINT screenH_p ){

			float x = ((float)x_p / (float)screenW_p); // interpolation amount
			float y = ((float)y_p / (float)screenH_p);

			float halfW = (float)screenW_p / 2.0f; // min and max
			float halfH = (float)screenH_p / 2.0f;

			x_p = x * screenW_p - halfW; //interpolation times full range + offset(min)
			y_p = -(y * screenH_p - halfH);
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

			m_fScrollDelta = (float)GET_WHEEL_DELTA_WPARAM(wParam_p)/(float)WHEEL_DELTA;
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		Buttons GetDownButtons(){return m_downButtons;}
		Buttons GetPressedButtons(){return m_pressedButtons;}
		Buttons GetReleasedButtons(){return m_releasedButtons;}
		Pos GetPos(){return m_pos;}
		Pos GetDelta(){return m_deltaPos;}
		float GetWheelNormalizedAmount(){return m_fScrollDelta;}
	};
}
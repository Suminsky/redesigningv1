#pragma once

#include <Windows.h>
#include "../namespace gen/gen_data.h"

namespace win{

	//------------------------------------------------------------------------
	// Rect by pos and res (to avoid the misuse of RECT)
	// x, y, w and h always means x, y, w and h!
	//------------------------------------------------------------------------
	struct Rect{

		int x,y,w,h;

		Rect(){};
		Rect( int x_p, int y_p, int w_p, int h_p ):x(x_p), y(y_p), w(w_p), h(h_p){}

		// facilities

		Rect( const RECT & rect_p ){

			x = rect_p.left;
			y = rect_p.top;
			w = rect_p.right - x;
			h = rect_p.bottom - y;
		}

		inline Rect & FromRECT( const RECT & rect_p ){

			x = rect_p.left;
			y = rect_p.top;
			w = rect_p.right - x;
			h = rect_p.bottom - y;

			return *this;
		}

		inline void ToRECT( RECT & newRect_p ){

			newRect_p.left = x;
			newRect_p.top = y;
			newRect_p.right = w + x;			
			newRect_p.bottom = h + y;
		}
		inline RECT ToRECT(){

			RECT rect;
			ToRECT( rect );
			return rect;
		}

		inline void AnchoredResize( int w_p, int h_p, gen::Anchor anchor_p ){

			// cache

			int xdif = (w_p - w);
			int ydif = (h_p - h);

			// horz

			switch( anchor_p.h ){

			//case gen::EHORZALIGN_LEFT:

			case gen::EHORZALIGN_RIGHT:

				x = x - xdif;

				break;
			case gen::EHORZALIGN_CENTER:

				x = x - (int)(xdif * 0.5f);

				break;
			}

			// vert

			switch( anchor_p.h ){

			//case gen::EVERTALIGN_TOP:

			case gen::EVERTALIGN_BOTTOM:

				y = y - ydif;

				break;
			case gen::EVERTALIGN_CENTER:

				y = y - (int)(ydif * 0.5f);

				break;
			}

			w = w_p;
			h = h_p;
		}
	};
}
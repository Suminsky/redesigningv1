#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/02/26
	created:	26:2:2014   13:23
	file:		AABB2D.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include <DirectXMath.h>

namespace phys{


	//========================================================================
	// 
	//========================================================================
	class AABB2D{

	public:

		DirectX::XMFLOAT2 m_pos;
		float m_halfW, m_halfH;

		float Left()	const{ return m_pos.x - m_halfW; }
		float Right()	const{ return m_pos.x + m_halfW; }
		float Up()		const{ return m_pos.y + m_halfH; }
		float Down()	const{ return m_pos.y - m_halfH; }
		float W()		const{ return m_halfW*2.0f; }
		float H()		const{ return m_halfH*2.0f; }

		DirectX::XMFLOAT2 Down_Left() const { return DirectX::XMFLOAT2( Left(), Down() );}
		DirectX::XMFLOAT2 Up_Right()  const { return DirectX::XMFLOAT2( Up(), Right() );}

		// negative means penetration
		float XDistance( const AABB2D & other_p ){

			return abs( m_pos.x - other_p.m_pos.x ) - (m_halfW + other_p.m_halfW);
		}
		float YDistance( const AABB2D & other_p ){

			return abs( m_pos.y - other_p.m_pos.y ) - (m_halfH + other_p.m_halfH);
		}

		bool IsColliding( const AABB2D & other_p ){

			if( XDistance( other_p) > 0.0f ) return false;
			if( YDistance( other_p) > 0.0f ) return false;

			return true;
		}

		void RelativeDirection( const AABB2D & other_p, DirectX::XMFLOAT2 & dir_p ){

			dir_p.x = other_p.m_pos.x - m_pos.x;
			dir_p.y = other_p.m_pos.y - m_pos.y;
		}

		// if closest == p, means is inside
		void ClosestPointToPoint( const DirectX::XMFLOAT2 & p_p, DirectX::XMFLOAT2 & closest_p ){

			closest_p = p_p; // start as the point itself, if inside it will stay like that

			if( p_p.x < Left() ) closest_p.x = Left();
			if( p_p.x > Right() ) closest_p.x = Right();

			if( p_p.y < Down() ) closest_p.y = Down();
			if( p_p.y > Up() ) closest_p.y = Up();
		}

		bool IsPointInside( const DirectX::XMFLOAT2 & p_p ){

			if( abs( p_p.x - m_pos.x ) < m_halfW
				&&
				abs( p_p.y - m_pos.y ) < m_halfH )

				return true;

			return false;
		}

		bool FitInside( const AABB2D & other_p, float & wGap_p, float & hGap_p ){

			wGap_p = W() - other_p.W();
			hGap_p = H() - other_p.H();

			return wGap_p >= 0 && hGap_p >= 0;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static float SquareDistanceToPoint( const AABB2D & a_p, const DirectX::XMFLOAT2 & p_p ){

			float dist = 0.0f;
			
			float Xdist;
			if( p_p.x < a_p.Left() ){

				Xdist = a_p.Left() - p_p.x;
				dist += (Xdist) * (Xdist);
			}
			if( p_p.x > a_p.Right() ){

				Xdist = p_p.x - a_p.Right();
				dist += (Xdist) * (Xdist);
			}

			float Ydist;
			if( p_p.y < a_p.Down() ){

				Ydist = a_p.Down() - p_p.y;
				dist += (Ydist) * (Ydist);
			}
			if( p_p.y > a_p.Up() ){

				Ydist = p_p.y - a_p.Up();
				dist += (Ydist) * (Ydist);
			}

			return dist;
		}

	private:

	};
}
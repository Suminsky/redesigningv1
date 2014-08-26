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
	class AABB3D{

	public:

		DirectX::XMFLOAT3 m_pos;
		float m_halfW, m_halfH, m_halfD;

		float Left()	const{ return m_pos.x - m_halfW; }
		float Right()	const{ return m_pos.x + m_halfW; }
		float Up()		const{ return m_pos.y + m_halfH; }
		float Down()	const{ return m_pos.y - m_halfH; }
		float Front()	const{ return m_pos.z - m_halfD; }
		float Back()	const{ return m_pos.z + m_halfD; }
		float W()		const{ return m_halfW*2.0f; }
		float H()		const{ return m_halfH*2.0f; }
		float D()		const{ return m_halfD*2.0f; }

		DirectX::XMFLOAT3 Down_Left_Back() const { return DirectX::XMFLOAT3( Left(), Down(), Back() );}
		DirectX::XMFLOAT3 Up_Right_Front() const { return DirectX::XMFLOAT3( Up(), Right(), Front() );}

		// negative means penetration
		float XDistance( const AABB3D & other_p ){

			return abs( m_pos.x - other_p.m_pos.x ) - (m_halfW + other_p.m_halfW);
		}
		float YDistance( const AABB3D & other_p ){

			return abs( m_pos.y - other_p.m_pos.y ) - (m_halfH + other_p.m_halfH);
		}
		float ZDistance( const AABB3D & other_p ){

			return abs( m_pos.z - other_p.m_pos.z ) - (m_halfD + other_p.m_halfD);
		}

		bool CollisionData( const AABB3D & other_p, DirectX::XMFLOAT3 & normal, float & penetration ){

			const DirectX::XMFLOAT3 normals[] = {	DirectX::XMFLOAT3( -1.0f,  0.0f, 0.0f ),
													DirectX::XMFLOAT3(  1.0f,  0.0f, 0.0f ),
													DirectX::XMFLOAT3(  0.0f, -1.0f, 0.0f ),
													DirectX::XMFLOAT3(  0.0f,  1.0f, 0.0f ),
													DirectX::XMFLOAT3(  0.0f,  0.0f, -1.0f ),
													DirectX::XMFLOAT3(  0.0f,  0.0f,  1.0f )};

			// get penetration

			const float distances[] = {	other_p.Left()	- Right(),
										Left()			- other_p.Right(),
										other_p.Down()	- Up(),
										Down()			- other_p.Up(),
										other_p.Front() - Back(),
										Front()			- other_p.Back() };

			penetration = 0.0;
			normal.x = normal.y = normal.z = 0.0f;
			for( int it = 0; it < 6; ++it ){

				// ignore if theres no penetration
				if( distances[it] >= 0.0f ) return false;

				// keep the normal with least penetration
				if( penetration == 0 || distances[it] > penetration ){

					penetration = distances[it];
					normal = normals[it];
				}
			}

			return true;
		}

		//------------------------------------------------------------------------
		// blocked face is a bit mask, each bit is a face
		//------------------------------------------------------------------------
		bool CollisionData_Btests( const AABB3D & other_p, DirectX::XMFLOAT3 & normal, float & penetration, char blockedFace_p ){

			const DirectX::XMFLOAT3 normals[] = {	DirectX::XMFLOAT3( -1.0f,  0.0f, 0.0f ),
													DirectX::XMFLOAT3(  1.0f,  0.0f, 0.0f ),
													DirectX::XMFLOAT3(  0.0f, -1.0f, 0.0f ),
													DirectX::XMFLOAT3(  0.0f,  1.0f, 0.0f ),
													DirectX::XMFLOAT3(  0.0f,  0.0f, -1.0f ),
													DirectX::XMFLOAT3(  0.0f,  0.0f,  1.0f )};

			// get penetration

			const float distances[] = {	other_p.Left()	- Right(),
										Left()			- other_p.Right(),
										other_p.Down()	- Up(),
										Down()			- other_p.Up(),
										other_p.Front() - Back(),
										Front()			- other_p.Back() };

			penetration = 0.0;
			normal.x = normal.y = normal.z = 0.0f;
			for( int it = 0; it < 6; ++it ){

				// ignore if theres no penetration between any of the opposing faces
				if( distances[it] >= 0.0f ) return false;

				// skip if face is blocked
				if( (blockedFace_p != 0x0f) // but if all faces are blocked, consider all
					 &&
					(blockedFace_p & (0x01 << it)) ) continue;

				// keep the normal with least penetration (pen is negative)
				if( penetration == 0
					||
					distances[it] > penetration ){

					penetration = distances[it];
					normal = normals[it];
				}
			}

			return true;
		}

		bool IsColliding( const AABB3D & other_p ){

			if( XDistance( other_p) > 0.0f ) return false;
			if( YDistance( other_p) > 0.0f ) return false;
			if( ZDistance( other_p) > 0.0f ) return false;

			return true;
		}
		bool IsColliding_e( const AABB3D & other_p ){

			if( XDistance( other_p) >= 0.0f ) return false;
			if( YDistance( other_p) >= 0.0f ) return false;
			if( ZDistance( other_p) >= 0.0f ) return false;

			return true;
		}

		void RelativeDirection( const AABB3D & other_p, DirectX::XMFLOAT3 & dir_p ){

			dir_p.x = other_p.m_pos.x - m_pos.x;
			dir_p.y = other_p.m_pos.y - m_pos.y;
			dir_p.z = other_p.m_pos.z - m_pos.z;
		}

		// if closest == p, means is inside
		void ClosestPointToPoint( const DirectX::XMFLOAT3 & p_p, DirectX::XMFLOAT3 & closest_p ){

			closest_p = p_p; // start as the point itself, if inside it will stay like that

			if( p_p.x < Left() ) closest_p.x = Left();
			if( p_p.x > Right() ) closest_p.x = Right();

			if( p_p.y < Down() ) closest_p.y = Down();
			if( p_p.y > Up() ) closest_p.y = Up();

			if( p_p.z < Front() ) closest_p.z = Front();
			if( p_p.z > Back() ) closest_p.z = Back();
		}

		bool IsPointInside( const DirectX::XMFLOAT3 & p_p ){

			if( abs( p_p.x - m_pos.x ) < m_halfW
				&&
				abs( p_p.y - m_pos.y ) < m_halfH
				&&
				abs( p_p.z - m_pos.z ) < m_halfD )

				return true;

			return false;
		}

		bool FitInside( const AABB3D & other_p, float & wGap_p, float & hGap_p, float & dGap_p ){

			wGap_p = W() - other_p.W();
			hGap_p = H() - other_p.H();
			dGap_p = D() - other_p.D();

			return (wGap_p + hGap_p + dGap_p) >= 0;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static float SquareDistanceToPoint( const AABB3D & a_p, const DirectX::XMFLOAT3 & p_p ){

			/*DirectX::XMFLOAT3 closest;
			a_p.ClosestPointToPoint( p_p, closest );

			float XDist = p_p.x - closest.x;
			float YDist = p_p.y - closest.y;
			float ZDist = p_p.z - closest.z;
			return  XDist * XDist
					+
					YDist * YDist
					+
					ZDist * ZDist;*/
			
			float dist;

			float Xdist;
			if( p_p.x < a_p.Left() ){

				Xdist = a_p.Left() - p_p.x;
				dist += (Xdist) * (Xdist);
			}
			else if( p_p.x > a_p.Right() ){

				Xdist = p_p.x - a_p.Right();
				dist += (Xdist) * (Xdist);
			}

			float Ydist;
			if( p_p.y < a_p.Down() ){

				Ydist = a_p.Down() - p_p.y;
				dist += (Ydist) * (Ydist);
			}
			else if( p_p.y > a_p.Up() ){

				Ydist = p_p.y - a_p.Up();
				dist += (Ydist) * (Ydist);
			}

			float Zdist;
			if( p_p.z < a_p.Front() ){
				
				Zdist = a_p.Front() - p_p.z;
				dist += (Zdist) * (Zdist);

			}
			if( p_p.z > a_p.Back() ){
				
				Zdist = p_p.z - a_p.Back();
				dist += (Zdist) * (Zdist);
			}

			return dist;
		}

	private:

	};
}
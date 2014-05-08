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

		bool CollisionData_v2( const AABB2D & other_p, float & penetration ){

			// UNTESTED
			float xDist = XDistance( other_p );
			float yDist = YDistance( other_p );

			if( xDist >= 0.0f && yDist >= 0.0f ) return false;

			if( xDist < 0.0f && yDist < 0.0f ){

				if( xDist > yDist )			penetration = xDist;
				else if( yDist > xDist )	penetration = yDist;
			}
			else{


				if( xDist < 0.0f )	penetration = xDist;
				else				penetration = yDist;
			}

			return true;
		}

		bool CollisionData( const AABB2D & other_p, DirectX::XMFLOAT2 & normal, float & penetration ){

			const DirectX::XMFLOAT2 normals[] = {	DirectX::XMFLOAT2( -1.0f,  0.0f ),
													DirectX::XMFLOAT2(  1.0f,  0.0f ),
													DirectX::XMFLOAT2(  0.0f, -1.0f ),
													DirectX::XMFLOAT2(  0.0f,  1.0f ) };

			// get penetration

			const float distances[] = {	other_p.Left()	- Right(),
										Left()			- other_p.Right(),
										other_p.Down()	- Up(),
										Down()			- other_p.Up() };

			penetration = 0.0;
			normal.x = normal.y = 0.0f;
			for( int it = 0; it < 4; ++it ){

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
		bool CollisionData_Btests( const AABB2D & other_p, DirectX::XMFLOAT2 & normal, float & penetration, char blockedFace_p ){

			const DirectX::XMFLOAT2 normals[] = {	DirectX::XMFLOAT2( -1.0f,  0.0f ),
													DirectX::XMFLOAT2(  1.0f,  0.0f ),
													DirectX::XMFLOAT2(  0.0f, -1.0f ),
													DirectX::XMFLOAT2(  0.0f,  1.0f ) };

			// get penetration

			const float distances[] = {	other_p.Left()	- Right(),
										Left()			- other_p.Right(),
										other_p.Down()	- Up(),
										Down()			- other_p.Up() };

			penetration = 0.0;
			normal.x = normal.y = 0.0f;
			for( int it = 0; it < 4; ++it ){

				// ignore if theres no penetration
				if( distances[it] >= 0.0f ) return false;

				// skip if face is blocked
				if( (blockedFace_p != 0x0f) // but if all faces are blocked, consider all
					 &&
					(blockedFace_p & (0x01 << it)) ) continue;

				// keep the normal with least penetration
				if( penetration == 0 || distances[it] > penetration ){

					penetration = distances[it];
					normal = normals[it];
				}
			}

			return true;
		}

		//------------------------------------------------------------------------
		// otherr is stationary UNTESTED
		//------------------------------------------------------------------------
		bool CollisionDataMoving( DirectX::XMFLOAT2 & vel_p, const AABB2D & other_p,
									DirectX::XMFLOAT2 & normal, float & penetration,
									float & tFirst_p, float & tLast_p ){

			// exit early if boxes initially overlapping

			if( CollisionData(other_p, normal, penetration) ) {
				tFirst_p = tLast_p = 0.0f;
				
				return true;
			}

			// Initialize times of first and last contact
			tFirst_p = 0.0f;
			tLast_p = 1.0f;


			const DirectX::XMFLOAT2 normals[] = {	DirectX::XMFLOAT2(  1.0f,  0.0f ),
													DirectX::XMFLOAT2( -1.0f,  0.0f ),
													DirectX::XMFLOAT2(  0.0f,  1.0f ),
													DirectX::XMFLOAT2(  0.0f,  -1.0f ) };

			// get penetration

			float mins[] = { Left(), Down() };
			float otherMins[] = { other_p.Left(), other_p.Down() };
			float maxs[] = { Right(), Up() };
			float otherMaxs[] = {  other_p.Right(),  other_p.Up() };

			float vel[] = { vel_p.x, vel_p.y };

			auto fmax = [](float a, float b)->float{

				return a > b ? a : b;
			};
			auto fmin = [](float a, float b)->float{

				return a < b ? a : b;
			};

			// For each axis, determine times of first and last contact, if any
			for (int i = 0; i < 2; i++) {

				if (vel[i] < 0.0f) {

					if (maxs[i] < otherMins[i]) return false; // Non-intersecting and moving apart
					if (otherMaxs[i] < mins[i]){

						penetration = otherMaxs[i] - mins[i];
						normal = normals[i];

						tFirst_p = fmax((otherMaxs[i] - mins[i]) / vel[i], tFirst_p);
					}
					if (maxs[i] > otherMins[i]){

						penetration = otherMins[i] - maxs[i];
						normal = normals[i];

						tLast_p = fmin((otherMins[i] - maxs[i]) / vel[i], tLast_p);
					}
				}
				if (vel[i] > 0.0f) {

					if (mins[i] > otherMaxs[i]) return 0; // Non-intersecting and moving apart
					if (maxs[i] < otherMins[i]){

						penetration = otherMins[i] - maxs[i];
						normal = normals[i+1];
						
						tFirst_p = fmax((otherMins[i] - maxs[i]) / vel[i], tFirst_p);
					}
					if (otherMaxs[i] > mins[i]){

						penetration = mins[i] - otherMaxs[i];
						normal = normals[i+1];
						
						tLast_p =  fmin((otherMaxs[i] - mins[i]) / vel[i], tLast_p);
					}
				}
				// No overlap possible if time of first contact occurs after time of last contact
				if (tFirst_p > tLast_p) return false;
			}

				return true;
		}

		bool IsColliding( const AABB2D & other_p ){

			if( XDistance( other_p) > 0.0f ) return false;
			if( YDistance( other_p) > 0.0f ) return false;

			return true;
		}
		bool IsColliding_e( const AABB2D & other_p ){

			if( XDistance( other_p) >= 0.0f ) return false;
			if( YDistance( other_p) >= 0.0f ) return false;

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
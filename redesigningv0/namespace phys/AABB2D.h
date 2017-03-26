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
#include "../namespace gen/gen_data.h"

namespace phys{

	//========================================================================
	// 
	//========================================================================
	class AABB2D{

	public:
		
		enum E_BLOCKEDFACE{

			E_BLOCKEDFACE_LEFT = 0x01 << 0,
			E_BLOCKEDFACE_RIGHT = 0x01 << 1,
			E_BLOCKEDFACE_DOWN = 0x01 << 2,
			E_BLOCKEDFACE_UP = 0x01 << 3,
			E_BLOCKEDFACE_HORZ = E_BLOCKEDFACE_LEFT | E_BLOCKEDFACE_RIGHT,
			E_BLOCKEDFACE_VERT = E_BLOCKEDFACE_UP | E_BLOCKEDFACE_DOWN,
			E_BLOCKEDFACE_ALLBUT_UP = E_BLOCKEDFACE_HORZ | E_BLOCKEDFACE_DOWN,
			E_BLOCKEDFACE_ALLBUT_DOWN = E_BLOCKEDFACE_HORZ | E_BLOCKEDFACE_UP,
			E_BLOCKEDFACE_ALLBUT_LEFT = E_BLOCKEDFACE_VERT | E_BLOCKEDFACE_RIGHT,
			E_BLOCKEDFACE_ALLBUT_RIGHT = E_BLOCKEDFACE_VERT | E_BLOCKEDFACE_LEFT
		};

		static const DirectX::XMFLOAT2 s_left, s_right, s_up, s_down;

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
		float XDistance( const AABB2D & other_p ) const{

			return abs( m_pos.x - other_p.m_pos.x ) - (m_halfW + other_p.m_halfW);
		}
		float YDistance( const AABB2D & other_p ) const{

			return abs( m_pos.y - other_p.m_pos.y ) - (m_halfH + other_p.m_halfH);
		}

		bool CollisionData_v2( const AABB2D & other_p, float & penetration ) const{

			// UNTESTED
			float xDist = XDistance( other_p );
			float yDist = YDistance( other_p );

			if( xDist >= 0.0f || yDist >= 0.0f ) return false;

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

		bool CollisionData( const AABB2D & other_p, DirectX::XMFLOAT2 & normal, float & penetration, float fNoPenTolerance_p = 0.0f ) const{

			const DirectX::XMFLOAT2 normals[] = {
				DirectX::XMFLOAT2 (-1.0f, 0.0f ),
				DirectX::XMFLOAT2( 1.0f,  0.0f ),
				DirectX::XMFLOAT2(  0.0f, -1.0f ),
				DirectX::XMFLOAT2(  0.0f,  1.0f )};

			// get penetration

			const float distances[] = {	other_p.Left()	- Right(),
										Left()			- other_p.Right(),
										other_p.Down()	- Up(),
										Down()			- other_p.Up() };

			penetration = (std::numeric_limits<float>::lowest)();
			normal.x = normal.y = 0.0f;
			for( int it = 0; it < 4; ++it ){

				// ignore if theres no penetration
				if( distances[it] >= fNoPenTolerance_p ) return false;

				// keep the normal with least penetration
				if( distances[it] > penetration ){

					penetration = distances[it];
					normal = normals[it];
				}
			}

			return true;
		}

		//------------------------------------------------------------------------
		// blocked face is a bit mask, each bit is a face
		//------------------------------------------------------------------------
		bool CollisionData_Btests( const AABB2D & other_p, DirectX::XMFLOAT2 & normal, float & penetration, char blockedFace_p, float fNoPenTolerance_p = 0.0f ) const{

			const DirectX::XMFLOAT2 normals[] = {
				DirectX::XMFLOAT2 (-1.0f, 0.0f ),
				DirectX::XMFLOAT2( 1.0f,  0.0f ),
				DirectX::XMFLOAT2(  0.0f, -1.0f ),
				DirectX::XMFLOAT2(  0.0f,  1.0f )};
			// NOTE!!!!
			// INCREDIBLE performance drop when using static XMFLOAT2 for the normals

			// get penetration

			const float distances[] = {	other_p.Left()	- Right(),
										Left()			- other_p.Right(),
										other_p.Down()	- Up(),
										Down()			- other_p.Up() };

			penetration = (std::numeric_limits<float>::lowest)();
			normal.x = normal.y = 0.0f;
			for( int it = 0; it < 4; ++it ){

				// ignore if theres no penetration between any of the opposing faces (SAT)
				if( distances[it] >= fNoPenTolerance_p ) return false;

				// skip if face is blocked
				if( blockedFace_p & (0x01 << it) ) continue;

				// keep the normal with least penetration (pen is negative)
				if( distances[it] > penetration ){

					penetration = distances[it];
					normal = normals[it];
				}
			}

			return (normal.x != 0.0f || normal.y != 0.0f );//penetration == (std::numeric_limits<float>::lowest)())//

			//return true;
		}

		bool CollisionData_Touching( const AABB2D & other_p, DirectX::XMFLOAT2 & normal, float & penetration ) const{

			const DirectX::XMFLOAT2 normals[] = {
				DirectX::XMFLOAT2 (-1.0f, 0.0f ),
				DirectX::XMFLOAT2( 1.0f,  0.0f ),
				DirectX::XMFLOAT2(  0.0f, -1.0f ),
				DirectX::XMFLOAT2(  0.0f,  1.0f )};

			// get penetration

			const float distances[] = {
				other_p.Left()	- Right(),
				Left()			- other_p.Right(),
				other_p.Down()	- Up(),
				Down()			- other_p.Up() };

			penetration = (std::numeric_limits<float>::lowest)();
			normal.x = normal.y = 0.0f;
			for( int it = 0; it < 4; ++it ){

				// ignore if theres no penetration between any of the opposing faces (SAT)
				if( distances[it] > 0.0f ) return false;

				// keep the normal with least penetration (pen is negative)
				if( distances[it] > penetration ){

					penetration = distances[it];
					normal = normals[it];
				}
			}

			//return (normal.x != 0.0f || normal.y != 0.0f );//penetration == (std::numeric_limits<float>::lowest)())//

			return true;
		}

		//------------------------------------------------------------------------
		// otherr is stationary UNTESTED
		//------------------------------------------------------------------------
		bool CollisionDataMoving( DirectX::XMFLOAT2 & vel_p, const AABB2D & other_p,
									DirectX::XMFLOAT2 & normal, float & penetration,
									float & tFirst_p, float & tLast_p ) const{

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
													DirectX::XMFLOAT2(  0.0f, -1.0f ) };

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

		bool IsColliding( const AABB2D & other_p, float fNoPenTolerance_p = 0.0f )const{

			if( XDistance( other_p) > fNoPenTolerance_p) return false;
			if( YDistance( other_p) > fNoPenTolerance_p ) return false;

			return true;
		}
		bool IsColliding_e( const AABB2D & other_p, float fNoPenTolerance_p = 0.0f ) const{

			if( XDistance(other_p) >= fNoPenTolerance_p ) return false;
			if( YDistance(other_p) >= fNoPenTolerance_p ) return false;

			return true;
		}
		//------------------------------------------------------------------------
		// the XMFLOAT4 is x = left, y = right, z = down, w = up
		//------------------------------------------------------------------------
		bool IsColliding( const AABB2D & other_p, DirectX::XMFLOAT4 & outIntersectionRect_p, float fNoPenTolerance_p = 0.0f ){

			if( IsColliding( other_p, fNoPenTolerance_p ) ){

				// if the AABBs are intersecting

				// get the intersect rect

				outIntersectionRect_p = GetIntersectionRect(other_p);
				
				return true;
			}

			return false;
		}

		//------------------------------------------------------------------------
		// assumes colliding
		// the XMFLOAT4 is x = left, y = right, z = down, w = up
		//------------------------------------------------------------------------
		DirectX::XMFLOAT4 GetIntersectionRect( const AABB2D & other_p )const{

			return DirectX::XMFLOAT4(
				gen::Max( Left(), other_p.Left() ),
				gen::Min( Right(), other_p.Right() ),
				gen::Max( Down(), other_p.Down() ),
				gen::Min( Up(), other_p.Up() )
				);
		}

		void RelativeDirection( const AABB2D & other_p, DirectX::XMFLOAT2 & dir_p )const{

			dir_p.x = other_p.m_pos.x - m_pos.x;
			dir_p.y = other_p.m_pos.y - m_pos.y;
		}

		// if closest == p, means is inside
		void ClosestPointToPoint( const DirectX::XMFLOAT2 & p_p, DirectX::XMFLOAT2 & closest_p )const{

			closest_p = p_p; // start as the point itself, if inside it will stay like that

			if( p_p.x < Left() ) closest_p.x = Left();
			if( p_p.x > Right() ) closest_p.x = Right();

			if( p_p.y < Down() ) closest_p.y = Down();
			if( p_p.y > Up() ) closest_p.y = Up();
		}

		bool IsPointInside( const DirectX::XMFLOAT2 & p_p )const{

			if( abs( p_p.x - m_pos.x ) < m_halfW
				&&
				abs( p_p.y - m_pos.y ) < m_halfH )

				return true;

			return false;
		}

		bool FitInside( const AABB2D & other_p, float & wGap_p, float & hGap_p )const{

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

			return dist;
		}

		static void GetOppositeToDirFacesMask( const DirectX::XMFLOAT2 & dir, char & cMask ){

			// logic:
			// if going right, can only collide with left faces of other block, not right faces, etc.

			if (dir.x == 0.0f)
				cMask |= E_BLOCKEDFACE_HORZ;
			else if( dir.x < 0.0f )
				cMask |= E_BLOCKEDFACE_LEFT;// 0x01 << 0;
			else//if( dir.x >= 0.0f )
				cMask |= E_BLOCKEDFACE_RIGHT;//0x01 << 1;

			if (dir.y == 0.0f)
				cMask |= E_BLOCKEDFACE_VERT;
			else if( dir.y < 0.0f )
				cMask |= E_BLOCKEDFACE_DOWN;//0x01 << 2;
			else//if( dir.y >= 0.0f )
				cMask |= E_BLOCKEDFACE_UP;//0x01 << 3;
		}

		static void GetIgnoredDueBLockedFacesMask(
			const DirectX::XMFLOAT2 & relDir, 
			const DirectX::XMFLOAT2 & Adir,	const DirectX::XMFLOAT2 & Bdir,
			char aMask_p, char bMask_p,
			char & cMask ){

				const bool goingDir[] = {
					relDir.x < 0.0f, // left, will collide in right
					relDir.x > 0.0f, // rit, will collide in left
					relDir.y < 0.0f, // down, will collide in up
					relDir.y > 0.0f // up, will collide in down
				};

				// this array is meant to be "whos going to hit who", but I think its wrong

				const char otherMask[] = {
					Adir.x < Bdir.x ? bMask_p : aMask_p,
					Adir.x > Bdir.x ? bMask_p : aMask_p,
					Adir.y < Bdir.y ? bMask_p : aMask_p,
					Adir.y > Bdir.y ? bMask_p : aMask_p,
				};

				const char counterOtherMask[] = {
					Adir.x < Bdir.x ? aMask_p : bMask_p,
					Adir.x > Bdir.x ? aMask_p : bMask_p,
					Adir.y < Bdir.y ? aMask_p : bMask_p,
					Adir.y > Bdir.y ? aMask_p : bMask_p,
				};

				// TODO, ta horrivel, mas funcia, dar um jeito de iniciar esses array melhor

				const char otherFaceMask[] = {
					E_BLOCKEDFACE_RIGHT, // rit
					E_BLOCKEDFACE_LEFT, // left
					E_BLOCKEDFACE_UP, // up
					E_BLOCKEDFACE_DOWN // down
				};

				const char counterOtherFaceMask[] = {
					E_BLOCKEDFACE_LEFT, // rit
					E_BLOCKEDFACE_RIGHT, // left
					E_BLOCKEDFACE_DOWN, // up
					E_BLOCKEDFACE_UP // down
				};

				for( int it = 0; it < 4; ++it ){

					if( goingDir[it] ){

						if( otherMask[it] & otherFaceMask[it]
							||
							counterOtherMask[it] & counterOtherFaceMask[it] )

							cMask |= otherFaceMask[it];//(0x01 << it);//
					}
				}
		}

		static void GetPrevSeparationMask(
			AABB2D a_p, const DirectX::XMFLOAT2 & Adir,
			AABB2D b_p, const DirectX::XMFLOAT2 & Bdir,
			char & cMask,
			float fNoPenTolerance_p = 0.0f ){

				// Logic: if it was "penetrating" on the previous pos at an axis (no separation at an axis),
				// cant be penetrating at that axis on current pos, even if penetration happens to be smaller on the
				// axis.
				// This is due:
				// #1) it wasnt colliding before, the code needs to assure 100% penetration free after leaving the
				// collisions test.
				// #2) if it wasnt colliding at the issuing axis (and so wasnt colliding at all(SAT fail)),
				// and now it is colliding, means the collision happens at an ortho axis.
				// Consider visualizing 2 AABB colliding for better understanding, in the prev step of a collision,
				// the axis at witch the collision happen will never be one that was previously already intercepting.
				// A --- B, A and B separated by axis X, theyre already intercepting in Y
				// --- AB, A hits B on X, its impossible to be colliding on Y, even if the penetration is smaller on Y for
				// whatever reason (its possible and not that rare, consider platformer death leaps (hard jumps), where character
				// reachs the other platformer by a few pixels)

				// put on prev pos:

				a_p.m_pos.x -= Adir.x;
				a_p.m_pos.y -= Adir.y;
				b_p.m_pos.x -= Bdir.x;
				b_p.m_pos.y -= Bdir.y;

				// check separating axis, mark axis with no separation:

				// get penetration

				const float distances[] = {
					b_p.Left() - a_p.Right(),
					a_p.Left() - b_p.Right(),
					b_p.Down() - a_p.Up(),
					a_p.Down() - b_p.Up() };

				for( int it = 0; it < 4; ++it ){

					// if theres penetration, mark as invalid

					if( distances[it] < fNoPenTolerance_p )
						cMask |= 0x01 << it;
				}
		}

	private:

	};
}
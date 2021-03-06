#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/02/26
	created:	26:2:2014   13:23
	file:		AABB2D.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	� Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
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
		float XDistance( const AABB3D & other_p ) const{

			return abs( m_pos.x - other_p.m_pos.x ) - (m_halfW + other_p.m_halfW);
		}
		float YDistance( const AABB3D & other_p ) const{

			return abs( m_pos.y - other_p.m_pos.y ) - (m_halfH + other_p.m_halfH);
		}
		float ZDistance( const AABB3D & other_p ) const{

			return abs( m_pos.z - other_p.m_pos.z ) - (m_halfD + other_p.m_halfD);
		}

		bool CollisionData( const AABB3D & other_p, DirectX::XMFLOAT3 & normal, float & penetration ) const{

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

			penetration = (std::numeric_limits<float>::lowest)();
			normal.x = normal.y = normal.z = 0.0f;
			for( int it = 0; it < 6; ++it ){

				// ignore if theres no penetration
				if( distances[it] >= 0.0f ) return false;

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
		bool CollisionData_Btests( const AABB3D & other_p, DirectX::XMFLOAT3 & normal, float & penetration, char blockedFace_p, float fNoPenTolerance_p = 0.0f ) const{

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

			penetration = (std::numeric_limits<float>::lowest)();
			normal.x = normal.y = normal.z = 0.0f;
			for( int it = 0; it < 6; ++it ){

				// ignore if theres no penetration between any of the opposing faces
				if( distances[it] >= fNoPenTolerance_p/*-0.001f*//*0.0f*/ ) return false;

				// skip if face is blocked
				if( blockedFace_p & (0x01 << it) ) continue;

				// keep the normal with least penetration (pen is negative)
				if( distances[it] > penetration ){

					penetration = distances[it];
					normal = normals[it];
				}
			}

			return true;
		}

		bool IsColliding( const AABB3D & other_p ) const{

			if( XDistance(other_p) > 0.0f ) return false;
			if( YDistance(other_p) > 0.0f ) return false;
			if( ZDistance(other_p) > 0.0f ) return false;

			return true;
		}
		bool IsColliding_e( const AABB3D & other_p, float fNoPenTolerance_p = 0.0f ) const{

			if( XDistance(other_p) >= fNoPenTolerance_p ) return false;
			if( YDistance(other_p) >= fNoPenTolerance_p ) return false;
			if( ZDistance(other_p) >= fNoPenTolerance_p ) return false;

			return true;
		}

		void RelativeDirection( const AABB3D & other_p, DirectX::XMFLOAT3 & dir_p ) const{

			dir_p.x = other_p.m_pos.x - m_pos.x;
			dir_p.y = other_p.m_pos.y - m_pos.y;
			dir_p.z = other_p.m_pos.z - m_pos.z;
		}

		// if closest == p, means is inside
		void ClosestPointToPoint( const DirectX::XMFLOAT3 & p_p, DirectX::XMFLOAT3 & closest_p ) const{

			closest_p = p_p; // start as the point itself, if inside it will stay like that

			if( p_p.x < Left() )		closest_p.x = Left();
			else if( p_p.x > Right() )	closest_p.x = Right();

			if( p_p.y < Down() )		closest_p.y = Down();
			else if( p_p.y > Up() )		closest_p.y = Up();

			if( p_p.z < Front() )		closest_p.z = Front();
			else if( p_p.z > Back() )	closest_p.z = Back();
		}

		bool IsPointInside( const DirectX::XMFLOAT3 & p_p ) const{

			if( abs( p_p.x - m_pos.x ) > m_halfW
				||
				abs( p_p.y - m_pos.y ) > m_halfH
				||
				abs( p_p.z - m_pos.z ) > m_halfD )

				return false;

			return true;
		}

		bool FitInside( const AABB3D & other_p, float & wGap_p, float & hGap_p, float & dGap_p ) const{

			wGap_p = W() - other_p.W();
			hGap_p = H() - other_p.H();
			dGap_p = D() - other_p.D();

			if( wGap_p < 0.0f 
				||
				hGap_p < 0.0f
				||
				dGap_p < 0.0f ) return false;

			return true;
		}
		bool IsInsideOther( const AABB3D & other_p ) const{

			float xPen = XDistance(other_p);
			float yPen = YDistance(other_p);
			float zPen = ZDistance(other_p);

			// 30 + (-30) 
			if( W() + xPen > 0.0f
				||
				H() + yPen > 0.0f
				||
				D() + zPen > 0.0f ) return false;

			return true;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static float SquareDistanceToPoint( const AABB3D & a_p, const DirectX::XMFLOAT3 & p_p ){
			
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

		static void GetOppositeToDirFacesMask( const DirectX::XMFLOAT3 & dir, char & cMask ){

				// logic:
				// if going right, can only collide with left faces of other block, not right faces, etc.

				if( dir.x <= 0.0f )
					cMask |= 0x01 << 0;
				if( dir.x >= 0.0f )
					cMask |= 0x01 << 1;

				if( dir.y <= 0.0f )
					cMask |= 0x01 << 2;
				if( dir.y >= 0.0f )
					cMask |= 0x01 << 3;

				if( dir.z <= 0.0f )
					cMask |= 0x01 << 4;
				if( dir.z >= 0.0f )
					cMask |= 0x01 << 5;
		}

	private:

	};
}
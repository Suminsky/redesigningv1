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

	//NOTE: untested and made in a rush!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//NOTE: untested and made in a rush!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//NOTE: untested and made in a rush!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// //NOTE: untested and made in a rush!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 
//NOTE: untested and made in a rush!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// //NOTE: untested and made in a rush!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//========================================================================
	// 
	//========================================================================
	class Circle{

	public:

		DirectX::XMFLOAT2 m_pos;
		float m_radius;

		// negative means penetration
		float Distance( const Circle & other_p ){

			DirectX::XMFLOAT2 relDir;
			RelativeDirection( other_p, relDir );
			DirectX::XMVECTOR vDir = DirectX::XMLoadFloat2( &relDir );

			return DirectX::XMVectorGetX(DirectX::XMVector2Length( vDir ));
		}

		bool IsColliding( const Circle & other_p ){

			if( Distance( other_p) > 0.0f ) return false;

			return true;
		}

		void RelativeDirection( const Circle & other_p, DirectX::XMFLOAT2 & dir_p ){

			dir_p.x = other_p.m_pos.x - m_pos.x;
			dir_p.y = other_p.m_pos.y - m_pos.y;
		}

		// if closest == p, means is inside
		void ClosestPointToPoint( const DirectX::XMFLOAT2 & p_p, DirectX::XMFLOAT2 & closest_p ){
			
			DirectX::XMFLOAT2 relDir( p_p.x - m_pos.x, p_p.y - m_pos.y );
			DirectX::XMVECTOR vDir = DirectX::XMLoadFloat2( &relDir );
			vDir = DirectX::XMVector2Normalize(vDir);
			vDir = DirectX::XMVectorScale( vDir, m_radius );

			DirectX::XMLoadFloat2( &closest_p );
		}

		bool IsPointInside( const DirectX::XMFLOAT2 & p_p ){

			DirectX::XMFLOAT2 relDir( p_p.x - m_pos.x, p_p.y - m_pos.y );
			DirectX::XMVECTOR vDir = DirectX::XMLoadFloat2( &relDir );
			float dist = DirectX::XMVectorGetX(DirectX::XMVector2Length(vDir));

			return dist < m_radius;
		}

		bool FitInside( const Circle & other_p, float & Gap_p ){

			Gap_p = m_radius - other_p.m_radius;

			return Gap_p >= 0;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static float SquareDistanceToPoint( const Circle & a_p, const DirectX::XMFLOAT2 & p_p ){

			DirectX::XMFLOAT2 rel( a_p.m_pos.x - p_p.x, a_p.m_pos.y - p_p.y );

			return rel.x * rel.x + rel.y * rel.y;
		}

	private:

	};
}
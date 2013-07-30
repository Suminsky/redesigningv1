#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/23
	created:	23:7:2013   20:28
	file:		Tweening.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	tweening functions

	� Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes

namespace gen{

	template< typename Time = float, typename Value = float >
	struct Tweening{

		enum E_EASING{
			E_IN,
			E_OUT,
			E_INOUT,
			E_OUTIN
		};

		//------------------------------------------------------------------------
		// linear interpolation
		//------------------------------------------------------------------------
		static Value Linear( Time elapsed_p, Time duration_p, Value A_p = (Value)0.0f, Value B_p = (Value)1.0f ){

			if( elapsed_p > duration_p ) return B_p;

			// change in position, relative difference between start and end, directional distance
			// times
			// percentage, amount to be traversed
			// plus offset, plus from where it started

			return (B_p-A_p) * (elapsed_p/duration_p) + A_p; 
		}

		//------------------------------------------------------------------------
		// Quadratic interpolation
		// t^2
		//------------------------------------------------------------------------
		static Value InQuadratic( Time elapsed_p, Time duration_p, Value A_p = (Value)0.0f, Value B_p = (Value)1.0f ){

			if( elapsed_p > duration_p ) return B_p;

			elapsed_p/= duration_p;

			return (B_p-A_p) * (elapsed_p*elapsed_p) + A_p; 
		}
		static Value OutQuadratic( Time elapsed_p, Time duration_p, Value A_p = (Value)0.0f, Value B_p = (Value)1.0f ){

			if( elapsed_p > duration_p ) return B_p;

			// t * (t-2)

			elapsed_p/= duration_p;

			return -(B_p-A_p) * (elapsed_p * (elapsed_p -(Time)2.0f) ) + A_p; 
		}
		static Value InOutQuadratic( Time elapsed_p, Time duration_p, Value A_p = (Value)0.0f, Value B_p = (Value)1.0f ){

			if( elapsed_p > duration_p ) return B_p;

			elapsed_p /= duration_p;

			if( elapsed_p < (Time)0.5f){

				return ( ( (B_p-A_p) * (elapsed_p*elapsed_p) ) / (Time)2.0f ) + A_p;

			}
			else{

				return ((-(B_p-A_p) * (elapsed_p * (elapsed_p -(Time)2.0f) )) / (Time)2.0f) + A_p;

				// TODO: check if works, I changed
			}
		}
	};
}
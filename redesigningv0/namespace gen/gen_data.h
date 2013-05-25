//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/06
	created:	6:2:2013   0:15
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace gen\gen_data.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace gen
	file base:	gen_data
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

namespace gen{

	//========================================================================
	// used initially for boost shared_ptr deleter, when using shared pointer 
	// with stack allocated data
	//========================================================================
	template <class T>
	static void NoOp(T* obj)
	{
		obj = obj;
		(void)0;
	}

	//========================================================================
	// used nitially for std::shared_ptr which for some really stupid reason
	// don't have a T[] version like unique_ptr
	//========================================================================
	template <class T>
	static void ArrayDeleter(T* obj){

		delete [] obj;
	}

	//========================================================================
	// 
	//========================================================================
	enum EHORZALIGN{

		EHORZALIGN_LEFT,
		EHORZALIGN_RIGHT,
		EHORZALIGN_CENTER
	};

	enum EVERTALIGN{

		EVERTALIGN_TOP,
		EVERTALIGN_BOTTOM,
		EVERTALIGN_CENTER
	};


	//========================================================================
	// Nine point anchor structure
	// \ ^ /
	// - o -
	// / v \
	//========================================================================
	struct Anchor{

		EHORZALIGN h;
		EVERTALIGN v;

		Anchor( EHORZALIGN h_p, EVERTALIGN v_p ): h(h_p), v(v_p){}

		// facility

		inline Anchor& Centralize(){

			h = EHORZALIGN_CENTER;
			v = EVERTALIGN_CENTER;

			return *this;
		}
	};

	//========================================================================
	// TWEENING functions
	//========================================================================
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

			elapsed_p/= duration_p;

			return (B_p-A_p) * (elapsed_p*elapsed_p) + A_p; 
		}
		static Value OutQuadratic( Time elapsed_p, Time duration_p, Value A_p = (Value)0.0f, Value B_p = (Value)1.0f ){

			// t * (t-2)

			elapsed_p/= duration_p;

			return -(B_p-A_p) * (elapsed_p * (elapsed_p -(Time)2.0f) ) + A_p; 
		}
		static Value InOutQuadratic( Time elapsed_p, Time duration_p, Value A_p = (Value)0.0f, Value B_p = (Value)1.0f ){

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
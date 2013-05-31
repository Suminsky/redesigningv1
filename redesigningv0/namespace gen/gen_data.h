//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/06
	created:	6:2:2013   0:15
	file base:	gen_data
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

//========================================================================
// member func pointer syntax example to remember..
//========================================================================
//private:
//
//	Color3 (RayTracer::*m_rayTracedColorFunction)(int, int);					// "delegate" that computes the traced color
//	Color3 (RayTracer::*m_illuminationMethodFunction)(int, const HitData &);	// "delegate" that computes color at hit point
//
//	m_rayTracedColorFunction = &RayTracer::TraceCloserRayHit;					// assignment
//	m_illuminationMethodFunction = &RayTracer::PhongAtIntersectionPoint;
//
// Color3 c = (this->*m_rayTracedColorFunction)(x, y);							// use
// returnColor = ((this->*m_illuminationMethodFunction)(itCandidateShape, candidatehit));
// 
// typedefing function/member function pointer:
// 
// int (*FuncPtr)(int); // simple func pointer declaration, FuncPtr is a variable
// typedef int (*FuncPtr)(int); // FuncPtr now a type
// typedef int (MyClass::*MyTypedef)( int); // MyTypedef is a type

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


	//========================================================================
	// picking n random elements from n elements with no repeat
	//========================================================================
	template < int nToPick, int nElements >
	void PickRandomElements( int * pElementsPicked_p, int * pElementsAvailable_p ){

		for( int it = 0, nElementsRemaining = nElements; it < nToPick; ++it ){

			int randIndex = rand() % nElementsRemaining;

			pElementsPicked_p[it] = pElementsAvailable_p[randIndex];

			// discard card picked by swapping it to last element

			int tmp = pElementsAvailable_p[--nElementsRemaining]; // last element wont be on the range anymore
			pElementsAvailable_p[nElementsRemaining] = pElementsAvailable_p[randIndex];
			pElementsAvailable_p[randIndex] = tmp;		
		}

	}
}
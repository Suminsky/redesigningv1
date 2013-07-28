//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/06
	created:	6:2:2013   0:15
	file base:	gen_data
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	
				NOTE:	external linkage function must be templated or inlined in order to
						appear in multiple modules without link error (already defined)

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
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
	// string stuff
	//========================================================================
#define GEN_MAXSTRINGSIZE 2048
	inline unsigned int CountString( const char* szString_p )
	{
		int counter = 0;

		while( szString_p[counter] != 0x00 ){
			++counter;
			if( counter > GEN_MAXSTRINGSIZE )
				break;
		}

		return counter;
	}
	inline unsigned int CountWString( const wchar_t * szString_p )
	{
		int counter = 0;

		while( szString_p[counter] != 0x0000 ){

			++counter;
			if( counter > GEN_MAXSTRINGSIZE )
				break;
		}

		return counter;
	}
	inline bool CompareStringBLB( const char* string1_p, const char* string2_p)
	{
		int it = 0;

		while( string1_p[it] == string2_p[it] )
		{
			if( string1_p[it] == 0x00 )
			{
				return true;
			}
			else{
				++it;
				if( it > GEN_MAXSTRINGSIZE )
					break;
			}
		}

		return false;
	}


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

#include "Tweening.h"
#include "Delegate.h"
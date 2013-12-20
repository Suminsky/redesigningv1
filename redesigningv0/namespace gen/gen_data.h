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

#include <assert.h>
#include <string> // memcpy

namespace gen{

	//========================================================================
	// used initially for boost shared_ptr deleter, when using shared pointer 
	// with stack allocated data
	//========================================================================
	template <class T>
	static void NoOp(T* obj)
	{
		obj = obj;
		//(void)0;
	}


	//========================================================================
	// min max clamping
	//========================================================================
	inline void MinClamp( float & f_p, float min_p = 0.0f ){

		if( f_p < min_p ) f_p = min_p;
	}
	inline void MaxClamp( float & f_p, float max_p = 1.0f ){

		if( f_p > max_p ) f_p = max_p;
	}

	//========================================================================
	// used nitially for std::shared_ptr which for some really stupid reason
	// don't have a T[] version like unique_ptr
	//========================================================================
	template <class T>
	static void ArrayDeleter(T* obj){

		delete [] obj;
	}

	//------------------------------------------------------------------------
	// array 2d <-> 1d conversions
	//------------------------------------------------------------------------
	inline int XFromIndex( int index_p, int iW_p ){

		return index_p % iW_p; // x/col repeats for every row
		// return index - ((index_p / iW_p) * iW_p);
	}
	inline int YFromIndex( int index_p, int iW_p ){

		return index_p / iW_p;
	}
	inline int indexFromXY( int x_p, int y_p, int iW_p ){

		return y_p * iW_p + x_p;
	}

	//========================================================================
	// 
	//========================================================================
	enum E_HORZALIGN{

		E_HORZALIGN_LEFT,
		E_HORZALIGN_RIGHT,
		E_HORZALIGN_CENTER
	};

	enum E_VERTALIGN{

		E_VERTALIGN_TOP,
		E_VERTALIGN_BOTTOM,
		E_VERTALIGN_CENTER
	};


	//========================================================================
	// Nine point anchor structure
	// \ ^ /
	// - o -
	// / v \
	//========================================================================
	struct Anchor{

		E_HORZALIGN h;
		E_VERTALIGN v;

		Anchor( E_HORZALIGN h_p, E_VERTALIGN v_p ): h(h_p), v(v_p){}

		// facility

		inline Anchor& Centralize(){

			h = E_HORZALIGN_CENTER;
			v = E_VERTALIGN_CENTER;

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
	inline bool CompareString( const char* string1_p, const char* string2_p)
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
	template < typename T >
	inline void PickRandomElementsNoRepeat( int nToPick, int nElements, T * pElementsPicked_p, T * pElementsAvailable_p ){

		for( int it = 0, nElementsRemaining = nElements; it < nToPick; ++it ){

			int randIndex = rand() % nElementsRemaining;

			pElementsPicked_p[it] = pElementsAvailable_p[randIndex];

			// discard picked by swapping it to last element

			pElementsAvailable_p[randIndex] = pElementsAvailable_p[--nElementsRemaining];
		}
	}
	template < typename T >
	inline void PickRandomElementsRepeat( int nToPick, int nElements, T * pElementsPicked_p, T * pElementsAvailable_p ){

		for( int it = 0, nElementsRemaining = nElements; it < nToPick; ++it ){

			int randIndex = rand() % nElementsRemaining;

			pElementsPicked_p[it] = pElementsAvailable_p[randIndex];
		}
	}

	//========================================================================
	// that thing really works
	//========================================================================
	template<int SIZE>
	struct MorpherUnion{

		unsigned char m_data[SIZE];
		static const int s_SIZE = SIZE;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		TYPE GetChunkAs( unsigned int iByteOffset_p ){

			assert( iByteOffset_p + sizeof(TYPE) <= SIZE );

			return   *((TYPE*) (&(m_data[iByteOffset_p])) );
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		void SetChunkAs( unsigned int iByteOffset_p, TYPE newData_p ){

			assert( iByteOffset_p + sizeof(TYPE) <= SIZE );

			*((TYPE*) (&(m_data[iByteOffset_p])) ) = newData_p;
		}

		/*
		testingBlob testingB;
			testingB.a = 'a';
			testingB.banana = 435;
			testingB.floating = 23.2f;

		MorpherUnion<sizeof(int)*2 + sizeof(testingBlob)> blobTest;

		blobTest.SetChunkAs<int>( 0, 35 );
		blobTest.SetChunkAs<int>( sizeof(int), 402 );
		blobTest.SetChunkAs<testingBlob>( sizeof(int)*2, testingB);

		int test = blobTest.GetChunkAs<int>(0);
		test = blobTest.GetChunkAs<int>(sizeof(int));

		testingBlob testStruct = blobTest.GetChunkAs<testingBlob>(sizeof(int)*2);

		test = testStruct.banana;
		*/
	};

	struct DataBuffer{

		unsigned int m_size;
		unsigned char *m_data;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		TYPE GetChunkAs( unsigned int iByteOffset_p ){

			assert( iByteOffset_p + sizeof(TYPE) <= m_size );

			return   *((TYPE*) (&(m_data[iByteOffset_p])) );
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		void SetChunkAs( unsigned int iByteOffset_p, TYPE newData_p ){

			assert( iByteOffset_p + sizeof(TYPE) <= m_size );

			*((TYPE*) (&(m_data[iByteOffset_p])) ) = newData_p;
		}
	};

	struct DataStream{

		unsigned int m_size;
		unsigned char *m_data;
		unsigned int m_currentByteIndex;

		void Set( unsigned char * pData, unsigned int iSize ){

			assert( iSize <= m_size );

			memcpy( m_data, pData, iSize );

			m_currentByteIndex = iSize;
		}

		void Queue( unsigned char * pData, unsigned int iSize ){

			assert( m_currentByteIndex + iSize <= m_size );

			memcpy( &m_data[m_currentByteIndex], pData, iSize );

			m_currentByteIndex += iSize;
		}
	};

	struct DataStreamW{

		unsigned int m_size;
		wchar_t *m_data;
		unsigned int m_currentByteIndex;

		void Set( wchar_t * pData, unsigned int iSize ){

			assert( iSize <= m_size );

			memcpy( m_data, pData, iSize );

			m_currentByteIndex = iSize;
		}

		void Queue( wchar_t * pData, unsigned int iSize ){

			assert( m_currentByteIndex + iSize <= m_size );

			memcpy( &m_data[m_currentByteIndex], pData, iSize );

			m_currentByteIndex += iSize;
		}
	};
}

#include "Tweening.h"
#include "Delegate.h"
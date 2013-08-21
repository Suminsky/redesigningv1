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
	template < int nToPick, int nElements >
	void PickRandomElements( int * pElementsPicked_p, int * pElementsAvailable_p ){

		for( int it = 0, nElementsRemaining = nElements; it < nToPick; ++it ){

			int randIndex = rand() % nElementsRemaining;

			pElementsPicked_p[it] = pElementsAvailable_p[randIndex];

			// discard picked by swapping it to last element

			pElementsAvailable_p[randIndex] = pElementsAvailable_p[nElementsRemaining--];
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
}

#include "Tweening.h"
#include "Delegate.h"
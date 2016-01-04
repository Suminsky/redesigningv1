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
#include <stdint.h>
#include <math.h>
#include "gen_macros.h"

namespace gen{

	//========================================================================
	// disallow copy of derived objects
	//========================================================================
	class NonCopyable{
	protected:
		NonCopyable(){}
		~NonCopyable(){}
	private: 
		NonCopyable( const NonCopyable& );
		const NonCopyable& operator=( const NonCopyable& );
	};

	//========================================================================
	// used initially for boost shared_ptr deleter, when using shared pointer 
	// with stack allocated data
	//========================================================================
	template <class T>
	inline void NoOp(T* obj)
	{
		obj = obj;
		//(void)0;
	}

	//========================================================================
	// used nitially for std::shared_ptr which for some really stupid reason
	// don't have a T[] version like unique_ptr
	//========================================================================
	template <class T>
	inline void ArrayDeleter(T* obj){

		delete [] obj;
	}

	//========================================================================
	// rounds to the next number thats a multiple of multiple_p
	//========================================================================
	inline uint32_t RoundUpToNextMultiple( uint32_t n_p, uint32_t multiple_p ){

		return n_p + multiple_p - 1 - (n_p - 1) % multiple_p;
	}
	inline uint32_t RoundUpToNextMultiple_POT( uint32_t n_p, uint32_t multiplePOT_p ){

		return (n_p + multiplePOT_p - 1) & ~(multiplePOT_p - 1);
	}
	inline int32_t RoundUpToNextMultiple( int32_t n_p, int32_t multiple_p ){

		if( n_p < 0 ){
			// multiple_p = -multiple_p; rounds in the left direction (rounds down)

			return - ((int)(RoundUpToNextMultiple( (uint32_t)-n_p, (uint32_t)multiple_p )) - multiple_p );
		}

		return RoundUpToNextMultiple( (uint32_t)n_p, (uint32_t)multiple_p );
	}
	inline int32_t RoundUpToNextMultiple_POT( int32_t n_p, int32_t multiplePOT_p ){

		if( n_p < 0 ){
			// multiple_p = -multiple_p; rounds in the left direction (rounds down)

			return - ((int)(RoundUpToNextMultiple_POT( (uint32_t)-n_p, (uint32_t)multiplePOT_p )) - multiplePOT_p );
		}

		return RoundUpToNextMultiple_POT( (uint32_t)n_p, (uint32_t)multiplePOT_p );
	}

	namespace Math2DUtil{

		#define gen_PI       3.14159265358979323846

		inline void VectorFromAngleD( float & x, float & y, float degree_p ){
			//orientationVector( cosAngle, sinAngle )
			float rad = degree_p * (float)gen_PI/180.0f ;

			x = cos( rad );
			y = sin( rad );
		}
		inline void VectorFromAngleR( float & x, float & y, float rad_p ){

			x = cos( rad_p );
			y = sin( rad_p );
		}

		inline float AngleFromDirectionVectorR( float x, float y ){

			return atan2( y, x );
		}
		inline float AngleFromDirectionVectorD( float x, float y ){

			return atan2( y, x ) * 180.0f/(float)gen_PI;
		}
	}

	//========================================================================
	// 
	//========================================================================
	inline float ConvertValueFromRangeToNewRange( float value_p, float start_p, float end_p, float newStart_p, float newEnd_p ){

		// eliminate the offset, get the % (piece) on the total range, multiply by the total new range, and add
		// the new offset

		return (value_p - start_p) / (end_p - start_p) * (newEnd_p - newStart_p) + newStart_p;
	}

	template<typename T>
	inline T Min( T a, T b ){

		return a < b ? a : b;
	}
	template<typename T>
	inline T Max( T a, T b ){

		return a > b ? a : b;
	}

	//========================================================================
	// min max clamping
	// TODO: try branchless version
	//========================================================================
	template<typename T>	
	inline void MinClamp( T & f_p, T min_p = (T)0 ){

		if( f_p < min_p ) f_p = min_p;
	}
	template<typename T>
	inline void MaxClamp( T & f_p, T max_p = (T)1.0f ){

		if( f_p > max_p ) f_p = max_p;
	}

//#include <xmmintrin.h>
//
//	float minss ( float a, float b )
//	{
//		// Branchless SSE min.
//		_mm_store_ss( &a, _mm_min_ss(_mm_set_ss(a),_mm_set_ss(b)) );
//		return a;
//	}
//
//	float maxss ( float a, float b )
//	{
//		// Branchless SSE max.
//		_mm_store_ss( &a, _mm_max_ss(_mm_set_ss(a),_mm_set_ss(b)) );
//		return a;
//	}
//
//	float clamp ( float val, float minval, float maxval )
//	{
//		// Branchless SSE clamp.
//		// return minss( maxss(val,minval), maxval );
//
//		_mm_store_ss( &val, _mm_min_ss( _mm_max_ss(_mm_set_ss(val),_mm_set_ss(minval)), _mm_set_ss(maxval) ) );
//		return val;
//	}

	//========================================================================
	// verify if given number is power of 2
	//========================================================================
	inline bool IsPowerOfTwo_zeroUnaware( int value_p ){

		return ( value_p & ( value_p -1 ) ) == 0;
	}
	inline bool IsPowerOfTwo( int value_p ){

		return  value_p
				&&
				( value_p & ( value_p -1 ) ) == 0;
	}

	//========================================================================
	// uvRect should be x, y, w, h
	//========================================================================
	inline void FlipUVRectHorz( float uv_p[4] ){

		uv_p[0] += uv_p[2];
		uv_p[2] = -uv_p[2];
	}
	inline void FlipUVRectVertc( float uv_p[4] ){

		uv_p[1] += uv_p[3];
		uv_p[3] = -uv_p[3];
	}

	namespace array2Dto1D{
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
		//
		inline int indexFromXY( int x_p, int y_p, int iW_p ){

			return x_p + y_p * iW_p;
		}
	}

	namespace array3Dto1D{

		//------------------------------------------------------------------------
		// array 3d <-> 1d conversions
		//------------------------------------------------------------------------
	
		inline int XFromIndex( int index_p, int iW_p ){

			return index_p % iW_p;
		}
		inline int ZFromIndex( int index_p, int iW_p, int iD_p ){

			return (index_p / iW_p) % iD_p;
		}
		inline int YFromIndex( int index_p, int iW_p, int iD_p ){

			return index_p / ( iW_p * iD_p );
		}
		//
		inline int indexFromXYZ( int x_p, int y_p, int z_p,  int iW_p, int iD_p ){

			return x_p + (z_p * iW_p) + (y_p * iW_p * iD_p);
		}
	}


	//------------------------------------------------------------------------
	// -assuming centralized world
	// -with y going down on negative
	// - starting from 0.0, 0.0 (first tile)
	//------------------------------------------------------------------------
	inline int WorldXToTileX( float x_p, float tileW_p ){

		return  int( ( x_p / tileW_p ) + 0.5f );
	}
	inline int WorldYToTileY( float y_p, float tileH_p ){

		return  -int( ( y_p / tileH_p ) - 0.5f );
	}
	inline void WordToTileXY( float x, float y, int & iX, int &iY, float tileW_p, float tileH_p )
	{
		// this commented out also works
		//iX = (int)((x + CurrentMap().hHalfSpc) / CurrentMap().hSpacing);
		//iY = -(int)((y - CurrentMap().vHalfSpc) / CurrentMap().vSpacing);

		iX =  int( ( x / tileW_p ) + 0.5f );
		iY = -int( ( y / tileH_p ) - 0.5f );
	}
	//------------------------------------------------------------------------
	// -assuming centralized world
	// - starting from 0.0, 0.0 (first tile)
	//------------------------------------------------------------------------
	inline void WordToTileXYZ( float x, float y, float z, int & iX, int &iY, int &iZ, float tileW_p, float tileH_p, float tileZ_p )
	{
		iX =  int( ( x / tileW_p ) + 0.5f );
		iY =  int( ( y / tileH_p ) + 0.5f );
		iZ =  int( ( z / tileZ_p ) + 0.5f );

		/*iX =  int( ( (x + 0.5f)/ tileW_p )  );
		iY =  int( ( (y + 0.5f)/ tileH_p )  );
		iZ =  int( ( (z + 0.5f)/ tileZ_p )  );*/
	}

	//------------------------------------------------------------------------
	// copies the 'rect' of a on b, without modifying anything else
	//------------------------------------------------------------------------
	template< typename T >
	inline void CopyPaste2DArray_AtoB(	T * pA_p, int aW_p, int aH_p,
										T * pB_p, int bW_p, int bH_p  ){

		int h = bH_p < aH_p ? bH_p : aH_p;
		int w = bW_p < aW_p ? bW_p : aW_p;

		for( int y = 0; y < h; ++y ){
			for( int x = 0; x < w; ++x ){

				int indexA = indexFromXY( x, y, aW_p );
				int indexB = indexFromXY( x, y, bW_p );

				pB_p[indexB] = pA_p[indexA];
			}
		}
	}

	template<typename T>
	static inline bool IsSameDir(const T a, const T b){

		return ( a > 0 ) == (b > 0);
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
	// heap stuff
	//========================================================================
	namespace binaryHeapUtil{

		// untested, this is from old code (old code works..)
		// prefer std::make_heap, pop_heap, push_heap

		inline int ParentNode( uint32_t index_p ){

			return int ((index_p - 1)/2.0f);
		}
		inline int LeftChildNode( uint32_t index_p ){

			return index_p*2 + 1;
		}
		inline int RightChildNode( uint32_t index_p ){

			return index_p*2 + 2;
		}
		template< typename T >
		inline void BubbleNodeUp( uint32_t index_p, T array_p[], uint32_t size_p ){

			uint32_t lastIndex = size_p-1;
			while( index_p < lastIndex ){

				// step 1: check if childs have higher value

				int biggerchild = LeftChildNode(index_p);	// start as left child
				if( biggerchild > lastIndex ) return;		// no childs, finish

				int rightchildID = biggerchild + 1;

				if( rightchildID <= lastIndex ){			// if theres a right child

					//then biger child is updated to right child, if right>biger

					if( array_p[rightchildID] > array_p[biggerchild] )
						biggerchild = rightchildID;
				}

				//if childs bigger than father, swap:

				if( array_p[biggerchild] > array_p[biggerchild] ){

					// swap
					T tmp = array_p[biggerchild];
					array_p[biggerchild] = array_p[index_p];
					array_p[index_p] = tmp;

					index_p = biggerchild; // next step the father will be the swapped child (bubbling down)	
				}
				else return; // if father is bigger, then startID to down is ordered
			}
		}

	}

	//========================================================================
	// string stuff
	//========================================================================
	namespace stringUtil{

#define GEN_MAXSTRINGSIZE 2048

		inline uint32_t CountString( const char* szString_p, uint32_t nMaxSize_p = GEN_MAXSTRINGSIZE )
		{
			uint32_t counter = 0;

			while( szString_p[counter] != 0x00 ){
				++counter;
				if( counter > nMaxSize_p )
					break;
			}

			return counter;
		}
		inline uint32_t CountWString( const wchar_t * szString_p, uint32_t nMaxSize_p = GEN_MAXSTRINGSIZE )
		{
			uint32_t counter = 0;

			while( szString_p[counter] != 0x0000 ){

				++counter;
				if( counter > nMaxSize_p )
					break;
			}

			return counter;
		}
		inline bool GCompareString( const char* string1_p, const char* string2_p, uint32_t nMaxSize_p = GEN_MAXSTRINGSIZE )
		{
			// the G is due name claching on stupid WinNIS.h or something

			uint32_t it = 0;

			while( string1_p[it] == string2_p[it] )
			{
				if( string1_p[it] == 0x00 )
				{
					return true;
				}
				else{
					++it;
					if( it > nMaxSize_p )
						break;
				}
			}

			return false;
		}
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
	// 
	//========================================================================
	template< typename T >
	class CircularBuffer{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		CircularBuffer()
			:
		m_data(nullptr){}

		void Initialize( uint32_t size_p ){

			if( m_data ) delete [] m_data;

			m_data = new T[size_p];
			m_currentIndex = 0;
			m_size = size_p;
		}
		CircularBuffer( uint32_t size_p )
			:
		m_data(nullptr){

			Initialize( size_p );
		}
		~CircularBuffer(){

			if( m_data ) delete [] m_data;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void clear(){

			m_currentIndex = 0;
		}

		//------------------------------------------------------------------------
		// accessors operators
		//------------------------------------------------------------------------
		T & operator[]( uint32_t index_p ){

			assert( index_p < m_size );
			return m_data[index_p];
		}
		T & operator() (){

			return m_data[m_currentIndex];
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		uint32_t GetCurrentIndex() const { return m_currentIndex; }

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		T operator++( int ){// the int param means postfix

			++m_currentIndex;
			if( m_currentIndex == m_size )
				m_currentIndex = 0;

			return (*this)();
		}
		T & operator++(){

			++m_currentIndex;
			if( m_currentIndex == m_size )
				m_currentIndex = 0;

			return (*this)();
		}

	private:

		uint32_t m_currentIndex;
		T * m_data;
		uint32_t m_size;
	};

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

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		void QueueChunkAs( const TYPE & newData_p ){

			assert( m_currentByteIndex + sizeof(TYPE) <= m_size );

			*((TYPE*) (&(m_data[m_currentByteIndex])) ) = newData_p;

			m_currentByteIndex += sizeof(TYPE);
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename TYPE >
		TYPE GetChunkAs( unsigned int iByteOffset_p ){

			assert( iByteOffset_p + sizeof(TYPE) <= m_size );

			return   *((TYPE*) (&(m_data[iByteOffset_p])) );
		}

		template< typename TYPE >
		TYPE GetNextChunkAs(){

			assert( m_currentByteIndex + sizeof(TYPE) <= m_size );
			
			TYPE * pT = ((TYPE*) (&(m_data[m_currentByteIndex])) );
			
			m_currentByteIndex += sizeof(TYPE);

			return *pT;
		}

		/*void GetNextChunk( unsigned char * pData, unsigned int iSize ){

			assert( m_currentByteIndex + iSize <= m_size );

			memcpy( pData, &m_data[m_currentByteIndex], iSize );

			m_currentByteIndex += iSize;
		}*/
	};

	//------------------------------------------------------------------------
	// da fuck did I do this for? TODO
	//------------------------------------------------------------------------
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


	template<typename T>
	class Array{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Array()
			:
		m_data(nullptr){}

		void Initialize( uint32_t size_p ){

			if( m_data ) delete [] m_data;

			m_data = new T[size_p];
			m_size = size_p;
		}
		Array( uint32_t size_p )
			:
		m_data(nullptr){

			m_data = new T[size_p];
			m_size = size_p;
		}
		~Array(){

			if( m_data ) delete [] m_data;
		}

		//------------------------------------------------------------------------
		// accessors operators
		//------------------------------------------------------------------------
		T & operator[]( uint32_t index_p ){

			assert( index_p < m_size );
			return m_data[index_p];
		}

		uint32_t Size() const{

			return m_size;
		}

	private:

		uint32_t m_size;
		T *m_data;

	};

	template<typename T, uint32_t SIZE>
	class Stack{

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		Stack()
			:
		m_topIndex(0){}

		uint32_t MaxSize() const{

			return SIZE;
		}
		uint32_t Size() const{

			return m_topIndex;
		}

		T* StackAlloc( uint32_t count_p = 1 ){

			assert( m_topIndex + count_p <= SIZE );

			T* pOffset = &m_data[m_topIndex];
			m_topIndex += count_p;

			return pOffset;
		}
		void Unstack( uint32_t count_p = 1 ){

			assert( m_topIndex > 0 );
			m_topIndex -= count_p;
		}

		void Reset(){

			m_topIndex = 0;
		}

		//------------------------------------------------------------------------
		// accessors operators
		//------------------------------------------------------------------------
		T & operator[]( uint32_t index_p ){

			assert( index_p < m_topIndex );
			return m_data[index_p];
		}
		const T & operator[]( uint32_t index_p )const{

			assert( index_p < m_topIndex );
			return m_data[index_p];
		}

	private:

		uint32_t m_topIndex; 
		T m_data[SIZE];
	};
}

#include "Tweening.h"
#include "Delegate.h"
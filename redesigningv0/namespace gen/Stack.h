#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2016/04/07
	created:	7:4:2016   22:57
	file:		Stack.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	fixed size stack

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes

namespace gen{

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

		void PushBack(const T & element_p){

			T* newElement = StackAlloc();
			*newElement = element_p;
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
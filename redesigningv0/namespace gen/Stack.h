#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2016/04/07
	created:	7:4:2016   22:57
	file:		Stack.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	fixed size stack, those are containers, not allocators

	� Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes

namespace gen{

	template<typename T, uint32_t SIZE = (uint32_t)-1>
	class Stack{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		Stack()
			:
		m_topIndex(0){}

		constexpr static uint32_t MaxSize(){

			return SIZE;
		}

		uint32_t Size() const{

			return m_topIndex;
		}


		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
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


		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
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

		T & Last(){
			assert( m_topIndex > 0 );
			return m_data[m_topIndex-1];
		}
		const T & Last()const{
			assert( m_topIndex > 0 );
			return m_data[m_topIndex-1];
		}

	private:

		uint32_t m_topIndex; 
		T m_data[SIZE];
	};


	
	//========================================================================
	// version that receives pointer to mem and size
	//========================================================================
	template<typename T>
	class Stack<T, (uint32_t)-1>{

	public:
		
		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		Stack(T* mem_p, uint32_t size_p)
			:
			m_topIndex(0), m_size(size_p), m_data(mem_p)
		{}

		Stack()
			:
			m_topIndex(0), m_size(0), m_data(nullptr)
		{}

		//------------------------------------------------------------------------
		// note all functions have assertions that already will trigger if this is
		// not called
		//------------------------------------------------------------------------
		void Initialize(T* mem_p, uint32_t size_p) {

			assert(m_data == nullptr);

			m_size = size_p;
			m_data = mem_p;
		}
		void Reinitialize(T* mem_p, uint32_t size_p) {

			m_size = size_p;
			m_data = mem_p;
		}

		uint32_t MaxSize() const {

			return m_size;
		}
		uint32_t Size() const {

			return m_topIndex;
		}


		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		T* StackAlloc(uint32_t count_p = 1) {

			assert(m_topIndex + count_p <= m_size);

			T* pOffset = &m_data[m_topIndex];
			m_topIndex += count_p;

			return pOffset;
		}
		void Unstack(uint32_t count_p = 1) {

			assert(m_topIndex > 0);
			m_topIndex -= count_p;
		}


		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void PushBack(const T & element_p) {

			T* newElement = StackAlloc();
			*newElement = element_p;
		}

		void Reset() {

			m_topIndex = 0;
		}

		//------------------------------------------------------------------------
		// accessors operators
		//------------------------------------------------------------------------
		T & operator[](uint32_t index_p) {

			assert(index_p < m_topIndex);
			return m_data[index_p];
		}
		const T & operator[](uint32_t index_p)const {

			assert(index_p < m_topIndex);
			return m_data[index_p];
		}

		T & Last() {
			assert(m_topIndex > 0);
			return m_data[m_topIndex - 1];
		}
		const T & Last()const {
			assert(m_topIndex > 0);
			return m_data[m_topIndex - 1];
		}


		//------------------------------------------------------------------------
		// assign operator
		// mem copy, assert stack size is enough
		//------------------------------------------------------------------------
		const Stack& operator = (const Stack & other) {

			assert(m_size != 0 && other.m_topIndex != 0);
			assert(m_size >= other.m_topIndex);
			
			m_topIndex = other.m_topIndex;

			memcpy( (void*)m_data, (void*)other.m_data, other.m_topIndex * sizeof(T) );

			return *this;
		}

		////------------------------------------------------------------------------
		//// shalow copy
		//// copy meant to be readonly, modifying this obj after calling this
		//// will mostly screw the original copied object
		////------------------------------------------------------------------------
		//void ReadOnlyCopy(const Stack & other) {

		//	m_topIndex = other.m_topIndex;
		//	m_size = other.m_size;
		//	m_data = other.m_data;
		//}

	private:

		uint32_t m_topIndex;
		uint32_t m_size;
		T*		m_data;
	};
}
#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/01/28
	created:	28:1:2014   15:04
	file:		Allocator.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes

namespace mem{

	//------------------------------------------------------------------------
	// to track the mem allocations by systems
	//------------------------------------------------------------------------
	enum E_MEM_SYSTEM{

		E_MEM_SYSTEM_GEN = 0,
		E_MEM_SYSTEM_GAME,
		E_MEM_SYSTEM_RENDER,
		E_MEM_SYSTEM_SPRITE,
		E_MEM_SYSTEM_TEXT,
		E_MEM_SYSTEM_SOUND,
		E_MEM_SYSTEM_WIN,
		E_MEM_SYSTEM_NET,
		E_MEM_SYSTEM_MAX
	};

	//------------------------------------------------------------------------
	// computes amount to be added to an address to it become aligned
	//------------------------------------------------------------------------
	inline int OffsetRequiredForAlignment( const uintptr_t address_p, const int alignment_p ){

		const uintptr_t mask = alignment_p - 1;
		uintptr_t amountMisaligned = (address_p & mask );
		return (alignment_p - int(amountMisaligned)) & int(mask);

		/*if (amountMisaligned == 0) return 0;
		else return (int)(alignment_p - amountMisaligned);*/
	}

	inline int BackwardOffsetRequiredForAlignment(const uintptr_t address_p, const int alignment_p) {

		const uintptr_t mask = alignment_p - 1;
		uintptr_t amountMisaligned = (address_p & mask);
		return (int)amountMisaligned;
	}

	template<class T>
	bool IsAligned(const void * ptr) {
		auto iptr = reinterpret_cast<uintptr_t>(ptr);
		return !(iptr % alignof(T));
	}
	bool IsAligned(const void * ptr, int alignment_p) {
		auto iptr = reinterpret_cast<uintptr_t>(ptr);
		return !(iptr % alignment_p);
	}

	bool IsPowerOfTwo(const uintptr_t x)
	{
		return (x != 0) && ((x & (x - 1)) == 0);
	}


	template <typename T, typename H, class CustomAllocator>
	T* allocateWithHeader(CustomAllocator& allocator)
	{
		struct addHeader {
			H header;
			T userData;
		};

		return new (allocator.allocate(sizeof(addHeader), __alignof(addHeader))) addHeader;
	}
	template <typename T, typename H, class CustomAllocator>
	T* deallocateWithHeader(CustomAllocator& allocator, T * ptr)
	{
		struct addHeader {
			H header;
			T userData;
		};


		int offset = offsetof(addHeader, userData);


		return new (allocator.allocate(sizeof(addHeader), __alignof(addHeader))) addHeader;
	}

	//------------------------------------------------------------------------
	// templated Delete() is used to avoid a type parameter on the macro MEM_DELETE,
	// this works due compiler template arg type deduction
	//------------------------------------------------------------------------
	template< typename T, class CustomAllocator >
	void Delete( T * ptr_p, CustomAllocator allocator_p ){

		ptr_p->~T();
		allocator_p.Free( ptr_p );
	}

	//------------------------------------------------------------------------
	// placement new must be called for each instance on the array, so it
	// cant just be a macro returning a ptr without a function like this
	//------------------------------------------------------------------------
	template< typename T, class CustomAllocator >
	void * NewArrayPOD( size_t N_p, CustomAllocator allocator_p ){

		union{
			void	* as_void;
			size_t	* as_size_t;
			T		* as_T;
		}ptr;

		ptr.as_void = allocator_p.AllocateAligned( sizeof(T) * N_p, __alignof(T) );
	}

	//------------------------------------------------------------------------
	// the purpose of the macro is allow ctor arguments naturally, while using
	// a custom allocator
	// (otherwise id opt for a static function)
	//------------------------------------------------------------------------
#define MEM_NEW( T, CustomAllocator )\
	new ( mem::CustomAllocator.AllocateAligned( sizeof(T), __alignof(T) ) T

#define MEM_DELETE( Ptr, CustomAllocator )\
	mem::Delete( Ptr, CustomAllocator );


	class RawAllocator{

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void * AllocateAligned( size_t size_p, int alignment_p ){
		
			void * ptr = malloc( size_p + alignment_p );
			int offset = OffsetRequiredForAlignment( (uintptr_t)ptr, alignment_p );
			ptr =  (void*) ((char*)ptr + offset);
			return ptr;
		}
		//void FreeAligned( void * ptr_p, size_t alignment_p ){


		//}


	private:
	};



	class Allocator : public gen::NonCopyable {

	public:

		//-----------------------------------
		// ctor
		//-----------------------------------

		Allocator(void* pMem, size_t size)
			:
			m_pMem((uint8_t *)pMem), m_size(size), m_topIndex(0){}


		void * AllocAlign(size_t size, int aligment) {

			assert(IsPowerOfTwo(aligment));

			int headerOffset = OffsetRequiredForAlignment((uintptr_t)&m_pMem[m_topIndex], alignof(Header));
			size_t headerSpace = headerOffset + sizeof(Header);

			int dataOffset = OffsetRequiredForAlignment((uintptr_t)&m_pMem[m_topIndex + headerSpace], aligment);
			size_t dataSpace = dataOffset + size;

			// [previously allocated mem][pad][header(offset to orig)(totalsize)][pad][data(user stuff)]

			size_t newSize = dataSpace + headerSpace;

			if (newSize + m_topIndex > m_size) return nullptr;



			uint8_t * p = &m_pMem[m_topIndex];

			Header * pHeader = (Header*)(p + headerOffset);
			assert(IsAligned<Header>(pHeader));
			pHeader->offsetFromOriginalAllocatedPointer = headerOffset;
			pHeader->totalSize = newSize;

			p = &m_pMem[m_topIndex + headerSpace + dataOffset];
			assert(IsAligned(p, aligment));

			m_topIndex += newSize;

			return p;
		}

		void DeallocAlign(void * ptr) {

			uint8_t * p = reinterpret_cast<uint8_t*>(ptr);

			p -= sizeof(Header);

			int headerOffset = BackwardOffsetRequiredForAlignment((uintptr_t)p, alignof(Header));
			Header * pHeader = (Header*)(p - headerOffset);

			m_topIndex -= pHeader->totalSize;
		}



	private:

		uint8_t * m_pMem;
		size_t m_topIndex;
		size_t m_size;


		Allocator();

		//------------------------
		// custom data
		//------------------------
		struct Header {
			size_t totalSize;
			int offsetFromOriginalAllocatedPointer;
		};
	};
}
// testing

//unsigned char buffer[32];
//
//void* ptr = &buffer[1];
//
//int * intPtr = reinterpret_cast<int*>(ptr);
//
//if (!mem::IsAligned<int>(intPtr)) {
//
//	int offToAlign = mem::OffsetRequiredForAlignment((uintptr_t)intPtr, alignof(int));
//
//	//intPtr += offToAlign; this will displace as if in an int array
//	intPtr = (int*)((char*)intPtr + offToAlign);
//
//	assert(mem::IsAligned<int>(intPtr));
//}
//
//*intPtr = 0xffffffff;
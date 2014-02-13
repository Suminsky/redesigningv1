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
	inline int OffsetRequiredForAlignment( uintptr_t address_p, int alignment_p ){

		uintptr_t mask = alignment_p - 1;
		uintptr_t amountMisaligned = (address_p & mask );
		int offset = (int)(alignment_p - amountMisaligned);

		if( offset == alignment_p ) offset = 0;

		return offset;
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
		//void * AllocateAligned( size_t size_p, size_t alignment_p ){
		//
		//	/*void * ptr = malloc( size_p + alignment_p );
		//	int offset = OffsetRequiredForAlignment( (uintptr_t)ptr, alignment_p );
		//	ptr =  (void*) ((uintptr_t)ptr + offset);
		//	(uintptr_t)ptr
		//	return ptr;*/
		//}
		//void FreeAligned( void * ptr_p, size_t alignment_p ){


		//}


	private:
	};
}
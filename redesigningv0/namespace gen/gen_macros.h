//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/05
	created:	5:2:2013   15:45
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace win\win_macros.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace win
	file base:	win_macros
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	general macros

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

// DEBUG MACROS

#ifdef _DEBUG
#define COMMA ,
#define DBG(param) param
//remember of COMMA() in the case of future problems
#define BREAKHERE {int breakhere = 7; breakhere = breakhere; }
#define NDBG(param)
#else
#define COMMA
#define DBG(param)
#define BREAKHERE {}
#define NDBG(param) param
#endif

#ifdef _DEBUG
#define keepAssert(x) assert(x)
#else
#define keepAssert(x) x
#endif // _DEBUG

#include <exception>
//------------------------------------------------------------------------
// this macro simply implements aligned malloc overloaded new and delete
// but only if its not a x64 build, on x64 builds align is already by 16
// disallow placement new
// placement new is used by std::make_shared, which means mem will not
// be properly aligned
// PRIVATE DOESNT SOLVE
//------------------------------------------------------------------------
		//void* operator new ( size_t size_p, void * p_p ){return p_p;}
		//void operator delete (void*, void*){}
#ifndef _WIN64
#define ALLIGN16ONLY 													\
						void* operator new( size_t size_p){				\
							void * p =  _aligned_malloc(size_p, 16);	\
							if( p == NULL)								\
								throw std::bad_alloc();					\
							return p;									\
						}												\
						void operator delete(void* p_p){				\
							_aligned_free(p_p);							\
						}												
						

//------------------------------------------------------------------------
// this macro allows single line initialization for both shared pointer
// creation versions
//------------------------------------------------------------------------
#define MAKE_SHARED_ALIGN16(s_ptr, type, ctor)\
	s_ptr = std::move(std::shared_ptr<type>(new type ctor))
//s_ptr ( new type ctor ); // tem como pegar o tipo por macro?
// this way works only if you instantiate locally (type + name), the mess Im
// using now works both with type + name and name only..
	
#else

#define ALLIGN16ONLY

#define MAKE_SHARED_ALIGN16(s_ptr, type, ctor)\
	s_ptr = std::make_shared<type> ctor;

#endif


//------------------------------------------------------------------------
// macro to release the burden of typing all the enormous shit
//------------------------------------------------------------------------
#include <memory>
#include "gen_data.h"

#define MAKE_STACK_SHAREDPTR( rawType, stackPtr ) std::shared_ptr<rawType>( stackPtr, &gen::NoOp<rawType> )
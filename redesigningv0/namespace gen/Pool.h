#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/14
	created:	14:11:2013   21:34
	file:		PoolTry.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <assert.h>

// private includes
#include "Delegate.h"

namespace gen{

	//========================================================================
	// 
	//========================================================================
	template< typename T >
	class Pool{

		// NOTE: T needs to have a member of type unsigned int called m_iCurrentRosterIndex, assessable
		// from this class

		typedef unsigned int uint;

		struct Roster{

			struct RosterElement{

				uint iPoolIndex;
				uint nTimesFree;
			};

			//------------------------------------------------------------------------
			// ctor
			//------------------------------------------------------------------------
			Roster( uint size_p )
				:
			m_size(size_p){

				m_elements = new RosterElement[size_p];

				ResetRoster(); 
			}
			~Roster(){

				delete [] m_elements;
			}

			//------------------------------------------------------------------------
			// set the initial indexes that should be in order
			//------------------------------------------------------------------------
			void ResetRoster(){

				m_iFreeIndexesStart = 0;

				for( uint it = 0; it < m_size; ++it ){

					m_elements[it].iPoolIndex = it;
					m_elements[it].nTimesFree = 0;
				}
			}

			
			uint m_iFreeIndexesStart;
			RosterElement * m_elements;

		private:
			uint m_size;
		};

	public:

		//------------------------------------------------------------------------
		// sorta like an iterator..but it doesnt hold a "current"
		//------------------------------------------------------------------------
		class PoolAccessor{

		public:

			//------------------------------------------------------------------------
			// ctor
			//------------------------------------------------------------------------
			PoolAccessor( T * pPool_p, Roster * pRoster_p )
				:
			m_pPoolRef(pPool_p),
			m_pRosterRef(pRoster_p)
			{}

			//------------------------------------------------------------------------
			// 
			//------------------------------------------------------------------------
			T * GetAllocated( uint it_p ){

				assert( it_p < m_pRosterRef->m_iFreeIndexesStart );

				return &m_pPoolRef[ m_pRosterRef->m_elements[it_p].iPoolIndex ];
			}

			//------------------------------------------------------------------------
			// 
			//------------------------------------------------------------------------
			uint GetNAllocated()const{return m_pRosterRef->m_iFreeIndexesStart;}

		private:

			T * m_pPoolRef;
			Roster * m_pRosterRef;

			//------------------------------------------------------------------------
			// prevents warning
			//------------------------------------------------------------------------
			//void operator = (const PoolAccessor &);
		};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		Pool( uint size_p )
			:
		m_roster(size_p), m_size(size_p){

			//m_pool = new T[m_size];
			m_pool = (T*)malloc( sizeof(T) * size_p );

			for( uint it = 0; it < m_size;++it ){

				//new (&m_pool[it]) T();

				m_pool[it].m_iCurrentRosterIndex = it;
			}

			// NOTE: a reset pool method would need to default initialize each member
		}
		~Pool(){

			/*for( uint it = 0; it < m_size;++it ){

				m_pool[it].~T();
			}*/

			//delete [] m_pool;
			free( m_pool );
		}

		//------------------------------------------------------------------------
		// allocate
		//------------------------------------------------------------------------
		T* Allocate(){

			assert( m_roster.m_iFreeIndexesStart < m_size );

			return new (&m_pool[m_roster.m_elements[m_roster.m_iFreeIndexesStart++].iPoolIndex]) T();

			//return &m_pool[m_roster.m_elements[m_roster.m_iFreeIndexesStart++].iPoolIndex];
		}

		//------------------------------------------------------------------------
		// deallocate
		//------------------------------------------------------------------------
		void Free( const T* pT_p ){

			pT_p->~T();

			uint iRosterIndex_p = pT_p->m_iCurrentRosterIndex;
			assert( iRosterIndex_p < m_roster.m_iFreeIndexesStart );


			uint iRosterIndexLastAllocated = m_roster.m_iFreeIndexesStart-1;

			// swap given rooster element with last allocated element, update pool element roster index

			Roster::RosterElement lastAllocated = m_roster.m_elements[iRosterIndexLastAllocated];

			m_roster.m_elements[iRosterIndexLastAllocated] = m_roster.m_elements[iRosterIndex_p];
			++m_roster.m_elements[iRosterIndexLastAllocated].nTimesFree;
			m_pool[m_roster.m_elements[iRosterIndexLastAllocated].iPoolIndex].m_iCurrentRosterIndex = iRosterIndexLastAllocated;

			m_roster.m_elements[iRosterIndex_p] = lastAllocated;
			m_pool[m_roster.m_elements[iRosterIndex_p].iPoolIndex].m_iCurrentRosterIndex = iRosterIndex_p;

			// "pop"

			--m_roster.m_iFreeIndexesStart;
		}
		//------------------------------------------------------------------------
		// hackish solution for having the same type for the delegate on
		// pool_ptr
		//------------------------------------------------------------------------
		void Free( const void* pT_p ){

			((T*)pT_p)->~T();

			uint iRosterIndex_p = ((T*)pT_p)->m_iCurrentRosterIndex;
			assert( iRosterIndex_p < m_roster.m_iFreeIndexesStart );


			uint iRosterIndexLastAllocated = m_roster.m_iFreeIndexesStart-1;

			// swap given rooster element with last allocated element, update pool element roster index

			Roster::RosterElement lastAllocated = m_roster.m_elements[iRosterIndexLastAllocated];

			m_roster.m_elements[iRosterIndexLastAllocated] = m_roster.m_elements[iRosterIndex_p];
			++m_roster.m_elements[iRosterIndexLastAllocated].nTimesFree;
			m_pool[m_roster.m_elements[iRosterIndexLastAllocated].iPoolIndex].m_iCurrentRosterIndex = iRosterIndexLastAllocated;

			m_roster.m_elements[iRosterIndex_p] = lastAllocated;
			m_pool[m_roster.m_elements[iRosterIndex_p].iPoolIndex].m_iCurrentRosterIndex = iRosterIndex_p;

			// "pop"

			--m_roster.m_iFreeIndexesStart;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		uint GetNAllocated()const{return m_roster.m_iFreeIndexesStart;}
		uint GetMaxSize() const{ return m_size; }

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		inline PoolAccessor GetAccess(){

			return PoolAccessor(m_pool, &m_roster);
		}

	private:
		
		uint m_size;
		Roster m_roster;
		T * m_pool;
	};

	//========================================================================
	// 
	//========================================================================
	class RefCounting{

	private:
		unsigned int m_iRefCount;

	public:

		RefCounting():m_iRefCount(0){}

		void AddRef(){ ++m_iRefCount; }
		unsigned int ReleaseRef(){ return --m_iRefCount; }
	};

	//========================================================================
	// shared intrusive pool pointer
	//========================================================================
	template< typename T >
	class pool_ptr{

		template<typename U> friend class pool_ptr;

		T * pData; // must have a RefCounting (and be accessible to pool_ptr), use the helper macro DCL_POOLELEMENT
		Delegate1Param<const void*> deleter;

	public:

		//------------------------------------------------------------------------
		// def ctor
		//------------------------------------------------------------------------
		pool_ptr()
			:
		pData(nullptr){}

		//------------------------------------------------------------------------
		// new up ctor
		//------------------------------------------------------------------------
		explicit pool_ptr( Pool<T> * pPool_p ){

			pData = pPool_p->Allocate();
			deleter.Set<Pool<T>, &Pool<T>::Free>( pPool_p );

			pData->m_refCount.AddRef();
		}

		//------------------------------------------------------------------------
		// generalized new up ctor
		//------------------------------------------------------------------------
		template< typename U >
		pool_ptr( Pool<U> & pool_p ){

			pData = (T*)pool_p.Allocate();
			deleter.Set<Pool<U>, &Pool<U>::Free>( &pool_p );

			pData->m_refCount.AddRef();
		}

		//------------------------------------------------------------------------
		// cpy ctor
		//------------------------------------------------------------------------
		pool_ptr( const pool_ptr & other_p ){

			pData = other_p.pData;
			deleter = other_p.deleter;

			pData->m_refCount.AddRef();
		}
		//------------------------------------------------------------------------
		// generalized cpy ctor
		//------------------------------------------------------------------------
		template< typename U >		
		pool_ptr( const pool_ptr<U> & other_p ){

			pData = (T*)other_p.Get();
			deleter = other_p.deleter;

			pData->m_refCount.AddRef();
		}

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		~pool_ptr(){

			if( pData ){

				if( !pData->m_refCount.ReleaseRef() ){

					deleter( pData );
				}
			}
		}

		//------------------------------------------------------------------------
		// assignment
		//------------------------------------------------------------------------
		pool_ptr & operator = ( const pool_ptr & other_p ){

			// release if already holding data

			if( pData ){

				if( !pData->m_refCount.ReleaseRef() ){

					deleter( pData );
				}
			}

			pData = other_p.pData;
			deleter = other_p.deleter;

			pData->m_refCount.AddRef();

			return *this;
		}
		//------------------------------------------------------------------------
		// generalized assignment
		//------------------------------------------------------------------------	
		template< typename U >
		pool_ptr & operator = ( const pool_ptr<U> & other_p ){

			// release if already holding data

			if( pData ){

				if( !pData->m_refCount.ReleaseRef() ){

					deleter( pData );
				}
			}

			pData = (T*)other_p.Get();
			deleter = other_p.deleter;

			pData->m_refCount.AddRef();

			return *this;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void New( Pool<T> * pPool_p ){

			assert( !pData );

			pData = pPool_p->Allocate();
			deleter.Set<Pool<T>, &Pool<T>::Free>( pPool_p );

			pData->m_refCount.AddRef();
		}
		void Release(){

			assert( pData );

			if( !pData->m_refCount.ReleaseRef() ){

				deleter( pData );
			}

			pData = nullptr;
		}

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		T* Get(){ return pData;	}
		T* Get()const{ return pData;	}
		T* operator ->(){ return pData; }
		T* operator ->() const { return pData; }

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		operator bool() const{

			return pData != nullptr;
		}
	};
}

#define DCL_POOLELEMENT()\
	template<typename T> friend class gen::Pool;\
	template<typename T> friend class gen::pool_ptr;\
	gen::RefCounting	m_refCount;\
	unsigned int		m_iCurrentRosterIndex
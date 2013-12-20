#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/14
	created:	14:11:2013   21:34
	file:		PoolTry.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	T needs to have a member of type unsigned int called m_iCurrentRosterIndex, assessable
				to this class

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes

namespace gen{

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


	template< typename T, unsigned int SIZE >
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
			Roster(){ ResetRoster(); }

			//------------------------------------------------------------------------
			// set the initial indexes that should be in order
			//------------------------------------------------------------------------
			void ResetRoster(){

				m_iFreeIndexesStart = 0;

				for( uint it = 0; it < SIZE; ++it ){

					m_elements[it].iPoolIndex = it;
					m_elements[it].nTimesFree = 0;
				}
			}

			uint m_iFreeIndexesStart;
			RosterElement m_elements[SIZE];			
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
			PoolAccessor( T pPool_p[], Roster * pRoster_p )
				:
			m_pPoolRef((T(*)[SIZE])pPool_p),
			m_pRosterRef(pRoster_p)
			{}

			//------------------------------------------------------------------------
			// 
			//------------------------------------------------------------------------
			T * GetAllocated( uint it_p ){

				assert( it_p < m_pRosterRef->m_iFreeIndexesStart );

				return &(*m_pPoolRef)[ m_pRosterRef->m_elements[it_p].iPoolIndex ];
			}

			//------------------------------------------------------------------------
			// 
			//------------------------------------------------------------------------
			uint GetNAllocated()const{return m_pRosterRef->m_iFreeIndexesStart;}

		private:

			T (*m_pPoolRef)[SIZE];
			Roster * m_pRosterRef;

			//------------------------------------------------------------------------
			// prevents warning
			//------------------------------------------------------------------------
			//void operator = (const PoolAccessor &);
		};

		//------------------------------------------------------------------------
		// shared intrusive pool pointer
		//------------------------------------------------------------------------
		class pool_ptr{

			T * pData; // must have a RefCounting (and be accessible to pool_ptr), use the helper macro DCL_POOLELEMENT
			Pool * pPoolRef;

		public:

			//------------------------------------------------------------------------
			// ctor
			//------------------------------------------------------------------------
			pool_ptr( const Pool & pool_p )
				:
			pPoolRef(&pool_p), pData(nullptr){}

			//------------------------------------------------------------------------
			// cpy ctor
			//------------------------------------------------------------------------
			pool_ptr( const pool_ptr & other_p ){

				pData = other_p.pData;
				pPoolRef = other_p.pPoolRef;

				pData->m_refCount.AddRef();
			}

			//------------------------------------------------------------------------
			// dctor
			//------------------------------------------------------------------------
			~pool_ptr(){

				if( pData ){

					if( !pData->m_refCount->ReleaseRef() ){

						pPoolRef->Free( pData );
					}
				}
			}

			//------------------------------------------------------------------------
			// 
			//------------------------------------------------------------------------
			void New(){

				assert( !pData );

				pData = pPoolRef->Allocate();
				pData->m_refCount->AddRef();
			}
			void Release(){

				assert( pData );

				if( !pData->m_refCount->ReleaseRef() ){

					pPoolRef->Free( pData );
				}

				pData = nullptr;
			}

			//------------------------------------------------------------------------
			// 
			//------------------------------------------------------------------------
			T* Get(){

				return pData;
			}

			//------------------------------------------------------------------------
			// assignment
			//------------------------------------------------------------------------
			pool_ptr & operator = ( const pool_ptr & other_p ){

				// release if already holding data

				if( pData ){

					if( !pData->m_refCount->ReleaseRef() ){

						pPoolRef->Free( pData );
					}
				}

				// assert from same pool? TODO

				pData = other_p.pData;
				pPoolRef = other_p.pPoolRef;

				pData->m_refCount.AddRef();

				return *this;
			}
		};

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		Pool(){

			for( uint it = 0; it < SIZE;++it ){

				m_pool[it].m_iCurrentRosterIndex = it;
			}

			// NOTE: a reset pool method would need to default initialize each member
		}

		//------------------------------------------------------------------------
		// allocate
		//------------------------------------------------------------------------
		T* Allocate(){

			assert( m_roster.m_iFreeIndexesStart < SIZE );

			return &m_pool[m_roster.m_elements[m_roster.m_iFreeIndexesStart++].iPoolIndex];
		}

		//------------------------------------------------------------------------
		// deallocate
		//------------------------------------------------------------------------
		void Free( const T* pT_p ){

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

			--m_iFreeIndexesStart;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		uint GetNAllocated()const{return m_roster.m_iFreeIndexesStart;}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		inline PoolAccessor GetAccess(){

			return PoolAccessor(m_pool, &m_roster);
		}

	private:
		
		Roster m_roster;
		T m_pool[SIZE];
	};

#define DCL_POOLELEMENT()\
	template<typename T, unsigned int SIZE> friend class gen::Pool;\
	template<typename T, unsigned int SIZE> friend class gen::Pool<T, SIZE>::pool_ptr;\
	RefCounting m_refCount;\
	unsigned int m_iCurrentRosterIndex

}
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


	template< typename T, unsigned int SIZE >
	class Pool{

		// NOTE: T needs to have a member of type unsigned int called m_iCurrentRosterIndex, assessable
		// from this class

		typedef unsigned int uint;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
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

		class PoolAccess;
		friend class PoolAccess;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		class PoolAccess{

		public:

			//------------------------------------------------------------------------
			// ctor
			//------------------------------------------------------------------------
			PoolAccess( T pPool_p[], Roster * pRoster_p )
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
			//void operator = (const PoolAccess &);
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
		PoolAccess GetAccess(){

			return PoolAccess(m_pool, &m_roster);
		}

	private:
		
		Roster m_roster;
		T m_pool[SIZE];
	};
}
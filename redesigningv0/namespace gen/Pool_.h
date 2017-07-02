#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2017, april 10

	author:		Icebone1000 (Giuliano Suminsky Pieta)

	purpose:	Pool container, inplace free list for finding free blocks;
				Cant allocate multiple objects, since they might not be
				sequential in mem;
				T size must be > than that of a ptr, to hold the free list inplace;

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes

// private includes

// fwd decls


namespace gen {


	//========================================================================
	// inplace freelist
	//========================================================================
	template< typename T, uint32_t SIZE >
	class Pool_FreeList {

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		Pool_FreeList() {

			static_assert(sizeof(FreeBlock) == sizeof(T), "size of T < size of ptr");
			assert((void*)&m_data[0] == (void*)&m_data[0].t);

			FreeBlock ** ppFB = &m_pCurrentFreeHead;

			for (int it = 0; it < SIZE; ++it) {

				*ppFB = &m_data[it];
				ppFB = &((*ppFB)->pNext);
			}

			*ppFB = nullptr;
		}


		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static uint32_t MaxSize() {

			return SIZE;
		}

		bool IsFull()const {

			return m_pCurrentFreeHead == nullptr;
		}

		//------------------------------------------------------------------------
		// allocator
		//------------------------------------------------------------------------
		T* PoolAlloc() {

			assert(m_pCurrentFreeHead != nullptr);

			T* pNew = &m_pCurrentFreeHead->t;

			m_pCurrentFreeHead = m_pCurrentFreeHead->pNext;
			
			return pNew;
		}

		//------------------------------------------------------------------------
		// deallocator
		//------------------------------------------------------------------------
		void PoolFree(T* p) {

			assert((void*)p >= (void*)&m_data[0] && (void*)p <= (void*)&m_data[SIZE-1]); // assert in range
			//assert((uintptr_t)p % sizeof(FreeBlock) == 0);
			assert((void*)&m_data[(((uintptr_t)p - (uintptr_t)&m_data[0])) / sizeof(FreeBlock)] == (void*)p); // assert is in a correct block

			FreeBlock * pFreeAgain = (FreeBlock*)p;

			pFreeAgain->pNext = m_pCurrentFreeHead;
			m_pCurrentFreeHead = pFreeAgain;
		}

	private:

		union FreeBlock {
			T t;
			FreeBlock * pNext;
		};

		FreeBlock * m_pCurrentFreeHead;
		FreeBlock	m_data[SIZE];
	};



	//========================================================================
	// inplace freelist
	// extra space for used double linked list
	// 
	// version that allocates extra pointers to link all used blocks, so it
	// can be traversed
	//========================================================================
	template< typename T, uint32_t SIZE >
	class Pool_FreeList_Traversable {

	public:

		union Block {
			struct Used {
				T t;
				Block * pPrevUsed;
				Block * pNextUsed;
			}used;
			Block * pNextFree;
		};

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Pool_FreeList_Traversable() {

			assert((void*)&m_data[0] == (void*)&m_data[0].used.t);

			m_usedHead.used.pPrevUsed = nullptr;
			m_usedHead.used.pNextUsed = &m_usedTail;
			
			m_usedTail.used.pPrevUsed = &m_usedHead;
			m_usedTail.used.pNextUsed = nullptr;

			//

			Block ** ppB = &m_pCurrentFreeHead;

			for (int it = 0; it < SIZE; ++it) {

				*ppB = &m_data[it];
				ppB = &m_data[it].pNextFree;
			}

			*ppB = nullptr;
		}

		~Pool_FreeList_Traversable() {

			// asserts everything was deallocated
			assert(m_usedHead.used.pNextUsed == &m_usedTail);
			assert(m_usedTail.used.pPrevUsed == &m_usedHead);
		}
		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static uint32_t MaxSize() {

			return SIZE;
		}

		bool IsFull()const {

			return m_pCurrentFreeHead == nullptr;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		T* PoolAlloc() {

			assert(m_pCurrentFreeHead != &m_usedHead);

			//  (inserting new node on double linked list)
			// tail prev next points to new
			m_usedTail.used.pPrevUsed->used.pNextUsed = m_pCurrentFreeHead;
			// new prev points to tail prev
			m_pCurrentFreeHead->used.pPrevUsed = m_usedTail.used.pPrevUsed;
			// new next point to tail
			m_pCurrentFreeHead->used.pNextUsed = &m_usedTail;
			// tail prev points to new
			m_usedTail.used.pPrevUsed = m_pCurrentFreeHead;

			assert(m_usedHead.used.pPrevUsed == nullptr);
			assert(m_usedTail.used.pNextUsed == nullptr);

			//

			T* pNew = &m_pCurrentFreeHead->used.t;

			m_pCurrentFreeHead = m_pCurrentFreeHead->pNextFree;

			return pNew;
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void PoolFree(T* p) {

			assert((void*)p >= (void*)&m_data[0] && (void*)p <= (void*)&m_data[SIZE - 1]); // assert in range
			assert((void*)&m_data[(((uintptr_t)p - (uintptr_t)&m_data[0])) / sizeof(Block)] == (void*)p); // assert is in a correct block

			Block * pFreeAgain = (Block*)p;

			// (removing node from double linked list)

			pFreeAgain->used.pPrevUsed->used.pNextUsed = pFreeAgain->used.pNextUsed;
			pFreeAgain->used.pNextUsed->used.pPrevUsed = pFreeAgain->used.pPrevUsed;

			assert(m_usedHead.used.pPrevUsed == nullptr);
			assert(m_usedTail.used.pNextUsed == nullptr);

			//

			pFreeAgain->pNextFree = m_pCurrentFreeHead;
			m_pCurrentFreeHead = pFreeAgain;
		}

		//not sure if the used are getting all linked correctly
		

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		typename Block::Used * FirstUsed() {

			return &m_usedHead.used.pNextUsed->used;
		}

		typename Block::Used * EndDelimiter() {

			return &m_usedTail.used;
		}

	private:

		Block * m_pCurrentFreeHead;
		Block	m_usedHead;
		Block	m_data[SIZE];
		Block	m_usedTail;
	};

}

//struct structTest {
//	int x;
//	int y;
//};
//
//gen::Pool_FreeList<structTest, 32> poolTest;
//
//structTest* tests[32];
//
//for (int it = 0; it < 32; ++it) {
//
//	tests[it] = poolTest.PoolAlloc();
//}
//
//assert(poolTest.IsFull());
//
//for (int it = 0; it < 32; ++it) {
//
//	poolTest.PoolFree(tests[it]);
//}
//
//for (int it = 0; it < 32; ++it) {
//
//	poolTest.PoolFree(tests[it]);
//}
//structTest * pT1 = poolTest.PoolAlloc();
//pT1->x = 10;
//pT1->y = 15;
//structTest * pT2 = poolTest.PoolAlloc();
//pT2->x = 20;
//pT2->y = 25;
//poolTest.PoolFree(pT1);
//poolTest.PoolFree(pT2);
//pT1 = poolTest.PoolAlloc();






//struct structTest {
//	int x;
//	int y;
//};
//
//gen::Pool_FreeList_Traversable<structTest, 32> poolTest;
//
//structTest* tests[32];
//
//for (int it = 0; it < 32; ++it) {
//
//	tests[it] = poolTest.PoolAlloc();
//
//	tests[it]->x = it;
//	tests[it]->y = 32 - it;
//}
//
//assert(poolTest.IsFull());
//
//gen::Pool_FreeList_Traversable<structTest, 32>::Block::Used * pUsed = poolTest.FirstUsed();
//
//for (int it = 0; pUsed != poolTest.EndDelimiter(); pUsed = &pUsed->pNextUsed->used) {
//
//	pUsed->t.x = it;
//}
//
//for (int it = 20; it < 32; ++it) {
//
//	poolTest.PoolFree(tests[it]);
//}
//
//for (int it = 0; it < 20; ++it) {
//
//	poolTest.PoolFree(tests[it]);
//}
//
//structTest * pT1 = poolTest.PoolAlloc();
//pT1->x = 10;
//pT1->y = 15;
//structTest * pT2 = poolTest.PoolAlloc();
//pT2->x = 20;
//pT2->y = 25;
//poolTest.PoolFree(pT1);
//pT1 = poolTest.PoolAlloc();
//poolTest.PoolFree(pT2);
//poolTest.PoolFree(pT1);
//
//for (int it = 0; it < 15; ++it) {
//
//	tests[it] = poolTest.PoolAlloc();
//
//	tests[it]->x = it;
//	tests[it]->y = 32 - it;
//}
//
//pUsed = poolTest.FirstUsed(); // pointing to delimiter
//
//char stringBuff[32];
//for (int it = 0; pUsed != poolTest.EndDelimiter(); pUsed = &pUsed->pNextUsed->used) {
//
//	pUsed->t.x = it;
//	_itoa_s(pUsed->t.x, stringBuff, 32, 10);
//	OutputDebugString(stringBuff);
//
//	++it;
//}
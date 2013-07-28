#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/06/07
	created:	7:6:2013   1:33
	file:		Event.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	im thinking on letting it act as a c# structure: always passed by copy

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
//#include <windows.h>
//#include <memory>


// private includes

namespace game{

	typedef int EventType;
	static const unsigned int INVALID_EVENTTYPE = (unsigned int)-1;

	template<typename DATA>
	struct Event{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Event( EventType type_p = INVALID_EVENTTYPE, DATA data_p = DATA() )
			:
			m_type(type_p),
			m_data(data_p)
			{}
		
		virtual ~Event(){}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Set( EventType type_p, DATA data_p = 0 ){

			m_type = type_p;
			m_data = data_p;
			/*m_data2 = data2_p;
			m_pData3 = pData3_p;*/
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		EventType GetType() const { return m_type; }

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		template< typename T >
		T GetData() const { return (T)m_data; }
		DATA GetData() const { return m_data; }

		// perhaps its good idea..
		/*template< typename T >
		T GetData2() const { return (T)m_data2; }
		INT_PTR GetData2() const { return m_data2; }*/
		// fuckoff, it will do for now // TODO

		//template< typename T >
		//std::shared_ptr<T> GetData3() const { return (const std::shared_ptr<T>)m_pData3; }


		//std::shared_ptr<void> GetData3() const { return m_pData3; }


	private:

		EventType m_type;
		DATA m_data;

		/*INT_PTR m_data;
		INT_PTR m_data2;
		std::shared_ptr<void> m_pData3;*/
	};

	//typedef std::shared_ptr<Event> shared_Event_ptr;
	//typedef std::weak_ptr<Event> weak_Event_ptr;

	//========================================================================
	// that thing really works
	//========================================================================
	template<int SIZE>
	struct MorpherUnion{ // previously EvilBlob

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
}


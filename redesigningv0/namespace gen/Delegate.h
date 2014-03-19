#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/07/23
	created:	23:7:2013   18:44
	file:		Delegate.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	all delegates here presented use member function ptrs, you cant use it with
				static/normal funcs
				they conform with the standard

	© Icebone1000 (Giuliano Suminsky Pieta), rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// private includes

namespace gen{

	//========================================================================
	// no param delegate
	// returns void
	//========================================================================
	class Delegate{

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Delegate()
			:
		m_pCallerInstance(nullptr),
		m_pFuncMemberFuncInvoker(nullptr){}

		~Delegate(){}

		//------------------------------------------------------------------------
		// Sugar for creating a delegate at once
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)() >
		static Delegate Build( InstanceClass * pInstance_p ){

			Delegate newDelegate;
			newDelegate.Set<InstanceClass, Method>( pInstance_p );

			return newDelegate;
		}

		//------------------------------------------------------------------------
		// holds obj instance and generates static templated invoker func,
		// holding its address on the func ptr.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)() >
		void Set( InstanceClass * pInstance_p ){

			m_pCallerInstance = pInstance_p;
			m_pFuncMemberFuncInvoker = &MemberFuncInvoker<InstanceClass, Method>;
		}

		//------------------------------------------------------------------------
		// test if the delegate is initialized
		//------------------------------------------------------------------------
		operator bool() const{

			return m_pFuncMemberFuncInvoker != nullptr;
		}

		//------------------------------------------------------------------------
		// Delegate callers, explicit and function calling syntax
		//------------------------------------------------------------------------
		void Execute() const{

			(*m_pFuncMemberFuncInvoker)(m_pCallerInstance);
		}
		void operator()() const{

			(*m_pFuncMemberFuncInvoker)(m_pCallerInstance);
		}

	private:

		//------------------------------------------------------------------------
		// this func main objective is have the same signature of the func ptr
		// holded by this class, the second objective is be templated so it
		// can "generate" any func matching the template args given.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)() >
		static void MemberFuncInvoker( void * pInstanceCaller_p ){

			( ((InstanceClass*)pInstanceCaller_p)->*(Method) )();
		}

		void * m_pCallerInstance;
		void (*m_pFuncMemberFuncInvoker)( void * );
	};


	//========================================================================
	// delegate with binds/holds a param at creation, always calling it with
	// that pre bound param..untested
	// returns void
	//========================================================================
	template< typename PARAM >
	class Delegate1BoundParam{
		
	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Delegate1BoundParam()
			:
		m_pCallerInstance(nullptr),
		m_pFuncMemberFuncInvoker(nullptr){}

		~Delegate1BoundParam(){}

		//------------------------------------------------------------------------
		// Sugar for creating a delegate at once
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM) >
		static Delegate1BoundParam Build( InstanceClass * pInstance_p, PARAM & param_p ){

			Delegate1BoundParam newDelegate;
			newDelegate.Set<InstanceClass, Method, param_p>( pInstance_p );

			return newDelegate;
		}

		//------------------------------------------------------------------------
		// holds obj instance and generates static templated invoker func,
		// holding its address on the func ptr.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM) >
		void Set( InstanceClass * pInstance_p, PARAM & param_p ){

			m_pCallerInstance = pInstance_p;
			m_pFuncMemberFuncInvoker = &MemberFuncInvoker<InstanceClass, Method>;
			m_param = param_p;
		}

		//------------------------------------------------------------------------
		// test if the delegate is initialized
		//------------------------------------------------------------------------
		operator bool() const{

			return m_pFuncMemberFuncInvoker != nullptr;
		}

		//------------------------------------------------------------------------
		// Delegate callers, explicit and function calling syntax
		//------------------------------------------------------------------------
		void Execute() const{

			(*m_pFuncMemberFuncInvoker)(m_pCallerInstance);
		}
		void operator()() const{

			(*m_pFuncMemberFuncInvoker)(m_pCallerInstance);
		}

	private:

		//------------------------------------------------------------------------
		// this func main objective is have the same signature of the func ptr
		// holded by this class, the second objective is be templated so it
		// can "generate" any func matching the template args given.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM) >
		static void MemberFuncInvoker( void * pInstanceCaller_p ){

			( ((InstanceClass*)pInstanceCaller_p)->*(Method) )( m_param );
		}

		void * m_pCallerInstance;
		void (*m_pFuncMemberFuncInvoker)( void * );
		PARAM m_param;
	};


	//========================================================================
	// delegate which takes one param from the invoker..untested
	// returns void
	//========================================================================
	template< typename PARAM >	class Delegate1Param;
	template< typename PARAM >	inline bool operator == (const Delegate1Param<PARAM> & lhs, const Delegate1Param<PARAM> & rhs);

	template< typename PARAM >
	class Delegate1Param{

		friend bool operator == <> (const Delegate1Param<PARAM> & lhs, const Delegate1Param<PARAM> & rhs);

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Delegate1Param()
			:
		m_pCallerInstance(nullptr),
		m_pFuncMemberFuncInvoker(nullptr){}

		~Delegate1Param(){}

		//------------------------------------------------------------------------
		// Sugar for creating a delegate at once
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM) >
		static Delegate1Param Build( InstanceClass * pInstance_p ){

			Delegate1Param newDelegate;
			newDelegate.Set<InstanceClass, Method>( pInstance_p );

			return newDelegate;
		}

		//------------------------------------------------------------------------
		// holds obj instance and generates static templated invoker func,
		// holding its address on the func ptr.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM) >
		void Set( InstanceClass * pInstance_p ){

			m_pCallerInstance = pInstance_p;
			m_pFuncMemberFuncInvoker = &MemberFuncInvoker<InstanceClass, Method>;
		}

		//------------------------------------------------------------------------
		// test if the delegate is initialized
		//------------------------------------------------------------------------
		operator bool() const{

			return m_pFuncMemberFuncInvoker != nullptr;
		}

		//------------------------------------------------------------------------
		// Delegate callers, explicit and function calling syntax
		//------------------------------------------------------------------------
		void Execute( PARAM param_p ) const{

			(*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p );
		}
		void operator()( PARAM param_p ) const{

			(*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p );
		}

	private:

		//------------------------------------------------------------------------
		// this func main objective is have the same signature of the func ptr
		// holded by this class, the second objective is be templated so it
		// can "generate" any func matching the template args given.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM) >
		static void MemberFuncInvoker( void * pInstanceCaller_p, PARAM param_p ){

			( ((InstanceClass*)pInstanceCaller_p)->*(Method) )( param_p );
		}

		void * m_pCallerInstance;
		void (*m_pFuncMemberFuncInvoker)( void*, PARAM );
	};
	
	//------------------------------------------------------------------------
	// ==
	//------------------------------------------------------------------------
	template< typename PARAM >
	bool operator == (const Delegate1Param<PARAM> & lhs, const Delegate1Param<PARAM> & rhs){

		return ( lhs.m_pFuncMemberFuncInvoker == rhs.m_pFuncMemberFuncInvoker
					&&
				lhs.m_pCallerInstance == rhs.m_pCallerInstance );
	}


	//========================================================================
	// delegate which takes 2 params from the invoker..untested
	// returns void
	//========================================================================
	template< typename PARAM, typename PARAM2 >	class Delegate2Param;
	template< typename PARAM, typename PARAM2 >	inline bool operator ==
	 (const Delegate2Param< PARAM, PARAM2 > & lhs, const Delegate2Param< PARAM, PARAM2 > & rhs);

	template< typename PARAM, typename PARAM2 >
	class Delegate2Param{

		friend bool operator == <> (const Delegate2Param<PARAM, PARAM2> & lhs, const Delegate2Param<PARAM, PARAM2> & rhs);

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Delegate2Param()
			:
		m_pCallerInstance(nullptr),
		m_pFuncMemberFuncInvoker(nullptr){}

		~Delegate2Param(){}

		//------------------------------------------------------------------------
		// Sugar for creating a delegate at once
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM, PARAM2) >
		static Delegate2Param Build( InstanceClass * pInstance_p ){

			Delegate2Param newDelegate;
			newDelegate.Set<InstanceClass, Method>( pInstance_p );

			return newDelegate;
		}

		//------------------------------------------------------------------------
		// holds obj instance and generates static templated invoker func,
		// holding its address on the func ptr.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM, PARAM2) >
		void Set( InstanceClass * pInstance_p ){

			m_pCallerInstance = pInstance_p;
			m_pFuncMemberFuncInvoker = &MemberFuncInvoker<InstanceClass, Method>;
		}

		//------------------------------------------------------------------------
		// test if the delegate is initialized
		//------------------------------------------------------------------------
		operator bool() const{

			return m_pFuncMemberFuncInvoker != nullptr;
		}

		//------------------------------------------------------------------------
		// Delegate callers, explicit and function calling syntax
		//------------------------------------------------------------------------
		void Execute( PARAM param_p, PARAM2 param2_p ) const{

			(*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p, param2_p );
		}
		void operator()( PARAM param_p, PARAM2 param2_p ) const{

			(*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p, param2_p );
		}

	private:

		//------------------------------------------------------------------------
		// this func main objective is have the same signature of the func ptr
		// holded by this class, the second objective is be templated so it
		// can "generate" any func matching the template args given.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM, PARAM2) >
		static void MemberFuncInvoker( void * pInstanceCaller_p, PARAM param_p, PARAM2 param2_p ){

			( ((InstanceClass*)pInstanceCaller_p)->*(Method) )( param_p, param2_p );
		}

		void * m_pCallerInstance;
		void (*m_pFuncMemberFuncInvoker)( void*, PARAM, PARAM2 );
	};

	//------------------------------------------------------------------------
	// ==
	//------------------------------------------------------------------------
	template< typename PARAM, typename PARAM2 >
	bool operator == (const Delegate2Param<PARAM, PARAM2> & lhs, const Delegate2Param<PARAM, PARAM2> & rhs){

		return ( lhs.m_pFuncMemberFuncInvoker == rhs.m_pFuncMemberFuncInvoker
					&&
				lhs.m_pCallerInstance == rhs.m_pCallerInstance );
	}

	//========================================================================
	// delegate which takes 2 params from the invoker..untested
	// returns void
	//========================================================================
	template< typename PARAM, typename PARAM2, typename PARAM3 >	class Delegate3Param;
	template< typename PARAM, typename PARAM2, typename PARAM3 >	inline bool operator ==
		(const Delegate3Param< PARAM, PARAM2, PARAM3 > & lhs, const Delegate3Param< PARAM, PARAM2, PARAM3 > & rhs);

	template< typename PARAM, typename PARAM2, typename PARAM3 >
	class Delegate3Param{

		friend bool operator == <> (const Delegate3Param<PARAM, PARAM2, PARAM3> & lhs, const Delegate3Param<PARAM, PARAM2, PARAM3> & rhs);

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Delegate3Param()
			:
		m_pCallerInstance(nullptr),
			m_pFuncMemberFuncInvoker(nullptr){}

		~Delegate3Param(){}

		//------------------------------------------------------------------------
		// Sugar for creating a delegate at once
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM, PARAM2, PARAM3) >
		static Delegate3Param Build( InstanceClass * pInstance_p ){

			Delegate3Param newDelegate;
			newDelegate.Set<InstanceClass, Method>( pInstance_p );

			return newDelegate;
		}

		//------------------------------------------------------------------------
		// holds obj instance and generates static templated invoker func,
		// holding its address on the func ptr.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM, PARAM2, PARAM3) >
		void Set( InstanceClass * pInstance_p ){

			m_pCallerInstance = pInstance_p;
			m_pFuncMemberFuncInvoker = &MemberFuncInvoker<InstanceClass, Method>;
		}

		//------------------------------------------------------------------------
		// test if the delegate is initialized
		//------------------------------------------------------------------------
		operator bool() const{

			return m_pFuncMemberFuncInvoker != nullptr;
		}

		//------------------------------------------------------------------------
		// Delegate callers, explicit and function calling syntax
		//------------------------------------------------------------------------
		void Execute( PARAM param_p, PARAM2 param2_p, PARAM3 param3_p ) const{

			(*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p, param2_p, param3_p );
		}
		void operator()( PARAM param_p, PARAM2 param2_p, PARAM3 param3_p ) const{

			(*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p, param2_p, param3_p );
		}

	private:

		//------------------------------------------------------------------------
		// this func main objective is have the same signature of the func ptr
		// holded by this class, the second objective is be templated so it
		// can "generate" any func matching the template args given.
		//------------------------------------------------------------------------
		template< class InstanceClass, void (InstanceClass::*Method)(PARAM, PARAM2, PARAM3) >
		static void MemberFuncInvoker( void * pInstanceCaller_p, PARAM param_p, PARAM2 param2_p, PARAM3 param3_p ){

			( ((InstanceClass*)pInstanceCaller_p)->*(Method) )( param_p, param2_p, param3_p );
		}

		void * m_pCallerInstance;
		void (*m_pFuncMemberFuncInvoker)( void*, PARAM, PARAM2, PARAM3 );
	};

	//------------------------------------------------------------------------
	// ==
	//------------------------------------------------------------------------
	template< typename PARAM, typename PARAM2, typename PARAM3 >
	bool operator == (const Delegate3Param<PARAM, PARAM2, PARAM3> & lhs, const Delegate3Param<PARAM, PARAM2, PARAM3> & rhs){

		return ( lhs.m_pFuncMemberFuncInvoker == rhs.m_pFuncMemberFuncInvoker
			&&
			lhs.m_pCallerInstance == rhs.m_pCallerInstance );
	}

	//========================================================================
	// delegate which takes 2 params from the invoker..untested
	// returns bool
	//========================================================================
	template< typename RETURN, typename PARAM >	class DelegateR1Param;
	template< typename RETURN, typename PARAM >	inline bool operator ==
	 (const DelegateR1Param< RETURN, PARAM > & lhs, const DelegateR1Param< RETURN, PARAM > & rhs);

	template< typename RETURN, typename PARAM >
	class DelegateR1Param{

		friend bool operator == <> (const DelegateR1Param<RETURN, PARAM> & lhs, const DelegateR1Param<RETURN, PARAM> & rhs);

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		DelegateR1Param()
			:
		m_pCallerInstance(nullptr),
			m_pFuncMemberFuncInvoker(nullptr){}

		~DelegateR1Param(){}

		//------------------------------------------------------------------------
		// Sugar for creating a delegate at once
		//------------------------------------------------------------------------
		template< class InstanceClass, RETURN (InstanceClass::*Method)(PARAM) >
		static DelegateR1Param Build( InstanceClass * pInstance_p ){

			DelegateR2Param newDelegate;
			newDelegate.Set<InstanceClass, Method>( pInstance_p );

			return newDelegate;
		}

		//------------------------------------------------------------------------
		// holds obj instance and generates static templated invoker func,
		// holding its address on the func ptr.
		//------------------------------------------------------------------------
		template< class InstanceClass, RETURN (InstanceClass::*Method)(PARAM) >
		void Set( InstanceClass * pInstance_p ){

			m_pCallerInstance = pInstance_p;
			m_pFuncMemberFuncInvoker = &MemberFuncInvoker<InstanceClass, Method>;
		}

		//------------------------------------------------------------------------
		// test if the delegate is initialized
		//------------------------------------------------------------------------
		operator bool() const{

			return m_pFuncMemberFuncInvoker != nullptr;
		}

		//------------------------------------------------------------------------
		// Delegate callers, explicit and function calling syntax
		//------------------------------------------------------------------------
		RETURN Execute( PARAM param_p ) const{

			return (*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p );
		}
		RETURN operator()( PARAM param_p ) const{

			return (*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p );
		}

	private:

		//------------------------------------------------------------------------
		// this func main objective is have the same signature of the func ptr
		// holded by this class, the second objective is be templated so it
		// can "generate" any func matching the template args given.
		//------------------------------------------------------------------------
		template< class InstanceClass, RETURN (InstanceClass::*Method)(PARAM) >
		static RETURN MemberFuncInvoker( void * pInstanceCaller_p, PARAM param_p ){

			return ( ((InstanceClass*)pInstanceCaller_p)->*(Method) )( param_p );
		}

		void * m_pCallerInstance;
		RETURN (*m_pFuncMemberFuncInvoker)( void*, PARAM );
	};

	//------------------------------------------------------------------------
	// ==
	//------------------------------------------------------------------------
	template< typename RETURN, typename PARAM >
	bool operator == (const DelegateR1Param<RETURN, PARAM> & lhs, const DelegateR1Param<RETURN, PARAM> & rhs){

		return ( lhs.m_pFuncMemberFuncInvoker == rhs.m_pFuncMemberFuncInvoker
					&&
				lhs.m_pCallerInstance == rhs.m_pCallerInstance );
	}

	//========================================================================
	// delegate which takes 2 params from the invoker..untested
	// returns bool
	//========================================================================
	template< typename RETURN, typename PARAM, typename PARAM2 >	class DelegateR2Param;
	template< typename RETURN, typename PARAM, typename PARAM2 >	inline bool operator ==
		(const DelegateR2Param< RETURN, PARAM, PARAM2 > & lhs, const DelegateR2Param< RETURN, PARAM, PARAM2 > & rhs);

	template< typename RETURN, typename PARAM, typename PARAM2 >
	class DelegateR2Param{

		friend bool operator == <> (const DelegateR2Param<RETURN, PARAM, PARAM2> & lhs, const DelegateR2Param<RETURN, PARAM, PARAM2> & rhs);

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		DelegateR2Param()
			:
		m_pCallerInstance(nullptr),
			m_pFuncMemberFuncInvoker(nullptr){}

		~DelegateR2Param(){}

		//------------------------------------------------------------------------
		// Sugar for creating a delegate at once
		//------------------------------------------------------------------------
		template< class InstanceClass, RETURN (InstanceClass::*Method)(PARAM, PARAM2) >
		static DelegateR2Param Build( InstanceClass * pInstance_p ){

			DelegateR2Param newDelegate;
			newDelegate.Set<InstanceClass, Method>( pInstance_p );

			return newDelegate;
		}

		//------------------------------------------------------------------------
		// holds obj instance and generates static templated invoker func,
		// holding its address on the func ptr.
		//------------------------------------------------------------------------
		template< class InstanceClass, RETURN (InstanceClass::*Method)(PARAM, PARAM2) >
		void Set( InstanceClass * pInstance_p ){

			m_pCallerInstance = pInstance_p;
			m_pFuncMemberFuncInvoker = &MemberFuncInvoker<InstanceClass, Method>;
		}

		//------------------------------------------------------------------------
		// test if the delegate is initialized
		//------------------------------------------------------------------------
		operator bool() const{

			return m_pFuncMemberFuncInvoker != nullptr;
		}

		//------------------------------------------------------------------------
		// Delegate callers, explicit and function calling syntax
		//------------------------------------------------------------------------
		RETURN Execute( PARAM param_p, PARAM2 param2_p ) const{

			return (*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p, param2_p );
		}
		RETURN operator()( PARAM param_p, PARAM2 param2_p ) const{

			return (*m_pFuncMemberFuncInvoker)( m_pCallerInstance, param_p, param2_p );
		}

	private:

		//------------------------------------------------------------------------
		// this func main objective is have the same signature of the func ptr
		// holded by this class, the second objective is be templated so it
		// can "generate" any func matching the template args given.
		//------------------------------------------------------------------------
		template< class InstanceClass, RETURN (InstanceClass::*Method)(PARAM, PARAM2) >
		static RETURN MemberFuncInvoker( void * pInstanceCaller_p, PARAM param_p, PARAM2 param2_p ){

			return ( ((InstanceClass*)pInstanceCaller_p)->*(Method) )( param_p, param2_p );
		}

		void * m_pCallerInstance;
		RETURN (*m_pFuncMemberFuncInvoker)( void*, PARAM, PARAM2 );
	};

	//------------------------------------------------------------------------
	// ==
	//------------------------------------------------------------------------
	template< typename RETURN, typename PARAM, typename PARAM2 >
	bool operator == (const DelegateR2Param<RETURN, PARAM, PARAM2> & lhs, const DelegateR2Param<RETURN, PARAM, PARAM2> & rhs){

		return ( lhs.m_pFuncMemberFuncInvoker == rhs.m_pFuncMemberFuncInvoker
					&&
				lhs.m_pCallerInstance == rhs.m_pCallerInstance );
	}
}

/*
 //------------------------------------------------------------------------
 // testing delegate trough a interface class DelegateProduct, where classes must derive from
 // DelegateProduct so that a delegate caller can convert member functions to DelegateProduct and invoke them.
 // Second stackoverflow, you can convert to base class..
 //------------------------------------------------------------------------
 class DelegateProduct{

 public:
 virtual ~DelegateProduct(){};
 };

 class DelegateConsumer{
 public:

 DelegateProduct * m_pCaller;
 void (DelegateProduct::*m_ICallback) ();

 void Execute(){

 ( (m_pCaller)->*(m_ICallback) )();
 }
 };

 class DelegateProductImplementer : public DelegateProduct{

 int m_integer;

 public:

 void Method(){
 //BREAKHERE;
 m_integer += 3;
 }

 int GetInteger() const {
 return m_integer;
 }
 };

 //------------------------------------------------------------------------
 // this test tries to avoid depending on know type trough a templated static
 // func. This is done trough a class who holds the object as a void * and
 // a normal func ptr that will actually contain the desired member func ptr
 // address (trough template arg) and will know how to convert from void* to
 // desired class type(also trough template arg). So the class job is resumed
 // to call the member func passing the void ptr containing the object.
 //------------------------------------------------------------------------
 class Delegate{

 void* m_pObjectCaller;
 void (*m_pFunc)(void* pCaller_p);

 public:

 template< class Caller, void (Caller::*CallerMethod)() >
 static void DelegateInvoker( void * pCaller ){

 ( ( ((Caller*)pCaller))->*(CallerMethod) )();
 }

 template< class Caller, void (Caller::*CallerMethod)() >
 void Set( Caller * pObj_p ){

 m_pObjectCaller = pObj_p;
 m_pFunc = &DelegateInvoker<Caller, CallerMethod>;
 }

 void Execute(){

 (*m_pFunc)(m_pObjectCaller);
 }
 };



 //////////////////////////////////////////////////////////////////////////

 DelegateConsumer myA;
 DelegateProductImplementer myImplementer;

 myA.m_pCaller = &myImplementer;
 myA.m_ICallback = (void (DelegateProduct::*)() )&DelegateProductImplementer::Method; // cast implementer to base

 //( (myA.m_pCaller)->*(myA.m_ICallback) )();
 myA.Execute();

 //

 Delegate myDeleg;
 myDeleg.Set<DelegateProductImplementer, &DelegateProductImplementer::Method>(&myImplementer);
 myDeleg.Execute();


//////////////////////////////////////////////////////////////////////////

DOUBLE dDurationA = 0.0;
DOUBLE dDurationB = 0.0;
mentle.GetTimer().m_cronometer.StartCount();

for( int it = 0; it < 1000000; ++it ){

myA.Execute();
//myDeleg.Execute();
}

mentle.GetTimer().m_cronometer.EndCountMilliseconds(dDurationA);

mentle.GetTimer().m_cronometer.StartCount();

for( int it = 0; it < 1000000; ++it ){

myDeleg.Execute();
//myA.Execute();
}

mentle.GetTimer().m_cronometer.EndCountMilliseconds(dDurationB);

win::UniqueFileLogger()<<"DELEGATETIMES toBase conv: "<<dDurationA<<SZ_NEWLINE;
win::UniqueFileLogger()<<"DELEGATETIMES static templated invoker: "<<dDurationB<<SZ_NEWLINE;
win::UniqueFileLogger()<<myImplementer.GetInteger()<<SZ_NEWLINE;

// RESULTS: in debug and release under VS the Inherit one is a bit faster. who cares
// in release outside VS the static templated is the faster

 */


//========================================================================
// member func pointer syntax example to remember..
//========================================================================
//private:
//
//	Color3 (RayTracer::*m_rayTracedColorFunction)(int, int);					// "delegate" that computes the traced color
//	Color3 (RayTracer::*m_illuminationMethodFunction)(int, const HitData &);	// "delegate" that computes color at hit point
//
//	m_rayTracedColorFunction = &RayTracer::TraceCloserRayHit;					// assignment
//	m_illuminationMethodFunction = &RayTracer::PhongAtIntersectionPoint;
//
// Color3 c = (this->*m_rayTracedColorFunction)(x, y);							// use
// returnColor = ((this->*m_illuminationMethodFunction)(itCandidateShape, candidatehit));
// 
// typedefing function/member function pointer:
// 
// int (*FuncPtr)(int); // simple func pointer declaration, FuncPtr is a variable
// typedef int (*FuncPtr)(int); // FuncPtr now a type
// typedef int (MyClass::*MyTypedef)( int); // MyTypedef is a type
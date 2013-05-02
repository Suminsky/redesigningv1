//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/06
	created:	6:2:2013   0:15
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace gen\gen_data.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace gen
	file base:	gen_data
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

namespace gen{

	//========================================================================
	// used initially for boost shared_ptr deleter, when using shared pointer 
	// with stack allocated data
	//========================================================================
	template <class T>
	static void NoOp(T* obj)
	{
		obj = obj;
		(void)0;
	}

	//========================================================================
	// 
	//========================================================================
	enum EHORZALIGN{

		EHORZALIGN_LEFT,
		EHORZALIGN_RIGHT,
		EHORZALIGN_CENTER
	};

	enum EVERTALIGN{

		EVERTALIGN_TOP,
		EVERTALIGN_BOTTOM,
		EVERTALIGN_CENTER
	};


	//========================================================================
	// Nine point anchor structure
	// \ ^ /
	// - o -
	// / v \
	//========================================================================
	struct Anchor{

		EHORZALIGN h;
		EVERTALIGN v;

		Anchor( EHORZALIGN h_p, EVERTALIGN v_p ): h(h_p), v(v_p){}

		// facility

		inline Anchor& Centralize(){

			h = EHORZALIGN_CENTER;
			v = EVERTALIGN_CENTER;

			return *this;
		}
	};
}
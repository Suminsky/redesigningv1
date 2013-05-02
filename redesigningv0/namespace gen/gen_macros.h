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
#define BREAKHERE {int breakhere = 7; breakhere = breakhere;}
#define NDBG(param)
#else
#define COMMA
#define DBG(param)
#define BREAKHERE
#define NDBG(param) param
#endif

#ifdef IDIOTSPROOFOFF
#define IDIOTSPROOF(p)
#else
#define IDIOTSPROOF(p) p
#endif
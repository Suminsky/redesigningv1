#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/08/17
	created:	17:8:2013   22:57
	file:		Parser.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <string>
#include <vector>

// private includes
#include "../namespace gen/gen_data.h"

namespace text{

	//========================================================================
	// 
	//========================================================================
	class Parser{

	public:
		

	//private:

		//------------------------------------------------------------------------
		// use this to remove sections between open and close
		// i.e. remove comments
		// cOpen = #
		// cClose = \n
		// open and close are also removed
		// 
		// made to support quotes, so open close inside "  "
		// are ignored.
		//------------------------------------------------------------------------
		static void RemoveSectionsBetween( gen::DataStream & buffer_p, char cOpen_p, char cClose_p );
		static void RemoveUnquotedSectionsBetween( gen::DataStream & buffer_p, char cOpen_p, char cClose_p, char cQuote = '"' );
		
		//------------------------------------------------------------------------
		// use this to replace all chars (szChars) on buffer by cReplacement
		//------------------------------------------------------------------------
		static void ReplaceCharsBy( const char * szChars_p, char cReplacement_p,  gen::DataStream & buffer_p   );

		//------------------------------------------------------------------------
		// use this to remove the chars from the stream.
		// the stream will be mem moved to trim the removed chars.
		// the m_size of the stream will also be updated, though no reallocation
		// happens
		//------------------------------------------------------------------------
		static void RemoveChars( const char * szToRemove_p, gen::DataStream & buffer_p );

		//------------------------------------------------------------------------
		// use this to get data section discarding all invalids on the start
		// and all invalids from the end.
		// i.e. use this to remove spaces tabs and new lines, but only from
		// the tips (start and end), letting the whole central part trimmed:
		// before:
		// \n im going \n \t to home \t \n
		// after:
		// im going \n \t to home
		// 
		// note:
		// if you want to have new lines or other generally invalid data
		// at the tips, put it between quotes, than remove invalids, than
		// remove quotes:
		// 
		// \n\t " \n\t going home happy \t\t\n " \t \t \n
		// "\n\t going home happy \t\t\n "
		// \n\t going home happy \t\t\n 
		//------------------------------------------------------------------------
		static void GetSectionBetweenInvalids( const char * szInvalids_p, gen::DataStream & buffer_p, int & iValidStartPos, unsigned int & nValidPortion_p );

		//------------------------------------------------------------------------
		// samething as above, but have only one invalid
		//------------------------------------------------------------------------
		static void GetSectionBetweenDelimiter( const char cDelimiter_p, gen::DataStream & buffer_p, int & iValidStartPos, unsigned int & nValidPortion_p );
	};

	//========================================================================
	// # gfig  basic:
	// # g config text file
	// # comments works like //, but use # instead
	// #	[a] -> element a
	// #	[a [b]] -> element a have element b
	// #	[a [b][c]] -> element a have b and c
	// #	[a = 2 [b][c = 6] ] -> element a (value 2) have b (no value) and c(value6)
	// #
	// # Element names and values have spaces removed from begin and end (not inbetween) 
	// # To keep " \t\r\n" at the ends of a name or value, use quotes " " at the ends, quotes
	// # will be removed, and everything in between is preserved.
	// # Note that you do can use quotes inside quotes, since it gets all data after first quote
	// # and before last one.
	// #
	// # Recommended use Notepad++ for editing, setting Ruby/Python/VB as language
	//========================================================================
	struct GfigElement{

		std::string m_name;
		std::string m_value;
		int m_indexOnParent;
		std::vector<GfigElement> m_subElements;

		bool GetSubElement( const char * szName_p, GfigElement *& pElement_p );
		bool GetSubElement( std::string szName_p, GfigElement *& pElement_p  );


		//------------------------------------------------------------------------
		// remove comments, than recursively parses using below function
		//------------------------------------------------------------------------
		static void ParseGfigFile( gen::DataStream & buffer_p, GfigElement & parsed_p );

	private:

		//------------------------------------------------------------------------
		// the stream given must have comments already removed, use the other overload
		// to do it
		//------------------------------------------------------------------------
		static void ParseCleanGfigFile( gen::DataStream & buffer, GfigElement & parsed_p /*, int nOpenBracketsAbove = 0*/ );

		//------------------------------------------------------------------------
		// removes spaces from ends, remove quotes if any
		//------------------------------------------------------------------------
		static void ParseAssign( gen::DataStream & buffer_p, std::string & szOut_p );
	};
}
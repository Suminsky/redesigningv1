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
		// quotes inside quotes must be protected by \
		//------------------------------------------------------------------------
		static void RemoveSectionsBetween( gen::DataStream & buffer_p, char cOpen_p, char cClose_p );
		static void RemoveSectionsBetweenW( gen::DataStreamW & buffer_p, wchar_t cOpen_p, wchar_t cClose_p );
		static void RemoveUnquotedSectionsBetween( gen::DataStream & buffer_p, char cOpen_p, char cClose_p, char cQuote = '"', char cQuoteShield = '\\' );
		static void RemoveUnquotedSectionsBetweenW( gen::DataStreamW & buffer_p, wchar_t cOpen_p, wchar_t cClose_p, wchar_t cQuote = L'"', wchar_t cQuoteShield = L'\\' );

		//------------------------------------------------------------------------
		// same thing as above, but instead of removing returns the section pos info
		//------------------------------------------------------------------------
		static void GetSectionBetween( gen::DataStream & buffer_p, const char cOpen_p, const char cClose_p, int & iValidStartPos, unsigned int & nValidPortion_p );
		static void GetSectionBetweenW( gen::DataStreamW & buffer_p, const wchar_t cOpen_p, const wchar_t cClose_p, int & iValidStartPos, unsigned int & nValidPortion_p );

		//------------------------------------------------------------------------
		// detects real new lines (\n) shieldened by \, than finds next \ and
		// replace from \ to \ to a single new line.
		// this is because text files have formatting spaces tabs, and when a new
		// line happens all those are included, using the \ to shield one can
		// indicate the valid portion.
		// example:
		//	desc = "line 1
		//			line 2"
		//	# in this sample, theres 2 tabs after new line
		//	
		//	desc = "line 1
		//	line 2"
		//	# in this sample, its ok, but the formatting is gone
		//	
		//	desc = "line1\
		//			\line2"
		//	# in this sample, problem is fixed.
		//------------------------------------------------------------------------
		static void AdjustQuotedNewLines( gen::DataStream & buffer_p, const char cForNewLineOpen_p = '\\', const char cForNewLineClose_p = '\\', const char cReplacement_p = '\n', const char cNewLine = '\r' );
		static void AdjustQuotedNewLinesW( gen::DataStreamW & buffer_p, const wchar_t cForNewLineOpen_p = L'\\', const wchar_t cForNewLineClose_p = L'\\', const wchar_t cReplacement_p = L'\n', const wchar_t cNewLine = L'\r' );

		//------------------------------------------------------------------------
		// use this to replace all chars (szChars) on buffer by cReplacement
		//------------------------------------------------------------------------
		static void ReplaceCharsBy( const char * szChars_p, const char cReplacement_p,  gen::DataStream & buffer_p   );
		static void ReplaceCharsByW( const wchar_t * szChars_p, const wchar_t cReplacement_p,  gen::DataStreamW & buffer_p   );

		//------------------------------------------------------------------------
		// use this to remove the chars from the stream.
		// the stream will be mem moved to trim the removed chars.
		// the m_size of the stream will also be updated, though no reallocation
		// happens
		//------------------------------------------------------------------------
		static void RemoveChars( const char * szToRemove_p, gen::DataStream & buffer_p );
		static void RemoveCharsW( const wchar_t * szToRemove_p, gen::DataStreamW & buffer_p );

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
		static void GetSectionBetweenInvalidsOnTips( const char * szInvalids_p, gen::DataStream & buffer_p, int & iValidStartPos, unsigned int & nValidPortion_p );
		static void GetSectionBetweenInvalidsOnTipsW( const wchar_t * szInvalids_p, gen::DataStreamW & buffer_p, int & iValidStartPos, unsigned int & nValidPortion_p );

		inline static bool NeedQuotes( const char cFirst_p, const char cLast_p, const char * szInvalids_p );

		
		
		//------------------------------------------------------------------------
		// an entire section replaced by a char
		// build to replace \       \  by a single new line char.
		// \ is used on text file languages to link/continue a line on another
		// without breaking quotes
		//------------------------------------------------------------------------
		static void ReplaceSectionBy( const char cDelimiter_p, gen::DataStream & buffer_p, const char cReplacement_p );
		static void ReplaceSectionByW( const wchar_t cDelimiter_p, gen::DataStreamW & buffer_p, const wchar_t cReplacement_p );
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
	struct GfigElementA{

		std::string m_name;
		std::string m_value;
		int m_indexOnParent;
		std::vector<GfigElementA> m_subElements;

		GfigElementA(){}
		GfigElementA( const std::string && szName_p) : m_name( std::move(szName_p) ){}
		GfigElementA( const std::string && szName_p, const std::string && szValue_p)
			: m_name( std::move(szName_p) ), m_value( std::move(szValue_p) ){}
		GfigElementA( const char * szName_p ) : m_name( szName_p ){}
		GfigElementA( const char * szName_p, const char * szValue_p ) : m_name( szName_p ), m_value( szValue_p ){}

		bool GetSubElement( const char * szName_p, GfigElementA *& pElement_p );
		bool GetSubElement( std::string szName_p, GfigElementA *& pElement_p  );

		static bool FindFirstElementBFS( const char * szName_p, GfigElementA *& pElement_p );
		static bool FindLastElementDFS( const char * szName_p, GfigElementA *& pElement_p );
		static bool FindFirstElementDFS( const char * szName_p, GfigElementA *& pElement_p );


		//------------------------------------------------------------------------
		// remove comments, than recursively parses using below function
		//------------------------------------------------------------------------
		static void ParseGfigFile( gen::DataStream & buffer_p, GfigElementA & parsed_p );

		//------------------------------------------------------------------------
		// expand gfig into a buffer that can be parsed by ParseGfigFile
		// invalidates buffer_p, cause it may(mostly will) reallocate it
		//------------------------------------------------------------------------
		static void SaveGfigToFile( const GfigElementA & gfig_p, std::string & header_p, gen::DataStream & buffer_p, const char cNewLine = '\n', const char cTab = '\t' );
		
		

	private:

		//------------------------------------------------------------------------
		// the stream given must have comments already removed, use the other overload
		// to do it
		//------------------------------------------------------------------------
		static void ParseCleanGfigFile( gen::DataStream & buffer, GfigElementA & parsed_p );

		static void SaveGfigToFile( const GfigElementA & gfig_p, std::string & formated_p, int currentTabIndentation_p = 0, const char cNewLine = '\n', const char cTab = '\t' );

		//------------------------------------------------------------------------
		// removes spaces from ends, remove quotes if any
		//------------------------------------------------------------------------
		static void ParseAssign( gen::DataStream & buffer_p, std::string & szOut_p );

		//------------------------------------------------------------------------
		// ads quotes and '\' pairs for new lines,
		// putting the current number of tabs inbetween for better formatting on the file
		//------------------------------------------------------------------------
		static void AdjustNewLineTabFormatting(
			const std::string & szIn_p, std::string & formated_p,
			int currentTabIndentation_p,
			const char cTab = '\t', const char cNewLine = '\n');
	};

	//========================================================================
	// 
	//========================================================================
	struct GfigElementW{

		std::wstring m_name;
		std::wstring m_value;
		int m_indexOnParent;
		std::vector<GfigElementW> m_subElements;

		GfigElementW(){}
		GfigElementW( const wchar_t * szName_p ) : m_name( szName_p ){}
		GfigElementW( const wchar_t * szName_p, const wchar_t * szValue_p ) : m_name( szName_p ), m_value( szValue_p ){}

		bool GetSubElement( const wchar_t * szName_p, GfigElementW *& pElement_p );
		bool GetSubElement( std::wstring szName_p, GfigElementW *& pElement_p  );


		//------------------------------------------------------------------------
		// remove comments, than recursively parses using below function
		//------------------------------------------------------------------------
		static void ParseGfigFile( gen::DataStreamW & buffer_p, GfigElementW & parsed_p );

	private:

		//------------------------------------------------------------------------
		// the stream given must have comments already removed, use the other overload
		// to do it
		//------------------------------------------------------------------------
		static void ParseCleanGfigFile( gen::DataStreamW & buffer, GfigElementW & parsed_p );

		//------------------------------------------------------------------------
		// removes spaces from ends, remove quotes if any
		//------------------------------------------------------------------------
		static void ParseAssign( gen::DataStreamW & buffer_p, std::wstring & szOut_p );
	};
}
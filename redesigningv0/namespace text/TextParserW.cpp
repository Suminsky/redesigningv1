#include "PCHF.h"
#include "TextParser.h"

#include "TextParser.h"

#include <memory>
#include <string>
#include <vector>

#include "../namespace gen/gen_macros.h"

using namespace text;
using namespace std;
using namespace gen;

void Parser::RemoveSectionsBetweenW( DataStreamW & buffer_p, wchar_t cOpen_p, wchar_t cClose_p )
{

	unsigned int iByteIndex = 0;
	int iCommentStartPos = -1;

	while( iByteIndex < buffer_p.m_size ){

		// if still not found any comment

		if( iCommentStartPos == -1 ){

			if( buffer_p.m_data[iByteIndex] == cOpen_p){

				// found comment start

				iCommentStartPos = iByteIndex;
			}
		}
		else{

			// if looking for the end of the comment

			if( buffer_p.m_data[iByteIndex] == cClose_p){

				int iCommentEndPos = iByteIndex;

				// from after comment to EOF, copy it over the start of the comment

				int iSizeAfterComment =  buffer_p.m_size - iCommentEndPos;

				memcpy(		&buffer_p.m_data[iCommentStartPos],
					&buffer_p.m_data[iCommentEndPos+1],
					iSizeAfterComment	);

				// "trim" data size

				int iCommentSize = (iCommentEndPos+1) - iCommentStartPos;
				buffer_p.m_size -= iCommentSize;

				// account current pos to the choped part

				iByteIndex = iByteIndex - iCommentSize; // iByteIndex += 1; -> on \r, not \n

				// start looking for comments again

				iCommentStartPos = -1;
			}
		}


		++iByteIndex;
	}

	if( iCommentStartPos != -1 ){

		// chop off the comment at the EOF

		buffer_p.m_size -= (buffer_p.m_size - iCommentStartPos);
	}

	//ReplaceCharsBy( "\n", '!', buffer_p );
	//ReplaceCharsBy( "\r", '?', buffer_p );

	return;
}
void Parser::RemoveUnquotedSectionsBetweenW( DataStreamW & buffer_p, wchar_t cOpen_p, wchar_t cClose_p, wchar_t cQuote, wchar_t cQuoteShield )
{
	unsigned int iByteIndex = 0;
	int iCommentStartPos = -1;
	bool bIgnoringDueQuote = false;

	while( iByteIndex < buffer_p.m_size ){

		if( !bIgnoringDueQuote ){			

			// if still not found any comment

			if( iCommentStartPos == -1 ){

				// first check for quote before comment

				if( buffer_p.m_data[iByteIndex] == cQuote ){

					bIgnoringDueQuote = true;
				}

				else if( buffer_p.m_data[iByteIndex] == cOpen_p){

					// found comment start

					iCommentStartPos = iByteIndex;
				}
			}
			else{

				// if looking for the end of the comment

				if( buffer_p.m_data[iByteIndex] == cClose_p){

					int iCommentEndPos = iByteIndex;

					// from after comment to EOF, copy it over the start of the comment

					int iSizeAfterComment =  buffer_p.m_size - iCommentEndPos;

					memcpy(		&buffer_p.m_data[iCommentStartPos],
						&buffer_p.m_data[iCommentEndPos+1],
						iSizeAfterComment	);

					// "trim" data size

					int iCommentSize = (iCommentEndPos+1) - iCommentStartPos;
					buffer_p.m_size -= iCommentSize;

					// account current pos to the choped part

					iByteIndex = iByteIndex - iCommentSize; // iByteIndex += 1; -> on \r, not \n

					// start looking for comments again

					iCommentStartPos = -1;
				}
			}

		}
		else{

			if( buffer_p.m_data[iByteIndex] == cQuote ){

				// check if quote inside quote

				if( buffer_p.m_data[iByteIndex-1] == cQuoteShield ){// \

					// NOTE: cant remove slash here, it is still used

					// than ignore this quote

					++iByteIndex;
					continue;
				}

				bIgnoringDueQuote = false;
			}
		}


		++iByteIndex;
	}

	if( iCommentStartPos != -1 ){

		// chop off the comment at the EOF

		buffer_p.m_size -= (buffer_p.m_size - iCommentStartPos);
	}


	return;
}
void text::Parser::GetSectionBetweenW( gen::DataStreamW & buffer_p, const wchar_t cOpen_p, const wchar_t cClose_p, int & iValidStartPos, unsigned int & nValidPortion_p )
{
	unsigned int iByteIndex = 0;
	iValidStartPos = -1;

	while( iByteIndex < buffer_p.m_size ){

		// if still not found any comment

		if( buffer_p.m_data[iByteIndex] == cOpen_p){

			// found comment start

			iValidStartPos = iByteIndex;
			break;
		}

		++iByteIndex;
	}

	if( iValidStartPos == -1 
		||
		(unsigned int)iValidStartPos == buffer_p.m_size - 1 )
		return;

	//----------------------------

	// traverse backwards till valid is found

	iByteIndex = buffer_p.m_size - 1; // start on last element
	int iCommentEndPos = -1;

	while( (int)iByteIndex > iValidStartPos ){

		// if looking for the end of the comment

		if( buffer_p.m_data[iByteIndex] == cClose_p){

			iCommentEndPos = iByteIndex;
			break;
		}

		--iByteIndex;
	}

	nValidPortion_p = (iCommentEndPos+1) - iValidStartPos;

	// including delimiters!!!

	return;
}

void Parser::ReplaceCharsByW( const wchar_t * szChars_p, wchar_t cReplacement_p, DataStreamW & buffer_p   )
{
	unsigned int iByteIndex = 0;

	while( iByteIndex < buffer_p.m_size ){


		for( int itChars = 0; szChars_p[itChars] != 0; ++itChars ){

			if( buffer_p.m_data[iByteIndex] == szChars_p[itChars] ){
				buffer_p.m_data[iByteIndex] = cReplacement_p;
				break;
			}
		}

		++iByteIndex;
	}
}

void Parser::RemoveCharsW( const wchar_t * szToRemove_p, DataStreamW & buffer_p )
{
	//unsigned int iByteIndex = 0;

	while( buffer_p.m_currentByteIndex < buffer_p.m_size ){


		for( int itChars = 0; szToRemove_p[itChars] != 0; ++itChars ){

			if( buffer_p.m_data[buffer_p.m_currentByteIndex] == szToRemove_p[itChars] ){

				memcpy( &buffer_p.m_data[buffer_p.m_currentByteIndex],
					&buffer_p.m_data[buffer_p.m_currentByteIndex+1],
					buffer_p.m_size - buffer_p.m_currentByteIndex );

				--buffer_p.m_size;

				break;
			}
		}

		++buffer_p.m_currentByteIndex;
	}
}

void Parser::GetSectionBetweenInvalidsOnTipsW( const wchar_t * szInvalids_p, DataStreamW & buffer_p, int & iValidStartPos_p, unsigned int & nValidPortion_p )
{
	assert( buffer_p.m_currentByteIndex < buffer_p.m_size );

	unsigned int iByteIndex = buffer_p.m_currentByteIndex;

	// traverse till valid is found

	bool bInvalid = false;
	while(	iByteIndex < buffer_p.m_size ){

		// compare against all invalid
		// if theres any invalid, mark as invalid..

		bInvalid = false;

		for( int itInv = 0; szInvalids_p[itInv] != 0; ++itInv ){

			if( buffer_p.m_data[iByteIndex] == szInvalids_p[itInv] ){

				bInvalid = true;
			}
		}

		if( !bInvalid ) break;

		++iByteIndex;
	}

	if( bInvalid ){

		iValidStartPos_p = -1;
		nValidPortion_p = 0;
		return;
	}

	iValidStartPos_p = iByteIndex;


	// traverse backwards till valid is found

	iByteIndex = buffer_p.m_size - 1; // start on last element

	while( (int)iByteIndex > iValidStartPos_p ){

		bInvalid = false;

		for( int itInv = 0; szInvalids_p[itInv] != 0; ++itInv ){

			if( buffer_p.m_data[iByteIndex] == szInvalids_p[itInv] ){

				bInvalid = true;
			}
		}

		if( !bInvalid ) break;

		--iByteIndex;
	}

	int iValidEnd = iByteIndex;
	nValidPortion_p = (iValidEnd - iValidStartPos_p) + 1;
}

void text::Parser::ReplaceSectionByW( const wchar_t cDelimiter_p, gen::DataStreamW & buffer_p, const wchar_t cReplacement_p )
{
	int iValidStart; unsigned int nValids;
	GetSectionBetweenW( buffer_p, cDelimiter_p, cDelimiter_p, iValidStart, nValids );

	if( iValidStart <= 0 ) return;
	if( nValids <= 0 ) return;

	// replace open delimiter by replacement

	buffer_p.m_data[ iValidStart ] = cReplacement_p;

	// move everything backwards

	memcpy( &buffer_p.m_data[iValidStart+1],
		&buffer_p.m_data[iValidStart+nValids],
		buffer_p.m_size - (iValidStart+1) );

	buffer_p.m_size -= nValids;
}

void text::Parser::AdjustQuotedNewLinesW( gen::DataStreamW & buffer_p, const wchar_t cForNewLineOpen_p /*= '\\'*/, const wchar_t cForNewLineClose_p /*= '\\'*/, const wchar_t cReplacement_p /*= '\n' */, const wchar_t cNewLine )
{
	unsigned int iByteIndex = 0;
	int iForNewLineStartPos = -1;

	while( iByteIndex < buffer_p.m_size ){

		// if still not found any formatted new line

		if( iForNewLineStartPos == -1 ){

			if( buffer_p.m_data[iByteIndex] == cForNewLineOpen_p){

				if( iByteIndex + 1 < buffer_p.m_size )
					if(  buffer_p.m_data[iByteIndex+1] == cNewLine ){

						// found formatted new line

						iForNewLineStartPos = iByteIndex;
					}	
			}
		}
		else{

			// if looking for the end of the formatted new line

			if( buffer_p.m_data[iByteIndex] == cForNewLineClose_p){

				int iCommentEndPos = iByteIndex;

				// from after formatted new line to EOF, copy it over the start of the new line

				int iSizeAfterComment =  buffer_p.m_size - iCommentEndPos;

				memcpy(		&buffer_p.m_data[iForNewLineStartPos+1],
					&buffer_p.m_data[iCommentEndPos+1],
					iSizeAfterComment	);

				// "trim" data size

				int iCommentSize = (iCommentEndPos+1) - (iForNewLineStartPos+1);
				buffer_p.m_size -= iCommentSize;

				// account current pos to the chopped part

				iByteIndex = iByteIndex - iCommentSize; // iByteIndex += 1; -> on \r, not \n

				// replace

				buffer_p.m_data[iForNewLineStartPos] = cReplacement_p;

				// start looking for formatted new lines again

				iForNewLineStartPos = -1;
			}
		}


		++iByteIndex;
	}

	if( iForNewLineStartPos != -1 ){

		// chop off the formatted new line at the EOF

		buffer_p.m_size -= (buffer_p.m_size - iForNewLineStartPos);
	}

	return;
}


//========================================================================
// 
//========================================================================

void GfigElementW::ParseGfigFile( DataStreamW & buffer_p, GfigElementW & parsed_p ){

	// remove comments

	//Parser::RemoveSectionsBetween( buffer_p, '#', '\n' );

	Parser::RemoveUnquotedSectionsBetweenW( buffer_p, L'#', L'\n', L'"', L'\\' );

	// enter recursion

	ParseCleanGfigFile( buffer_p, parsed_p );
}

void GfigElementW::ParseCleanGfigFile( DataStreamW & buffer_p, GfigElementW & parsed_p )
{

	int iOpenBracketPos = -1;
	int iEqualsPos = -1;
	bool bIgnoringDueQuote = false;

	while( buffer_p.m_currentByteIndex < buffer_p.m_size ){

		if( buffer_p.m_data[buffer_p.m_currentByteIndex] == L'"' ){

			if( bIgnoringDueQuote ){

				// check if quote inside quote

				if( buffer_p.m_data[buffer_p.m_currentByteIndex-1] == L'\\'){// \

					// than ignore this quote

					// remove inverse slash

					memcpy( &buffer_p.m_data[buffer_p.m_currentByteIndex-1],
						&buffer_p.m_data[buffer_p.m_currentByteIndex],
						buffer_p.m_size - buffer_p.m_currentByteIndex );

					--buffer_p.m_size;

					continue;
				}
			}

			bIgnoringDueQuote = !bIgnoringDueQuote;
		}

		if( !bIgnoringDueQuote ){

			if( buffer_p.m_data[buffer_p.m_currentByteIndex] == L'[' ){

				// check if theres already an open bracket

				if( iOpenBracketPos != -1 ){

					if( iEqualsPos != -1 ){

						// if theres an equals pending

						// all data between equals and open bracket
						// must be set as value

						unsigned int nCharsBetweenEqualsAndOpenBracket = (buffer_p.m_currentByteIndex - iEqualsPos) -1;

						if( nCharsBetweenEqualsAndOpenBracket ){

							// discard spaces on both ends, remove possible quotes

							DataStreamW bufferTmp = buffer_p;
							bufferTmp.m_currentByteIndex = iEqualsPos + 1; // after equals
							bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenEqualsAndOpenBracket; // amount after bracket

							ParseAssign( bufferTmp, parsed_p.m_value );
						}

						iEqualsPos = -1;
					}
					else if( parsed_p.m_name.empty() ){

						// if didnt got a name yet
						// get all data between this open bracket and the previous one

						unsigned int nCharsBetweenOpenBrackets = (buffer_p.m_currentByteIndex - iOpenBracketPos) -1;

						if( nCharsBetweenOpenBrackets ){

							// discard spaces on both ends
							DataStreamW bufferTmp = buffer_p;
							bufferTmp.m_currentByteIndex = iOpenBracketPos + 1;
							bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenOpenBrackets;

							ParseAssign( bufferTmp, parsed_p.m_name );
						}
					}

					// this new open bracket is a new element

					GfigElementW subElement;

					//++nOpenBracketsAbove_p;

					ParseCleanGfigFile( buffer_p, subElement /*, nOpenBracketsAbove_p*/ );

					//--nOpenBracketsAbove_p;

					subElement.m_indexOnParent = (int)parsed_p.m_subElements.size();
					parsed_p.m_subElements.emplace_back( std::move(subElement) ); // VS 2012 have bugfree emplaceback(TODO)
				}
				else{

					// hold new open bracket

					iOpenBracketPos = buffer_p.m_currentByteIndex;
				}
			}
			else if( buffer_p.m_data[buffer_p.m_currentByteIndex] == L'=' ){

				// assure theres an open bracket

				if( iOpenBracketPos != -1 ){

					iEqualsPos = buffer_p.m_currentByteIndex;

					// all data between open bracket and equals must be set as name
					// note that here is not needed to test if name is empty, cause
					// there cant be a name already set between '[' and '=', that
					// would require 2 or more equals..

					unsigned int nCharsBetweenOpenBracketAndEquals = (iEqualsPos - iOpenBracketPos) -1;

					if( nCharsBetweenOpenBracketAndEquals ){

						// discard spaces on both ends
						DataStreamW bufferTmp = buffer_p;
						bufferTmp.m_currentByteIndex = iOpenBracketPos + 1;
						bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenOpenBracketAndEquals;

						ParseAssign( bufferTmp, parsed_p.m_name );

						// here would be a good moment to get the quoted section and remove the babk slashes..

					}				
				}
			}
			else if( buffer_p.m_data[buffer_p.m_currentByteIndex] == L']' ){

				// assure there an open bracket

				if( iOpenBracketPos != -1 ){

					int iCloseBracketPos = buffer_p.m_currentByteIndex;

					if( iEqualsPos != -1 ){

						// if theres an equals pending

						// all data between equals and end bracket
						// must be set as value

						unsigned int nCharsBetweenEqualsAndEndBracket = (iCloseBracketPos - iEqualsPos) -1;

						if( nCharsBetweenEqualsAndEndBracket ){

							// discard spaces on both ends
							DataStreamW bufferTmp = buffer_p;
							bufferTmp.m_currentByteIndex = iEqualsPos + 1;
							bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenEqualsAndEndBracket;

							ParseAssign( bufferTmp, parsed_p.m_value );
						}

						iEqualsPos = -1;
					}
					else{

						if( parsed_p.m_name.empty() ){

							unsigned int nCharsBetweenBrackets = (iCloseBracketPos - iOpenBracketPos) -1; // [#,4,5], 5-4 = 1, -1 = 0

							if( nCharsBetweenBrackets ){

								// discard spaces on both ends
								DataStreamW bufferTmp = buffer_p;
								bufferTmp.m_currentByteIndex = iOpenBracketPos + 1;
								bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenBrackets;

								ParseAssign( bufferTmp, parsed_p.m_name );
							}
						}
					}

					return;
				}
			}
		}


		++buffer_p.m_currentByteIndex;
	}
}

void GfigElementW:: ParseAssign( DataStreamW & buffer_p, std::wstring & szOut_p )
{
	assert( szOut_p.empty() );

	// discard spaces on both ends

	int iStart;
	unsigned int nChars;
	Parser::GetSectionBetweenInvalidsOnTipsW( L" \t\r\n", buffer_p, iStart, nChars );

	// if quoted, remove quotes

	if( buffer_p.m_data[iStart] == L'"'
		&&
		buffer_p.m_data[ iStart + nChars -1 ] == L'"' ){

			++iStart;
			nChars -= 2;
	}

	// new
	buffer_p.m_currentByteIndex = 0;//iStart;
	buffer_p.m_data += iStart;
	buffer_p.m_size = nChars;//buffer_p.m_currentByteIndex + nChars;

	Parser::AdjustQuotedNewLinesW( buffer_p, L'\\', L'\\', L'\n');
	// new

	// assign
	//std::string sz((const char*)&buffer_p.m_data[buffer_p.m_currentByteIndex], buffer_p.m_size );
	//szOut_p.assign( sz.begin(), sz.end() );
	szOut_p.assign( (const wchar_t*)&buffer_p.m_data[buffer_p.m_currentByteIndex], buffer_p.m_size );
	BREAKHERE;
}

bool GfigElementW::GetSubElement( const wchar_t * szName_p, GfigElementW *& pElement_p )
{
	for( int it = 0, itEnd = (int)m_subElements.size(); it != itEnd; ++it ){

		if( m_subElements[it].m_name == szName_p ){

			pElement_p = &m_subElements[it];

			return true;
		}
	}

	return false;
}

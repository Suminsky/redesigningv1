#include "TextParser.h"

#include <memory>
#include <string>
#include <vector>

#include "../namespace gen/gen_macros.h"

using namespace text;
using namespace std;
using namespace gen;

void Parser::RemoveSectionsBetween( DataStream & buffer_p, char cOpen_p, char cClose_p )
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

void Parser::ReplaceCharsBy( const char * szChars_p, char cReplacement_p, DataStream & buffer_p   )
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

void Parser::RemoveChars( const char * szToRemove_p, DataStream & buffer_p )
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

void Parser::GetSectionBetweenInvalids( const char * szInvalids_p, DataStream & buffer_p, int & iValidStartPos_p, unsigned int & nValidPortion_p )
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

void Parser::GetSectionBetweenDelimiter( const char cDelimiter_p, DataStream & buffer_p, int & iValidStartPos_p, unsigned int & nValidPortion_p )
{
	assert( buffer_p.m_currentByteIndex < buffer_p.m_size );

	unsigned int iByteIndex = buffer_p.m_currentByteIndex;

	// traverse till valid is found

	bool bInvalid = false;
	while(	iByteIndex < buffer_p.m_size ){

		// compare against all invalid
		// if theres any invalid, mark as invalid..

		if( buffer_p.m_data[iByteIndex] == cDelimiter_p ){

			bInvalid = true;
			break;
		}

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

		if( buffer_p.m_data[iByteIndex] == cDelimiter_p){

			break;
		}

		--iByteIndex;
	}

	int iValidEnd = iByteIndex;
	nValidPortion_p = (iValidEnd - iValidStartPos_p) + 1;
}

//========================================================================
// 
//========================================================================

void GfigElement::ParseGfigFile( DataStream & buffer_p, GfigElement & parsed_p ){

	// remove comments

	Parser::RemoveSectionsBetween( buffer_p, '#', '\n' );

	// enter recursion

	ParseCleanGfigFile( buffer_p, parsed_p );
}

void GfigElement::ParseCleanGfigFile( DataStream & buffer, GfigElement & parsed_p /*, int nOpenBracketsAbove_p = 0*/ )
{
	//unsigned int iByteIndex = buffer.m_currentByteIndex;

	int iOpenBracketPos = -1;
	int iEqualsPos = -1;

	while( buffer.m_currentByteIndex < buffer.m_size ){

		if( buffer.m_data[buffer.m_currentByteIndex] == '[' ){

			// check if theres already an open bracket

			if( iOpenBracketPos != -1 ){

				if( iEqualsPos != -1 ){

					// if theres an equals pending

					// all data between equals and open bracket
					// must be set as value

					unsigned int nCharsBetweenEqualsAndOpenBracket = (buffer.m_currentByteIndex - iEqualsPos) -1;

					if( nCharsBetweenEqualsAndOpenBracket ){

						// discard spaces on both ends, remove possible quotes

						DataStream bufferTmp = buffer;
						bufferTmp.m_currentByteIndex = iEqualsPos + 1; // after equals
						bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenEqualsAndOpenBracket; // amount after bracket

						ParseAssign( bufferTmp, parsed_p.m_value );
					}

					iEqualsPos = -1;
				}
				else if( parsed_p.m_name.empty() ){

				// if didnt got a name yet
				// get all data between this open bracket and the previous one

					unsigned int nCharsBetweenOpenBrackets = (buffer.m_currentByteIndex - iOpenBracketPos) -1;

					if( nCharsBetweenOpenBrackets ){

						// discard spaces on both ends
						DataStream bufferTmp = buffer;
						bufferTmp.m_currentByteIndex = iOpenBracketPos + 1;
						bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenOpenBrackets;

						ParseAssign( bufferTmp, parsed_p.m_name );
					}
				}

				// this new open bracket is a new element

				GfigElement subElement;

				//++nOpenBracketsAbove_p;

				ParseCleanGfigFile( buffer, subElement /*, nOpenBracketsAbove_p*/ );

				//--nOpenBracketsAbove_p;

				subElement.m_indexOnParent = (int)parsed_p.m_subElements.size();
				parsed_p.m_subElements.emplace_back( std::move(subElement) ); // VS 2012 have bugfree emplaceback(TODO)
			}
			else{

				// hold new open bracket
					
				iOpenBracketPos = buffer.m_currentByteIndex;
			}
		}
		else if( buffer.m_data[buffer.m_currentByteIndex] == '=' ){

			// assure theres an open bracket

			if( iOpenBracketPos != -1 ){

				iEqualsPos = buffer.m_currentByteIndex;

				// all data between open bracket and equals must be set as name
				// note that here is not needed to test if name is empty, cause
				// there cant be a name already set between '[' and '=', that
				// would require 2 or more equals..
				
				unsigned int nCharsBetweenOpenBracketAndEquals = (iEqualsPos - iOpenBracketPos) -1;

				if( nCharsBetweenOpenBracketAndEquals ){

					// discard spaces on both ends
					DataStream bufferTmp = buffer;
					bufferTmp.m_currentByteIndex = iOpenBracketPos + 1;
					bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenOpenBracketAndEquals;
					
					ParseAssign( bufferTmp, parsed_p.m_name );
				}				
			}
		}
		else if( buffer.m_data[buffer.m_currentByteIndex] == ']' ){

			// assure there an open bracket

			if( iOpenBracketPos != -1 ){

				int iCloseBracketPos = buffer.m_currentByteIndex;

				if( iEqualsPos != -1 ){

					// if theres an equals pending

					// all data between equals and end bracket
					// must be set as value
					
					unsigned int nCharsBetweenEqualsAndEndBracket = (iCloseBracketPos - iEqualsPos) -1;

					if( nCharsBetweenEqualsAndEndBracket ){

						// discard spaces on both ends
						DataStream bufferTmp = buffer;
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
							DataStream bufferTmp = buffer;
							bufferTmp.m_currentByteIndex = iOpenBracketPos + 1;
							bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenBrackets;

							ParseAssign( bufferTmp, parsed_p.m_name );
						}
					}
				}

				return;
			}
		}

		++buffer.m_currentByteIndex;
	}
}

void GfigElement:: ParseAssign( DataStream & buffer_p, std::string & szOut_p )
{
	assert( szOut_p.empty() );

	// discard spaces on both ends

	int iStart;
	unsigned int nChars;
	Parser::GetSectionBetweenInvalids( " \t\r\n", buffer_p, iStart, nChars );

	// if quoted, remove quotes

	if( buffer_p.m_data[iStart] == '"'
		&&
		buffer_p.m_data[ iStart + nChars -1 ] == '"'){

			++iStart;
			nChars -= 2;
	}

	// assign

	szOut_p.assign( (const char*)&buffer_p.m_data[iStart], nChars );
}

bool GfigElement::GetSubElement( const char * szName_p, GfigElement *& pElement_p )
{
	for( int it = 0, itEnd = (int)m_subElements.size(); it != itEnd; ++it ){

		if( m_subElements[it].m_name == szName_p ){

			pElement_p = &m_subElements[it];

			return true;
		}
	}

	return false;
}
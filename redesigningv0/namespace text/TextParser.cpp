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
void Parser::RemoveUnquotedSectionsBetween( DataStream & buffer_p, char cOpen_p, char cClose_p, char cQuote, char cQuoteShield )
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

	//ReplaceCharsBy( "\n", '!', buffer_p );
	//ReplaceCharsBy( "\r", '?', buffer_p );

	return;
}
void text::Parser::GetSectionBetween( gen::DataStream & buffer_p, const char cOpen_p, const char cClose_p, int & iValidStartPos, unsigned int & nValidPortion_p )
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

void Parser::GetSectionBetweenInvalidsOnTips( const char * szInvalids_p, DataStream & buffer_p, int & iValidStartPos_p, unsigned int & nValidPortion_p )
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
				break;
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

				bInvalid = true; //todo: insert break
				break;
			}
		}

		if( !bInvalid ) break;

		--iByteIndex;
	}

	int iValidEnd = iByteIndex;
	nValidPortion_p = (iValidEnd - iValidStartPos_p) + 1;
}

void text::Parser::ReplaceSectionBy( const char cDelimiter_p, gen::DataStream & buffer_p, const char cReplacement_p )
{
	int iValidStart; unsigned int nValids;
	GetSectionBetween( buffer_p, cDelimiter_p, cDelimiter_p, iValidStart, nValids );

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

void text::Parser::AdjustQuotedNewLines( gen::DataStream & buffer_p, const char cForNewLineOpen_p /*= '\\'*/, const char cForNewLineClose_p /*= '\\'*/, const char cReplacement_p /*= '\n' */, const char cNewLine )
{
	// TODO, detect if newlines are only \n or \r\n,
	// currently assuming \r\n

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

bool text::Parser::NeedQuotes( const char cFirst_p, const char cLast_p, const char * szInvalids_p )
{
	// traverse invalids string
	for( int it = 0; szInvalids_p[it]; ++it ){

		//check if first or last char is invalid
		if( cFirst_p == szInvalids_p[it] )
			return true;
		if( cLast_p == szInvalids_p[it] )
			return true;
	}

	return false;
}

//========================================================================
// 
//========================================================================

void GfigElementA::ParseGfigFile( DataStream & buffer_p, GfigElementA & parsed_p ){

	// remove comments

	//Parser::RemoveSectionsBetween( buffer_p, '#', '\n' );

	Parser::RemoveUnquotedSectionsBetween( buffer_p, '#', '\n', '"', '\\' );

	// enter recursion

	ParseCleanGfigFile( buffer_p, parsed_p );
}

void GfigElementA::ParseCleanGfigFile( DataStream & buffer_p, GfigElementA & parsed_p /*, int nOpenBracketsAbove_p = 0*/ )
{
	//unsigned int iByteIndex = buffer_p.m_currentByteIndex;

	int iOpenBracketPos = -1;
	int iEqualsPos = -1;
	bool bIgnoringDueQuote = false;

	// test
	bool bNameHandled = false;

	while( buffer_p.m_currentByteIndex < buffer_p.m_size ){

		if( buffer_p.m_data[buffer_p.m_currentByteIndex] == '"' ){

			if( bIgnoringDueQuote ){
				
				// check if quote inside quote
				
				if( buffer_p.m_data[buffer_p.m_currentByteIndex-1] == '\\'){// \
					
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

			if( buffer_p.m_data[buffer_p.m_currentByteIndex] == '[' ){

				// check if theres already an open bracket

				if( iOpenBracketPos != -1 ){

					if( iEqualsPos != -1 ){

						// if theres an equals pending

						// all data between equals and open bracket
						// must be set as value

						unsigned int nCharsBetweenEqualsAndOpenBracket = (buffer_p.m_currentByteIndex - iEqualsPos) -1;

						if( nCharsBetweenEqualsAndOpenBracket ){

							// discard spaces on both ends, remove possible quotes

							DataStream bufferTmp = buffer_p;
							bufferTmp.m_currentByteIndex = iEqualsPos + 1; // after equals
							bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenEqualsAndOpenBracket; // amount after bracket

							ParseAssign( bufferTmp, parsed_p.m_value );
						}

						iEqualsPos = -1;
					}
					else if( !bNameHandled /*parsed_p.m_name.empty()*/ ){

						bNameHandled = true;


					// if didnt got a name yet
					// get all data between this open bracket and the previous one
					// 
					// problem: this can be the second child of a nameless bracket, means it
					// will get the first child as name!

						unsigned int nCharsBetweenOpenBrackets = (buffer_p.m_currentByteIndex - iOpenBracketPos) -1;

						if( nCharsBetweenOpenBrackets ){

							// discard spaces on both ends
							DataStream bufferTmp = buffer_p;
							bufferTmp.m_currentByteIndex = iOpenBracketPos + 1;
							bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenOpenBrackets;

							ParseAssign( bufferTmp, parsed_p.m_name );
						}
					}

					// this new open bracket is a new element

					GfigElementA subElement;

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
			else if( buffer_p.m_data[buffer_p.m_currentByteIndex] == '=' ){

				// assure theres an open bracket

				if( iOpenBracketPos != -1 ){

					iEqualsPos = buffer_p.m_currentByteIndex;

					// all data between open bracket and equals must be set as name
					// note that here is not needed to test if name is empty, cause
					// there cant be a name already set between '[' and '=', that
					// would require 2 or more equals..
					// 
					bNameHandled = true;
				
					unsigned int nCharsBetweenOpenBracketAndEquals = (iEqualsPos - iOpenBracketPos) -1;

					if( nCharsBetweenOpenBracketAndEquals ){

						// discard spaces on both ends
						DataStream bufferTmp = buffer_p;
						bufferTmp.m_currentByteIndex = iOpenBracketPos + 1;
						bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenOpenBracketAndEquals;
					
						ParseAssign( bufferTmp, parsed_p.m_name );

						// here would be a good moment to get the quoted section and remove the babk slashes..
						
					}				
				}
			}
			else if( buffer_p.m_data[buffer_p.m_currentByteIndex] == ']' ){

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
							DataStream bufferTmp = buffer_p;
							bufferTmp.m_currentByteIndex = iEqualsPos + 1;
							bufferTmp.m_size = bufferTmp.m_currentByteIndex + nCharsBetweenEqualsAndEndBracket;
						
							ParseAssign( bufferTmp, parsed_p.m_value );
						}

						iEqualsPos = -1;
					}
					else{

						if( !bNameHandled/*parsed_p.m_name.empty()*/ ){

							bNameHandled = true;

							unsigned int nCharsBetweenBrackets = (iCloseBracketPos - iOpenBracketPos) -1; // [#,4,5], 5-4 = 1, -1 = 0

							if( nCharsBetweenBrackets ){

								// discard spaces on both ends
								DataStream bufferTmp = buffer_p;
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

void GfigElementA:: ParseAssign( DataStream & buffer_p, std::string & szOut_p )
{
	assert( szOut_p.empty() );

	// discard spaces on both ends

	int iStart;
	unsigned int nChars;
	Parser::GetSectionBetweenInvalidsOnTips( " \t\r\n", buffer_p, iStart, nChars );

	// if quoted, remove quotes

	if( buffer_p.m_data[iStart] == '"'
		&&
		buffer_p.m_data[ iStart + nChars -1 ] == '"' ){

			++iStart;
			nChars -= 2;
	}

	// new
	buffer_p.m_currentByteIndex = 0;//iStart;
	buffer_p.m_data += iStart;
	buffer_p.m_size = nChars;//buffer_p.m_currentByteIndex + nChars;

	Parser::AdjustQuotedNewLines( buffer_p, '\\', '\\', '\n');
	// new
	
	// assign

	szOut_p.assign( (const char*)&buffer_p.m_data[buffer_p.m_currentByteIndex], buffer_p.m_size );
}

bool GfigElementA::GetSubElement( const char * szName_p, GfigElementA *& pElement_p )
{
	for( int it = 0, itEnd = (int)m_subElements.size(); it != itEnd; ++it ){

		if( m_subElements[it].m_name == szName_p ){

			pElement_p = &m_subElements[it];

			return true;
		}
	}

	return false;
}

bool text::GfigElementA::FindFirstElementBFS( const char * szName_p, GfigElementA *& pElement_p )
{
	std::vector<GfigElementA*> vCandidates;

	vCandidates.push_back( pElement_p );
	int itFront = 0;

	while( vCandidates.size() - itFront > 0 ){

		GfigElementA * pCurrent = vCandidates[itFront];

		if( pCurrent->m_name == szName_p ){

			pElement_p = pCurrent;
			return true;
		}

		++itFront;

		for( int it = 0, itEnd = (int)pCurrent->m_subElements.size(); it != itEnd; ++it ){

			vCandidates.push_back( &pCurrent->m_subElements[it] );
		}
	}

	return false;
}
bool text::GfigElementA::FindLastElementDFS( const char * szName_p, GfigElementA *& pElement_p )
{
	std::vector<GfigElementA*> vCandidates;

	vCandidates.push_back( pElement_p );

	while( vCandidates.size() > 0 ){

		GfigElementA * pCurrent = vCandidates.back();
		vCandidates.pop_back();

		if( pCurrent->m_name == szName_p ){

			pElement_p = pCurrent;
			return true;
		}

		for( int it = 0, itEnd = (int)pCurrent->m_subElements.size(); it != itEnd; ++it ){

			vCandidates.push_back( &pCurrent->m_subElements[it] );
		}
	}

	return false;
}
bool text::GfigElementA::FindFirstElementDFS( const char * szName_p, GfigElementA *& pElement_p )
{
	std::vector<GfigElementA*> vCandidates;

	vCandidates.push_back( pElement_p );

	while( vCandidates.size() > 0 ){

		GfigElementA * pCurrent = vCandidates.back();
		vCandidates.pop_back();

		if( pCurrent->m_name == szName_p ){

			pElement_p = pCurrent;
			return true;
		}

		for( int it = (int)pCurrent->m_subElements.size()-1; it > -1; --it ){

			vCandidates.push_back( &pCurrent->m_subElements[it] );
		}
	}

	return false;
}

void text::GfigElementA::SaveGfigToFile( const GfigElementA & gfig_p, std::string & header_p,
										gen::DataStream & buffer_p, const char cNewLine, const char cTab ){

	//new
	header_p += '\r';// /new
	header_p += '\n';

	// create formated string

	SaveGfigToFile( gfig_p, header_p, 0, cNewLine, cTab );

	// allocate bigger buffer if needed

	int size = (int)header_p.size();

	if( buffer_p.m_currentByteIndex + size > buffer_p.m_size ){

		int sizeNeed = buffer_p.m_currentByteIndex + size;

		DataStream newBuffer_p;
		newBuffer_p.m_data = new unsigned char[sizeNeed];
		newBuffer_p.m_size = sizeNeed;
		newBuffer_p.Set( buffer_p.m_data, buffer_p.m_currentByteIndex );
		if( buffer_p.m_size ) delete [] buffer_p.m_data;
		buffer_p = newBuffer_p;
	}

	// copy data to buffer

	memcpy( (void*)buffer_p.m_data, (void*)header_p.data(), sizeof(char)*size );
}

void text::GfigElementA::SaveGfigToFile( const GfigElementA & gfig_p, std::string & formated_p, int currentTabIndentation_p, const char cNewLine, const char cTab ){

	const char szQuotedName[] = { '[', '"', 0 };
	const char szQUotedValue[] = { '=', '"', 0 };

	// save element header
	// variations:
	// [name = value]\n
	// [name = value\n\t
	// [name]\n
	// [name\n\t
	
	// new testing
	formated_p += '\r';// /new testing
	formated_p += cNewLine;
	for( int itTab = 0; itTab < currentTabIndentation_p; ++ itTab )
		formated_p += cTab;

	if( Parser::NeedQuotes(  gfig_p.m_name[0], gfig_p.m_name[(int)gfig_p.m_name.size()-1], " \n\t\r" ) ){

		formated_p += szQuotedName + gfig_p.m_name + '"';
	}
	else 
	formated_p += '[' + gfig_p.m_name; // [name

	if( !gfig_p.m_value.empty() ){

		if( Parser::NeedQuotes(  gfig_p.m_value[0], gfig_p.m_value[(int)gfig_p.m_value.size()-1], " \n\t\r" ) ){

			//formated_p += szQUotedValue + gfig_p.m_value + '"';

			//new
			formated_p += szQUotedValue;
			AdjustNewLineTabFormatting( gfig_p.m_value, formated_p, currentTabIndentation_p, cTab, cNewLine );
			formated_p += '"';
		}
		else{

			//formated_p += " = " + gfig_p.m_value; // [name = value

			//new
			formated_p += " = ";
			AdjustNewLineTabFormatting( gfig_p.m_value, formated_p, currentTabIndentation_p, cTab, cNewLine );
		}
		
	}

	int nChildElements = (int)gfig_p.m_subElements.size();
	if( nChildElements != 0 ){

		++currentTabIndentation_p;

		for( int itChild = 0; itChild < nChildElements; ++ itChild ){

			SaveGfigToFile( gfig_p.m_subElements[itChild], formated_p, currentTabIndentation_p, cNewLine, cTab );
		}

		--currentTabIndentation_p;

		// new testing
		formated_p += '\r';// /new testing
		formated_p += cNewLine;
		for( int itTab = 0; itTab < currentTabIndentation_p; ++ itTab )
			formated_p += cTab;

		formated_p += ']';
	}
	else{

		formated_p += ']'; // [name]\n or  [name = value]\n
	}
}

void text::GfigElementA::AdjustNewLineTabFormatting( const std::string & szIn_p, std::string & formated_p, int currentTabIndentation_p, const char cTab /*= '\t'*/, const char cNewLine /*= '\n'*/ )
{

	// NOTE: sz.size() =  (null)
	// sz.size-1 = last char
	
	int itLastChar = (int)szIn_p.size()-1;

	bool bOpenedNewLine = false;
	for( int itC = 0; itC <= itLastChar; ++itC ){

		if( szIn_p[itC] == cNewLine ){

			formated_p += '\\';
			formated_p += '\r';formated_p += cNewLine;
			for( int itTab = 0; itTab < currentTabIndentation_p; ++ itTab )
				formated_p += cTab;
			bOpenedNewLine = true;
		}
		else{
			
			if( bOpenedNewLine ){

				formated_p += '\\';
				bOpenedNewLine = false;
			}

			formated_p += szIn_p[itC];
		}
	}
}

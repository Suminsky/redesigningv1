#include "FileLogger.h"

#include<assert.h>
#include<stdio.h>//sprintf_s
#include <exception>

using namespace win;

win::FileLogger::FileLogger( const char* szFileName_p ) :m_szBuff(NULL), m_nBuffChars(0), m_iAvailableSize(I_BUFFSIZE_MINUS_SZABOVEFLUSHED), m_nMinDigits(6), m_nFractionalDigits(4)
{
	assert(szFileName_p != NULL);

	// check for appended .glog string

	int iLen = CountString(szFileName_p);
	bool bAppendDOTGLOG = TRUE;
	if( iLen > 5 && CompareString(szFileName_p, ".glog")){

		bAppendDOTGLOG = FALSE;
	}

	CHAR szNewName[256];
	if( bAppendDOTGLOG ){

		sprintf_s( szNewName, 256, "%s.glog", szFileName_p );
	}else{

		strcpy_s( szNewName, 256, szFileName_p );
	}

	// create log file(or open, if its the same name) for writing:

	m_hLogFile = CreateFileA( szNewName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN|FILE_ATTRIBUTE_NORMAL, NULL );

	if( m_hLogFile == INVALID_HANDLE_VALUE ){

		throw std::exception("could not create file");
	}

	// check if file already exists:

	if( GetLastError() == ERROR_ALREADY_EXISTS ){

		// so "delete"(truncate its size, the file pointer of the handle is used) it:

		SetEndOfFile( m_hLogFile );
	}

	m_fileOffset.Offset = m_fileOffset.OffsetHigh = 0xFFFFFFFF; // == append data
	m_fileOffset.hEvent = NULL;
	m_fileOffset.Internal = m_fileOffset.InternalHigh = NULL;

	SetDigitsCount(m_nMinDigits,m_nFractionalDigits);

	m_szBuff = new char[SZBUFF_SIZE];
}

win::FileLogger::FileLogger( const E_DATE_DESIGN eDateOrder /*= eDAY_MONTH_YEAR_HMS*/ ) :m_szBuff(NULL), m_nBuffChars(0), m_iAvailableSize(I_BUFFSIZE_MINUS_SZABOVEFLUSHED), m_nMinDigits(6), m_nFractionalDigits(4)
{
	char szName[128]; // will hold the name

	// get time info:

	SYSTEMTIME nowTime;
	GetLocalTime( &nowTime );

	// create string:

	switch( eDateOrder ){

	case eDAY_MONTH_YEAR_HMS:{
		sprintf_s( szName, 128, "%i_%i_%i_%ih%im%is.glog",	nowTime.wDay, nowTime.wMonth, nowTime.wYear,
			nowTime.wHour, nowTime.wMinute, nowTime.wSecond );
							 }break;
	case eMONTH_DAY_YEAR_HMS:{
		sprintf_s( szName, 128, "%i_%i_%i_%ih%im%is.glog",	nowTime.wMonth, nowTime.wDay, nowTime.wYear,
			nowTime.wHour, nowTime.wMinute, nowTime.wSecond );
							 }break;
	}//F switch date order

	// create log file for writing:-----------

	m_hLogFile = CreateFileA( szName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );

	// if already exists(very improbably, since time is used for name):

	for(UINT i = 1;  m_hLogFile == INVALID_HANDLE_VALUE; i++ ){

		// check(confirm) if file alredy exists:

		if( GetLastError() == ERROR_ALREADY_EXISTS ){

			// create one with the same name but with a (n) appended

			sprintf_s( szName, 128, "%i_%i_%i_%ih%im%is(%i).glog",	nowTime.wMonth, nowTime.wDay, nowTime.wYear,
				nowTime.wHour, nowTime.wMinute, nowTime.wSecond,
				i	);

			// try create again:

			m_hLogFile = CreateFileA( szName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
		}else{

			//MB( " TimerLog Error", "Fatal Error trying to create log file!", MB_E );
			break;
		}
	}//for createfile fail

	m_fileOffset.Offset = m_fileOffset.OffsetHigh = 0xFFFFFFFF;// == append data
	m_fileOffset.hEvent = NULL;
	m_fileOffset.Internal = m_fileOffset.InternalHigh = NULL;

	SetDigitsCount(m_nMinDigits, m_nFractionalDigits);

	m_szBuff = new char[SZBUFF_SIZE];
}

win::FileLogger::~FileLogger()
{
	// check if m_szBuff is not empty, if not, log the data
	// on it to file:
	if( m_nBuffChars > 0 ){//I_ABOVEFLUSHED_LEN

		FlushToFile();

	}
	//else{
	//	//check if file is empty, if so, delete it:
	//	char buff[1];
	//	int nBytes;
	//	m_fileOffset.OffsetHigh = 0;
	//	ReadFile(m_hLogFile, buff, 1, (LPDWORD)&nBytes, &m_fileOffset );

	//	if( nBytes == 0 ){

	//		DeleteFileA())
	//	}
	//}

	// close the file:
	CloseHandle( m_hLogFile );

	if( m_szBuff != NULL )
		delete m_szBuff;
}

//------------------------------------------------------------------------
FileLogger& FileLogger::operator<<( const char* stringA_p )
{
	// check if string to append is bigger than empty space:

	if( (CountString( stringA_p )+1) >= m_iAvailableSize ){

		// if it is, flush buffer  to file

		FlushToFile();
	}

	m_nBuffChars += sprintf_s( &m_szBuff[m_nBuffChars], m_iAvailableSize, "%s", stringA_p );
	m_iAvailableSize = I_BUFFSIZE_MINUS_SZABOVEFLUSHED - m_nBuffChars;

	return *this;
}
//------------------------------------------------------------------------
FileLogger& FileLogger::operator<<( const double &float_p )
{
	// max size of a float string is 25(counting minus sign)

	// check if string to append is bigger than empt space:

	if( (UINT)25 >= m_iAvailableSize ){

		// if it is, flush buffer to file:

		FlushToFile();
	}

	m_nBuffChars += sprintf_s( &m_szBuff[m_nBuffChars], m_iAvailableSize, "%*.*f", m_nMinDigits, m_nFractionalDigits, float_p );
	m_iAvailableSize = I_BUFFSIZE_MINUS_SZABOVEFLUSHED - m_nBuffChars;

	return *this;
}
//------------------------------------------------------------------------
FileLogger& FileLogger::operator<<( const int &int_p )
{
	// max size of a float string is 25(counting minus sign)

	// check if string to append is bigger than empt space:

	if( (UINT)25 >= m_iAvailableSize ){

		// if it is, flush buffer to file:

		FlushToFile();
	}

	m_nBuffChars += sprintf_s( &m_szBuff[m_nBuffChars], m_iAvailableSize, "%*i", m_nMinDigits, int_p );
	m_iAvailableSize = I_BUFFSIZE_MINUS_SZABOVEFLUSHED - m_nBuffChars;

	return *this;
}
//------------------------------------------------------------------------
void FileLogger::FlushToFile()
{
	// append flush to file information to buffer(above flushed):
	// NOTE: m_iAvailableSize is accounting for I_ABOVEFLUSHED_LEN, but here it shouldn't.
	
	sprintf_s( &m_szBuff[m_nBuffChars], (m_iAvailableSize + I_ABOVEFLUSHED_LEN), "%s", SZ_ABOVE_FLUSHED );

	m_nBuffChars += I_ABOVEFLUSHED_LEN;

	// write data to file:

	WriteFile( m_hLogFile, (LPCVOID)m_szBuff, (DWORD)m_nBuffChars, (LPDWORD)&m_nBytesWritten, &m_fileOffset );

	// reset buffer used count:

	m_nBuffChars = 0;

	// reset available buff space:

	m_iAvailableSize = I_BUFFSIZE_MINUS_SZABOVEFLUSHED;
}
//------------------------------------------------------------------------

namespace win{
	FileLogger& UniqueFileLogger(){

		static FileLogger logger;
		return logger;
	}
}
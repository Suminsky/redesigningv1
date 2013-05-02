//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2012/07/07
	created:	7:7:2012   23:40
	filename: 	c:\Users\Gateway\Documents\Visual Studio 2008\Projects\TestandoGFrameOnly\TestandoGFrameOnly\FileLogger.h
	file path:	c:\Users\Gateway\Documents\Visual Studio 2008\Projects\TestandoGFrameOnly\TestandoGFrameOnly
	file base:	FileLogger
	file ext:	h
	author:		Giuliano Suminsky (a.k.a. Icebone1000)
	
	purpose:	Logger class that logs to file, txt, ascii.
				Windows only.

	© Giuliano Suminsky (a.k.a. Icebone1000) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

#include <windows.h>

namespace win{

	class FileLogger{

	//100KB
	#define SZBUFF_SIZE 10240

	#define SZ_NEWLINE "\r\n"
	#define SZ_ABOVE_FLUSHED "-ABOVE FLUSHED-\r\n"
	#define I_ABOVEFLUSHED_LEN 17

	//buff size accounting above flushed string
	#define I_BUFFSIZE_MINUS_SZABOVEFLUSHED (SZBUFF_SIZE-I_ABOVEFLUSHED_LEN)

	private:

		HANDLE m_hLogFile;
			UINT m_nBuffChars;			//remember to always account for SZ_ABOVE_FLUSHED
			UINT m_iAvailableSize;
			UINT m_nBytesWritten;
			OVERLAPPED m_fileOffset;	//data need to append to file

		UINT m_nMinDigits;				//the minimum number of digits to write for numbers, after comma.
		UINT m_nFractionalDigits;		//the max number of digits after comma

		//char m_szBuff[SZBUFF_SIZE];
		char * m_szBuff;

		//------------------------------------------------------------------------
		static UINT CountString( const char* string_p )
		{
			int counter = 0;

			while( string_p[counter] != 0x00 ) ++counter;

			return counter;
		}
		static bool CompareString( const char* string1_p, const char* string2_p)
		{
			int it = 0;

			while( string1_p[it] == string2_p[it] )
			{
				if( string1_p[it] == 0x00 )
				{
					return true;
				}
				else
					++it;
			}

			return false;
		}
		//------------------------------------------------------------------------

	public:

		enum E_DATE_DESIGN{
			eDAY_MONTH_YEAR_HMS,
			eMONTH_DAY_YEAR_HMS
		};

		//------------------------------------------------------------------------
		// ctors
		//------------------------------------------------------------------------
		FileLogger(const char* szFileName_p);
		FileLogger(const E_DATE_DESIGN eDateOrder = eDAY_MONTH_YEAR_HMS);

		//------------------------------------------------------------------------
		// dctor
		//------------------------------------------------------------------------
		virtual ~FileLogger();

		//------------------------------------------------------------------------
		//Sets the max n of digits after comma.
		//------------------------------------------------------------------------
		void SetDigitsCount( UINT minCount_p = 6, UINT fractionalCount_p = 4){

			m_nMinDigits = minCount_p;
			m_nFractionalDigits = fractionalCount_p;
		}

		//-----------------------------------------
		// Log Operators:
		//-----------------------------------------
		FileLogger& operator <<( const char* stringA_p );
		FileLogger& operator <<( const double &float_p );//used for float e double
		FileLogger& operator<<( const int &int_p );

		//------------------------------------------------------------------------
		// Forces file writing/buffer cleaning.This is also a helper function used
		// by the class to write to the file always it needs
		// [-ABOVE FLUSHED-][newline]
		//------------------------------------------------------------------------
		void FlushToFile();
	};




	static FileLogger& UniqueFileLogger(){

		static FileLogger logger;
		return logger;
	}
}
#include "PCHF.h"
#include "net.h"

#pragma warning( push )
#pragma warning( disable: 4005 ) //'_WINSOCKAPI_' : macro redefinition
#include <WinSock2.h>
#pragma warning( pop )
#include <assert.h>

#include "../namespace gen/gen_macros.h"

namespace net{

	bool Init( E_ERROR * peError_p /*= nullptr*/ ){

		WSADATA wsaData;
		int result = WSAStartup( MAKEWORD(2,2), &wsaData );

		if( result == NO_ERROR ){

			return true;
		}
		else{

			assert( result != WSAEFAULT );
			assert( result != WSAEINPROGRESS );

			if( peError_p ){

				switch( result ){
				case WSASYSNOTREADY:		*peError_p = E_ERROR_SYSTEMNOTREADY;		break;
				case WSAVERNOTSUPPORTED:	*peError_p = E_ERROR_VERSIONNOTSUPPORTED;	break;
				case WSAEPROCLIM:			*peError_p = E_ERROR_SHITGOTFUCKED;			break;
				default:					*peError_p = E_ERROR_NOIDEA;				break;
				}
			}

			return false;
		}
	}

	bool Destroy( E_ERROR * peError_p /*= nullptr*/ ){

		int result = WSACleanup();

		if( result == NO_ERROR ){

			return true;
		}
		else{

			DBG(
			result = WSAGetLastError();

			assert( result != WSANOTINITIALISED );
			assert( result != WSAEINPROGRESS );
			);

			if( peError_p ){

				NDBG( result = WSAGetLastError(); );

				switch( result ){
				case WSAENETDOWN:			*peError_p = E_ERROR_SHITGOTFUCKED;			break;
				default:					*peError_p = E_ERROR_NOIDEA;				break;
				}
			}

			return false;
		}
	}

}
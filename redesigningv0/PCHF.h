// standard includes

#ifdef NDEBUG
	#define _HAS_EXCEPTIONS 0
#endif

//#pragma warning(push)
//#pragma warning(disable:4838)
//#pragma warning(disable:4458)

#include <new>
#include <memory>
#include <vector>
#include <algorithm>

#include <assert.h>

//#include <WinSock2.h>
#include <windows.h>

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>

#include <DirectXMath.h>
//#pragma warning(default:4838)


// private
#include "namespace gen/gen_data.h"
#include "namespace win/win_genData.h"
#include "namespace win/FileLogger.h"
//#pragma warning(pop)
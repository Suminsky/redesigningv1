// standard includes

#ifdef NDEBUG
	#define _HAS_EXCEPTIONS 0
#endif
#include <new>
#include <memory>
#include <vector>

#include <assert.h>

#include <windows.h>

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

// private
#include "namespace gen/gen_data.h"
#include "namespace win/win_genData.h"
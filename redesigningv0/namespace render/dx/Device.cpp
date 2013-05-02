#include "Device.h"

#include "../../namespace gen/gen_macros.h"

using namespace dx;

void Device::InitDevice( /*in: */ IDXGIAdapter1 *pAdapter_p, const BOOL bSingleThreaded_p, /*in/holded: */ IDXGIFactory1 *pFactory_p, /*not used only holded */ const D3D_FEATURE_LEVEL *pFeatureLVLSrequested_p, const UINT nFeatures_p )
{
	//create a device(used for resource creation) with the adapter gattered previously, with the high feature level supported:

	HRESULT err = D3D11CreateDevice(	pAdapter_p,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		(bSingleThreaded_p? D3D11_CREATE_DEVICE_SINGLETHREADED : 0) DBG(| D3D11_CREATE_DEVICE_DEBUG ),
		pFeatureLVLSrequested_p, nFeatures_p,
		D3D11_SDK_VERSION,
		&m_pDevice,
		&m_featureLVLavailable,
		&m_pContext );

	//check the matter of the error, if its internal, throw an exception:

	if( err != S_OK ){

		assert(err != D3DERR_INVALIDCALL);
		assert(err != E_INVALIDARG);

		throw std::exception("failed on dx device creation");	
	}

	m_pFactory = pFactory_p;
}

void Device::ExecutePipelineCommands( const commandbuffer & commandList_p )
{
	commandbuffer::const_iterator end = commandList_p.end();

	for( commandbuffer::const_iterator it = commandList_p.begin();
		it != end;
		++it ){

			(*it)->Execute( m_pContext );
	}
}
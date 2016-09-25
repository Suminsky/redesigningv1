#include "PCHF.h"
#include "BlendStateBinders.h"

void sprite::BlendStateBinders::Init( dx::Device * pDevice_p )
{
	dx::BlendState::CreationParams params = {0};
	params.desc.blendDesc.AlphaToCoverageEnable = false;
	params.desc.blendDesc.IndependentBlendEnable = false;
	params.desc.blendDesc.RenderTarget[0].BlendEnable = true;

	// alpha blend:
	//final rgb:
	//alpha blending = (src.rgb * src.a) + (dest.rgb * (1-dest.a))
	params.desc.blendDesc.RenderTarget[0].BlendOp			= D3D11_BLEND_OP_ADD;
	params.desc.blendDesc.RenderTarget[0].SrcBlend			= D3D11_BLEND_SRC_ALPHA;
	params.desc.blendDesc.RenderTarget[0].DestBlend			= D3D11_BLEND_INV_SRC_ALPHA;
	//final alpha:
	//alpha blending = (src.a * 1) + (dest.a * 1)
	params.desc.blendDesc.RenderTarget[0].BlendOpAlpha		= D3D11_BLEND_OP_ADD;
	params.desc.blendDesc.RenderTarget[0].SrcBlendAlpha		= D3D11_BLEND_ONE;
	params.desc.blendDesc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ONE;

	params.desc.blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // witch component can be write

	ID3D11BlendState * pBlendState = NULL;
	pDevice_p->m_pCacheBlendState->Acquire( params, pBlendState);

	m_cache[E_BLEND_ALPHA_BLENDED].Init(pBlendState, NULL);

	// additive blend:
	//final rgb:
	//alpha blending = (src.rgb * src.a) + (dest.rgb * 1)
	params.desc.blendDesc.RenderTarget[0].BlendOp			= D3D11_BLEND_OP_ADD;
	params.desc.blendDesc.RenderTarget[0].SrcBlend			= D3D11_BLEND_SRC_ALPHA;
	params.desc.blendDesc.RenderTarget[0].DestBlend			= D3D11_BLEND_ONE;
	//final alpha:
	//alpha blending = (src.a * 1) + (dest.a * 1)
	params.desc.blendDesc.RenderTarget[0].BlendOpAlpha		= D3D11_BLEND_OP_ADD;
	params.desc.blendDesc.RenderTarget[0].SrcBlendAlpha		= D3D11_BLEND_ONE;
	params.desc.blendDesc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ONE;

	params.desc.blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // witch component can be write

	pBlendState = NULL;
	pDevice_p->m_pCacheBlendState->Acquire( params, pBlendState);

	m_cache[E_BLEND_ADDITIVE].Init(pBlendState, NULL);

	// none default blend:
	m_cache[E_BLEND_NONE_DEFAULT].Init(NULL, NULL);
}

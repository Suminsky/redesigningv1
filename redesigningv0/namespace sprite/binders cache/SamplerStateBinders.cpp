#include "PCHF.h"

#include "SamplerStateBinders.h"

void sprite::SamplerStateBinders::Init( dx::Device * pDevice_p )
{
	// none

	dx::SamplerState::CreationParams params;
	ZeroMemory(&params, sizeof(dx::SamplerState::CreationParams));
	params.desc.samplerDesc.Filter =  D3D11_FILTER_MIN_MAG_MIP_POINT;
	params.desc.samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP ;
	params.desc.samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP ;
	params.desc.samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP ;
	params.desc.samplerDesc.MaxAnisotropy = 1;//16;
	params.desc.samplerDesc.MaxLOD = 0;//D3D11_FLOAT32_MAX; // TODO : not allowing mipmap cause the cards look better without!!

	ID3D11SamplerState * pSampler = NULL;
	pDevice_p->m_pCacheSamplerState->Acquire( params, pSampler );

	m_cache[E_SAMPLER_NONE].Init(0, pSampler);

	// linear

	params.desc.samplerDesc.Filter =  D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	params.desc.samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP ;
	params.desc.samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP ;
	params.desc.samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP ;
	params.desc.samplerDesc.MaxAnisotropy = 1;//16;//
	params.desc.samplerDesc.MaxLOD = 0;//D3D11_FLOAT32_MAX; // TODO

	pSampler = NULL;
	pDevice_p->m_pCacheSamplerState->Acquire( params, pSampler );

	m_cache[E_SAMPLER_LINEAR].Init(0, pSampler);
}

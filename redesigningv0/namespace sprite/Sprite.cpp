#include "Sprite.h"
#include "SpriteRenderer.h"

using namespace sprite;
using namespace DirectX;

Sprite::Sprite( dx::Device * pDevice_p,
				const char * szTexture_p, 
				float fWidth_p, float fHeight_p,
				float fUVOffset_U_p, float fUVOffset_V_p,
				E_BLENDTYPE blendType_p, E_SAMPLERTYPE sampler_p,
				SpriteRenderer * pSpriteRenderer_p )
:m_currentPermutation(0), m_iCurrentPermutationIndex(0)
{

	m_renderData.m_mWorld = XMMatrixIdentity();

	m_renderData.m_res.x = fWidth_p;
	m_renderData.m_res.y = fHeight_p;
	m_renderData.m_uvRect.x = fUVOffset_U_p;
	m_renderData.m_uvRect.y = fUVOffset_V_p;

	// creates a ID3DBuffer for the vs constant buffer

	dx::BufferResource::CreationParams cbufferParams;
	ZeroMemory(&cbufferParams, sizeof(dx::BufferResource::CreationParams));
	cbufferParams.desc.bufferDesc.ByteWidth = DrawableCbuffer::s_SIZE;
	cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferParams.desc.bufferDesc.CPUAccessFlags = 0;
	cbufferParams.desc.bufferDesc.StructureByteStride = 0;
	cbufferParams.desc.bufferDesc.MiscFlags = 0;

	ID3D11Buffer * pBuffer = NULL;
	pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, pBuffer );

	// initialize pipe state for this sprite

	m_VSDrawableCbufferBinder.Initialize( pBuffer, &m_renderData );

	m_pipeState.AddBinderCommand( &m_VSDrawableCbufferBinder );

	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_tex2D_cache.Get(szTexture_p) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_samplers_cache.GetSamplerBind(sampler_p) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_blends_cache.GetBlendBind(blendType_p) );
}

void sprite::Sprite::Update( double dInterpolation_p )
{
	// interpolate

	DirectX::XMMATRIX && mNewWorld = m_trafo.ComputeRenderWorldMatrix( (float)dInterpolation_p );

	if( memcmp( &m_renderData.m_mWorld, &mNewWorld, sizeof(DirectX::XMMATRIX)) ){

		m_renderData.m_mWorld = mNewWorld;

		// send to GPU?
		m_renderData.m_bUpdate = true;
	}
}
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
	m_renderData.m_uvOffset.x = fUVOffset_U_p;
	m_renderData.m_uvOffset.y = fUVOffset_V_p;

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

	m_pipeState.AddBinderCommand( new BindVSDrawableCBuffer(pBuffer, shared_DrawableCbuffer_ptr(&m_renderData, &gen::NoOp<DrawableCbuffer>) )  );

	m_pipeState.AddBinderCommand( dx::shared_Binder_ptr(&pSpriteRenderer_p->m_texs.Get(szTexture_p), &gen::NoOp<dx::Binder>  ));
	m_pipeState.AddBinderCommand( dx::shared_Binder_ptr(&pSpriteRenderer_p->m_samplers.GetSamplerBind(sampler_p), &gen::NoOp<dx::Binder>  ));
	m_pipeState.AddBinderCommand( dx::shared_Binder_ptr(&pSpriteRenderer_p->m_blends.GetBlendBind(blendType_p), &gen::NoOp<dx::Binder>  ));
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
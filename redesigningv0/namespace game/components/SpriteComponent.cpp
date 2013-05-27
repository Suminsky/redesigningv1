#include "SpriteComponent.h"


using namespace std;
using namespace gen;
using namespace game;
using namespace sprite;
using namespace dx;
using namespace DirectX;

game::SpriteComponent::SpriteComponent(	Device * pDevice_p,
										const char * szTexture_p, float fWidth_p, float fHeight_p, XMFLOAT4 uvRect_p,
										E_BLENDTYPE blendType_p, E_SAMPLERTYPE sampler_p,
										SpriteRenderer * pSpriteRenderer_p,
										const shared_MovableComponent_ptr & pMovable_p )
{
	m_pMovableRef = pMovable_p;
	m_sortKey.intRepresentation = 0;

	m_TextureID= m_BlendModeID= m_FilterModeID= m_ShaderID = 21;
	m_pSpriteRendererRef = pSpriteRenderer_p;
	m_iShaderPermutation = 0;

	m_renderData.m_mWorld = XMMatrixIdentity();

	m_renderData.m_res.x = fWidth_p;
	m_renderData.m_res.y = fHeight_p;
	m_renderData.m_uvRect = uvRect_p;
	m_renderData.m_bUpdate = true;
	m_renderData.m_color.x = m_renderData.m_color.y = m_renderData.m_color.z = m_renderData.m_color.w = 1.0f;

	// creates a ID3DBuffer for the vs constant buffer

	BufferResource::CreationParams cbufferParams;
	ZeroMemory(&cbufferParams, sizeof(BufferResource::CreationParams));
	cbufferParams.desc.bufferDesc.ByteWidth = DrawableCbuffer::s_SIZE;
	cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferParams.desc.bufferDesc.CPUAccessFlags = 0;
	cbufferParams.desc.bufferDesc.StructureByteStride = 0;
	cbufferParams.desc.bufferDesc.MiscFlags = 0;

	ID3D11Buffer * pBuffer = NULL;
	pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, pBuffer );

	// initialize pipe state for this sprite

	m_pipeState.AddBinderCommand( make_shared<BindVSDrawableCBuffer>(pBuffer, shared_DrawableCbuffer_ptr(&m_renderData, &NoOp<DrawableCbuffer>)) );

	m_pipeState.AddBinderCommand( shared_Binder_ptr(&pSpriteRenderer_p->m_texs.Get(szTexture_p), &NoOp<Binder>  ));
	m_pipeState.AddBinderCommand( shared_Binder_ptr(&pSpriteRenderer_p->m_samplers.GetSamplerBind(sampler_p), &NoOp<Binder>  ));
	m_pipeState.AddBinderCommand( shared_Binder_ptr(&pSpriteRenderer_p->m_blends.GetBlendBind(blendType_p), &NoOp<Binder>  ));
}

void game::SpriteComponent::OnDraw( double dInterpolation_p )
{
	// interpolate
	if( shared_MovableComponent_ptr pMovable = m_pMovableRef.lock() ){

		XMMATRIX && mNewWorld = pMovable->ComputeRenderWorldMatrix( (float)dInterpolation_p );

		if( memcmp( &m_renderData.m_mWorld, &mNewWorld, sizeof(XMMATRIX)) ){

			m_renderData.m_mWorld = mNewWorld;

			// send to GPU?
			m_renderData.m_bUpdate = true;
		}
		//DBG(m_renderData.m_bUpdate = true;)
		//m_pipeState.m_binds

		//---
	}
	//m_renderData.m_bUpdate = true;
	m_pSpriteRendererRef->Render(this);
}

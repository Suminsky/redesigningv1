#include "Camera.h"

#include "../namespace gen/gen_data.h"

using namespace sprite;
using namespace DirectX;

void sprite::Camera::BuildPipeState( UINT width_p, UINT height_p, dx::Device * pDevice_p, ID3D11RenderTargetView * pRTV_p )
{
	m_pipeState.m_binds.resize(0);

	// viewport

	m_viewPort.Width = (float)width_p;///2.0f;
	m_viewPort.Height = (float)height_p;///2.0f;
	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	m_pipeState.AddBinderCommand( std::make_shared<dx::BindRSViewPort>(m_viewPort));

	// cbuffer

	m_renderData.m_mViewProjection = m_mProjection = DirectX::XMMatrixOrthographicLH( (float)width_p, (float)height_p, 0.0f, 1.0f );

	dx::BufferResource::CreationParams params = {0};
	params.desc.bufferDesc.ByteWidth = CameraCbuffer::s_SIZE;
	params.desc.bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	params.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	
	pDevice_p->m_pCacheBuffer->Acquire( params, m_pBuffer );
	m_pipeState.AddBinderCommand( std::make_shared<BindVSCameraCBuffer>( m_pBuffer, shared_CameraCbuffer_ptr(&m_renderData, &gen::NoOp<CameraCbuffer>)) );

	// render target

	m_pipeState.AddBinderCommand( std::make_shared< dx::BindOMRenderTargetDepthStencil>(pRTV_p, nullptr ) );
}

void sprite::Camera::Update()
{
	XMMATRIX && mNewViewProj = m_mView * m_mProjection;

	if( memcmp( &m_renderData.m_mViewProjection, &mNewViewProj, sizeof(DirectX::XMMATRIX)) ){
	
		m_renderData.m_mViewProjection = mNewViewProj;

		// send to GPU?
		m_renderData.m_bUpdate = true;
	}
}
#include "DrawableCbuffer.h"

using namespace sprite;

sprite::DrawableCbuffer::DrawableCbuffer( float width_p, float height_p, float uvOffsetU_p, float uvOffsetV_p, DirectX::CXMMATRIX mWorld_p )
{
	m_bUpdate = true;
	m_res.x = width_p;
	m_res.y = height_p;
	m_uvRect.x = uvOffsetU_p;
	m_uvRect.y = uvOffsetV_p;
	m_mWorld = mWorld_p;
}

//========================================================================
// 
//========================================================================

ID3D11Buffer * BindVSDrawableCBuffer::s_pConstantBufferBound = nullptr;
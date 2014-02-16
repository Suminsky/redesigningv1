#include <algorithm>
#include "InstancedSprites.h"
#include "../namespace game/components/SpriteComponent_.h"

using namespace sprite;


sprite::InstancesVertexBuffer::InstancesVertexBuffer( UINT vbMaxInstances_p, UINT vbSlot_p, ID3D11Buffer * pVB_p, UINT vbOffset_p /*= 0 */ )
{
	Initialize( vbMaxInstances_p, vbSlot_p, pVB_p, vbOffset_p );
}

void sprite::InstancesVertexBuffer::Initialize( UINT vbMaxInstances_p, UINT vbSlot_p, ID3D11Buffer * pVB_p, UINT vbOffset_p /*= 0 */ )
{
	m_firstAvailable = 0;
	m_size = vbMaxInstances_p;
	m_vSpriteInstances.resize( vbMaxInstances_p );
	m_vSortable.resize( vbMaxInstances_p );
	m_vSpriteInstancesTmp.resize( vbMaxInstances_p );

	m_bLocked = false;
	m_bindVB.Initialize( vbSlot_p, pVB_p, sizeof(spriteInstance), vbOffset_p );
}

void sprite::InstancesVertexBuffer::StartBuffering()
{
	assert( !m_bLocked );

	m_iBufferingAt = 0;
	m_bLocked = true;
}

void sprite::InstancesVertexBuffer::BufferNewInstance( const spriteInstance & spriteData_p, UINT64 sortKey_p )
{
	m_vSpriteInstances[m_iBufferingAt] = spriteData_p;

	Entry entry = { sortKey_p, m_iBufferingAt };
	m_vSortable[m_iBufferingAt] = entry;

	++m_iBufferingAt;
	assert( m_iBufferingAt < m_size );

	++m_firstAvailable;
	if( m_firstAvailable == m_size ){

		// warp
		m_firstAvailable = 0;
	}
}

void sprite::InstancesVertexBuffer::BufferNewInstance( const spriteInstance & spriteData_p )
{
	m_vSpriteInstances[m_iBufferingAt] = spriteData_p;

	++m_iBufferingAt;
	assert( m_iBufferingAt < m_size );

	++m_firstAvailable;
	if( m_firstAvailable == m_size ){

		// warp
		m_firstAvailable = 0;
	}
}

void sprite::InstancesVertexBuffer::MapInstancesToVB( dx::Device * pDevice_p )
{
	// sort instance data

	std::sort( m_vSortable.begin(),  m_vSortable.begin() + m_iBufferingAt, Entry() );

	// now re-order vector

	size_t sizeBytes = sizeof(spriteInstance)*(m_iBufferingAt);

	//m_vSpriteInstancesTmp = m_vSpriteInstances;
	memcpy( (void*)(m_vSpriteInstancesTmp.data()), (void*)(m_vSpriteInstances.data()), sizeBytes );
	for( UINT it = 0; it < m_iBufferingAt; ++it ){

		m_vSpriteInstances[it] = m_vSpriteInstancesTmp[ m_vSortable[it].index ];
	}

	// map data to vertex buffer

	D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
	pDevice_p->GetContext()->Map( m_bindVB.GetBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped );
	memcpy( ( ((spriteInstance*)mapped.pData) + m_firstAvailable ), // offset on the VB
			(void*)m_vSpriteInstances.data(),						// from zero to current
			sizeBytes );
	pDevice_p->GetContext()->Unmap( m_bindVB.GetBuffer(), 0 );

	m_bLocked = false;
}

void sprite::InstancesVertexBuffer::MapInstancesToVB_NoSort( dx::Device * pDevice_p )
{
	// map data to vertex buffer

	size_t sizeBytes = sizeof(spriteInstance)*(m_iBufferingAt);

	D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
	pDevice_p->GetContext()->Map( m_bindVB.GetBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped );
	memcpy( ( ((spriteInstance*)mapped.pData) + m_firstAvailable ), // offset on the VB
			(void*)m_vSpriteInstances.data(),						// from zero to current
			sizeBytes );
	pDevice_p->GetContext()->Unmap( m_bindVB.GetBuffer(), 0 );

	m_bLocked = false;
}
//========================================================================
// 
//========================================================================
//======================================================================== 
//========================================================================
// 
//========================================================================

sprite::InstancedSprites::InstancedSprites()
	:
m_drawCall(6),
m_drawCall_warpException(6),
m_nWarpedAt(0)
{
	m_drawable.AddPipelineState( &m_pipeState );
	m_drawable.SetDrawCall( &m_drawCall );

	m_drawable_warpException.AddPipelineState( &m_pipeState );
	m_drawable_warpException.SetDrawCall( &m_drawCall );
}

void sprite::InstancedSprites::Initialize( dx::BindPSShaderResourceView & pTextureBinder_p, dx::BindOMBlendState & pBlendBinder, dx::BindPSSampler & pSampleState, InstancesVertexBuffer & pIVB_p )
{
	m_pipeState.Reset();
	m_pipeState.AddBinderCommand( &pTextureBinder_p );
	m_pipeState.AddBinderCommand( &pBlendBinder );
	m_pipeState.AddBinderCommand( &pSampleState );
	m_pipeState.AddBinderCommand( pIVB_p.GetBinder() );
	// missing, IA and camera binds

	m_pIVBref = &pIVB_p;
}

void sprite::InstancedSprites::StartBufferingInstances()
{
	m_pIVBref->StartBuffering();
	m_drawCall.UpdateInstanceOffset( m_pIVBref->GetCurrentVBOffset() );
	m_drawCall.UpdateNumberOfInstances( 0 );
	m_drawCall_warpException.UpdateNumberOfInstances( 0 );
}

void sprite::InstancedSprites::AddInstance( const spriteInstance & spriteData_p, UINT64 sortKey_p /*= 0 */ )
{
	m_pIVBref->BufferNewInstance( spriteData_p, sortKey_p );
	m_drawCall.UpdateNumberOfInstances( m_drawCall.GetNInstances() + 1 );

	// check if warped
	if( m_pIVBref->GetCurrentVBOffset() < m_drawCall.GetInstanceOffset() ){

		m_drawCall_warpException.UpdateInstanceOffset( m_pIVBref->GetCurrentVBOffset() );
		//m_drawCall_warpException.UpdateNumberOfInstances( 0 );

		// used for distribute amount over the 2 draw calls
		m_nWarpedAt = m_drawCall.GetNInstances();
	}
}

void sprite::InstancedSprites::AddInstance( const spriteInstance & spriteData_p )
{
	m_pIVBref->BufferNewInstance( spriteData_p );
	m_drawCall.UpdateNumberOfInstances( m_drawCall.GetNInstances() + 1 );

	// check if warped
	if( m_pIVBref->GetCurrentVBOffset() < m_drawCall.GetInstanceOffset() ){

		m_drawCall_warpException.UpdateInstanceOffset( m_pIVBref->GetCurrentVBOffset() );
		//m_drawCall_warpException.UpdateNumberOfInstances( 0 );

		// used for distribute amount over the 2 draw calls
		m_nWarpedAt = m_drawCall.GetNInstances();
	}
}

void sprite::InstancedSprites::AddInstance( game::SpriteComponent_ *pSprite_p )
{
	spriteInstance instanceData;
	memcpy( (void*)&instanceData, (void*)&pSprite_p->m_renderData, sizeof(float)*28);

	AddInstance( instanceData, pSprite_p->m_sortKey.intRepresentation );
}

void sprite::InstancedSprites::AddInstance_NoSort( game::SpriteComponent_ *pSprite_p )
{
	spriteInstance instanceData;
	memcpy( (void*)&instanceData, (void*)&pSprite_p->m_renderData, sizeof(float)*28);

	AddInstance( instanceData );
}

void sprite::InstancedSprites::ConcludeInstancesBuffering( dx::Device * pDevice_p, UINT64 sortKey_p )
{
	assert( m_drawCall.GetNInstances() );

	if( m_nWarpedAt ){

		// need to distribute amount of instances between draw calls
		m_drawCall_warpException.UpdateNumberOfInstances( m_drawCall.GetNInstances() - m_nWarpedAt );
		m_drawCall.UpdateNumberOfInstances( m_nWarpedAt );

		m_drawable_warpException.SetSortKey( sortKey_p );

		m_nWarpedAt = 0;
	}

	m_drawable.SetSortKey( sortKey_p );

	m_pIVBref->MapInstancesToVB( pDevice_p );

	//m_queue.Submit( m_drawableInstances );
}

void sprite::InstancedSprites::ConcludeInstancesBuffering_NoSort( dx::Device * pDevice_p, UINT64 sortKey_p )
{
	assert( m_drawCall.GetNInstances() );

	if( m_nWarpedAt ){

		// need to distribute amount of instances between draw calls
		m_drawCall_warpException.UpdateNumberOfInstances( m_drawCall.GetNInstances() - m_nWarpedAt );
		m_drawCall.UpdateNumberOfInstances( m_nWarpedAt );

		m_drawable_warpException.SetSortKey( sortKey_p );

		m_nWarpedAt = 0;
	}

	m_drawable.SetSortKey( sortKey_p );

	m_pIVBref->MapInstancesToVB_NoSort( pDevice_p );

	//m_queue.Submit( m_drawableInstances );
}

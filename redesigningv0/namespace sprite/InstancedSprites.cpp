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
	m_iVBfirstAvailable = 0;
	m_size = vbMaxInstances_p;
	m_nWarpedAt = 0;
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
	Entry entry = { sortKey_p, m_iBufferingAt };
	m_vSortable[m_iBufferingAt] = entry;

	BufferNewInstance( spriteData_p );
}

void sprite::InstancesVertexBuffer::BufferNewInstance( const spriteInstance & spriteData_p )
{
	m_vSpriteInstances[m_iBufferingAt] = spriteData_p;

	++m_iBufferingAt;
	assert( m_iBufferingAt < m_size );

	if( m_iVBfirstAvailable + m_iBufferingAt == m_size ){

		// warp

		m_nWarpedAt = m_iBufferingAt;
	}
}

void sprite::InstancesVertexBuffer::MapInstancesToVB( ID3D11DeviceContext * pDContext_p )
{
	// sort instance data

	std::sort( m_vSortable.begin(),  m_vSortable.begin() + m_iBufferingAt, Entry() );

	// now re-order vector

	size_t sizeBytes = sizeof(spriteInstance)*(m_iBufferingAt);

	memcpy( (void*)(m_vSpriteInstancesTmp.data()), (void*)(m_vSpriteInstances.data()), sizeBytes );
	for( UINT it = 0; it < m_iBufferingAt; ++it ){

		m_vSpriteInstances[it] = m_vSpriteInstancesTmp[ m_vSortable[it].index ];
	}

	// map data to vertex buffer

	MapInstancesToVB_NoSort( pDContext_p );
}

void sprite::InstancesVertexBuffer::MapInstancesToVB_NoSort( ID3D11DeviceContext * pDContext_p )
{
	size_t sizeBytes = sizeof(spriteInstance)*m_iBufferingAt;

	D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
	pDContext_p->Map( m_bindVB.GetBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped );

	// check if warped

	if( m_nWarpedAt ){

		size_t unwarpedSizeBytes = sizeof(spriteInstance)*m_nWarpedAt;

		memcpy( ( ((spriteInstance*)mapped.pData) + m_iVBfirstAvailable ),	// offset on the VB
				(void*)m_vSpriteInstances.data(),							// from zero to current
				unwarpedSizeBytes );

		sizeBytes -= unwarpedSizeBytes;
		m_iBufferingAt -= m_nWarpedAt; // used bellow to advance on the VB

		m_iVBfirstAvailable = 0;
	}

	memcpy( ( ((spriteInstance*)mapped.pData) + m_iVBfirstAvailable ),	// offset on the VB
			(void*)&m_vSpriteInstances[m_nWarpedAt],					// from zero to current
			sizeBytes );

	pDContext_p->Unmap( m_bindVB.GetBuffer(), 0 );

	// advance on the VB
	m_iVBfirstAvailable += m_iBufferingAt;

	m_nWarpedAt = 0;

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
m_drawCall_warpException(6)
{
	m_drawable.AddPipelineState( &m_pipeState );
	m_drawable.SetDrawCall( &m_drawCall );

	m_drawable_warpException.AddPipelineState( &m_pipeState );
	m_drawable_warpException.SetDrawCall( &m_drawCall_warpException );
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
	//m_drawCall.UpdateNumberOfInstances( 0 );
	m_drawCall_warpException.UpdateNumberOfInstances( 0 );
}

void sprite::InstancedSprites::AddInstance( const spriteInstance & spriteData_p, UINT64 sortKey_p /*= 0 */ )
{
	m_pIVBref->BufferNewInstance( spriteData_p, sortKey_p );

	AddInstance_Aux();
}

void sprite::InstancedSprites::AddInstance( const spriteInstance & spriteData_p )
{
	m_pIVBref->BufferNewInstance( spriteData_p );

	AddInstance_Aux();	
}

void sprite::InstancedSprites::AddInstance_Aux()
{
	// check if warped

	if( m_drawCall_warpException.GetNInstances() == 0	// if didn't warped already
		&&
		m_pIVBref->GetCurrentNBeforeWarp() ){

		m_drawCall_warpException.UpdateInstanceOffset( m_pIVBref->GetCurrentVBOffset() );
		m_drawCall_warpException.UpdateNumberOfInstances( m_pIVBref->GetCurrentNBeforeWarp() );

		m_drawCall.UpdateInstanceOffset( m_pIVBref->GetWarpOffset() );
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

void sprite::InstancedSprites::ConcludeInstancesBuffering( ID3D11DeviceContext * pDContext_p, UINT64 sortKey_p )
{
	ConcludeInstancesBuffering_Aux( sortKey_p );

	m_pIVBref->MapInstancesToVB( pDContext_p );

	//m_queue.Submit( m_drawableInstances );
}

void sprite::InstancedSprites::ConcludeInstancesBuffering_NoSort( ID3D11DeviceContext * pDContext_p, UINT64 sortKey_p )
{
	ConcludeInstancesBuffering_Aux( sortKey_p );

	m_pIVBref->MapInstancesToVB_NoSort( pDContext_p );

	//m_queue.Submit( m_drawableInstances );
}

void sprite::InstancedSprites::ConcludeInstancesBuffering_Aux( UINT64 sortKey_p )
{
	assert( m_pIVBref->GetCurrentNBuffered() );

	m_drawCall.UpdateNumberOfInstances( m_pIVBref->GetCurrentNBuffered() - m_pIVBref->GetCurrentNBeforeWarp() );
	m_drawable.SetSortKey( sortKey_p );
	m_drawable_warpException.SetSortKey( sortKey_p );
}
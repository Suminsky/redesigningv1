#include "PCHF.h"
#include "SpriteTextSys.h"

#include "../Layer.h"
#include "../components/SpriteTextCompo.h"
#include "../../namespace sprite/text/SpriteTextComposer.h"
#include "../../namespace sprite/Camera.h"
#include "../../namespace phys/AABB2D.h"
#include "../../namespace sprite/SpriteRenderer.h"
#include "../../namespace render/Drawable.h"

using namespace game;
using namespace DirectX;
using namespace dx;
using namespace render;
using namespace sprite;

void SpriteTextSys::VOnInit()
{
	shared_SpriteTextCompoFactory_ptr pTextFactory = std::static_pointer_cast<SpriteTextCompoFactory>(
		m_pSysMachineOwner->GetLayer()->m_componentFactory.GetComponentFactory<SpriteTextCompo>()
		);

	m_poolAccess = pTextFactory->m_pool.GetAccess();
	m_pTextCompoFactoryRef = pTextFactory.get();

	m_sortArray.reserve(m_poolAccess.GetNAllocated());

	m_pipeState_instVB.AddBinderCommand(&m_pTextCompoFactoryRef->m_instVBBinder);
	m_drawableAux.Initialize(m_drawablePipeMemBuff.StackAlloc(2u), 2u);
	m_drawableAux.AddPipelineState(&m_pipeState_instVB);
}

void SpriteTextSys::VOnDraw( double interp_p )
{
	m_sortArray.clear();

	// create cam box

	float zoom = m_pCameraRef->m_fZoom * 0.5f; // halving on zoom (order of prod dont aff res)
	float camHalfW = m_pCameraRef->m_viewPort.Width * zoom;
	float camHalfH = m_pCameraRef->m_viewPort.Height * zoom;
	XMFLOAT2 camPos(
		-XMVectorGetX( m_pCameraRef->m_mView.r[3] ),
		-XMVectorGetY( m_pCameraRef->m_mView.r[3] )
		);

	phys::AABB2D camBox = { camPos, camHalfW, camHalfH };

	for( uint32_t itText = 0, iSize = m_poolAccess.GetNAllocated();
		 itText < iSize;
		 ++itText ){

		SpriteTextCompo & textCompo = (*m_poolAccess.GetAllocated( itText ));
		if( !textCompo.GetObjectOwner()->IsAttached() ) continue;

		textCompo.InterpolateWorld( interp_p );
		textCompo.InterpolateColor( interp_p );

		if( textCompo.m_pRenderData->color[3] > 0.0f ){ // cull invisible sprites

			if( textCompo.m_bUpdateBindData ){

				textCompo.UpdateInterpolatedRenderDataToAllInstances();
				textCompo.m_bUpdateBindData = false;
			}

			// create spritetext box

			float halfW = textCompo.m_textRes.x * 0.5f;
			float halfH = textCompo.m_textRes.y * 0.5f;

			XMFLOAT2 pos( textCompo.m_pRenderData->mWorld[gen::array2Dto1D::indexFromXY(0,3,4)],
						  textCompo.m_pRenderData->mWorld[gen::array2Dto1D::indexFromXY(1,3,4)] );

			// centralize pos (as currently, pos is on the top left)

			pos.x += halfW;
			pos.y += halfH;

			phys::AABB2D textBox = { pos, halfW, halfH };

			// no per camera text...yet?
			
			if( textBox.IsColliding_e( camBox ) ){

				// so it needs rendering

				Entry entry = { textCompo.m_sortKey.intRepresentation, itText };
				m_sortArray.push_back( entry );
			}
		}
	}

	uint32_t nSorted = (uint32_t) m_sortArray.size();
	if( nSorted ){

		//PrepareSortedTexts_OptimizedForStaticLinearlyAllocated();
		PrepareSortedTexts_MappedPerText();
	}
}

void SpriteTextSys::SortTexts()
{
	auto smaller = []( const Entry & a, const Entry & b)->bool{

		return a.drawableKey < b.drawableKey;
	};

	std::sort( m_sortArray.begin(), m_sortArray.end(), smaller );
}

void SpriteTextSys::MapAllInstMemBlockToVB()
{
	ID3D11Buffer *& pVB = m_pTextCompoFactoryRef->m_pDxInstVBBuffer;

	D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
	m_pDevice->GetContext()->Map( pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped );

	const size_t sizeBytes = m_pTextCompoFactoryRef->E_MAXTEXTCHARS * sizeof(game::spriteInstance);

	memcpy( mapped.pData,
			(void*)&(m_pTextCompoFactoryRef->m_spriteInstStack[0]),	// from zero to current
			sizeBytes );

	m_pDevice->GetContext()->Unmap( pVB, 0 );
}

D3D11_MAPPED_SUBRESOURCE SpriteTextSys::StartMappingToVB()
{
	D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
	m_pDevice->GetContext()->Map(
		m_pTextCompoFactoryRef->m_pDxInstVBBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped );

	return mapped;
}

void SpriteTextSys::ConcludeMapping()
{
	m_pDevice->GetContext()->Unmap( m_pTextCompoFactoryRef->m_pDxInstVBBuffer, 0 );
}

void SpriteTextSys::PrepareSortedTexts_MappedPerText()
{
	SortTexts();

	D3D11_MAPPED_SUBRESOURCE mapped = StartMappingToVB();

	SpriteTextCompo & startingTextCompo = (*m_poolAccess.GetAllocated( m_sortArray[0].index ));
	
	uint64_t				currentKey =		startingTextCompo.m_sortKey.intRepresentation;
	DrawIndexedInstanced *	currentDrawCall =	&startingTextCompo.m_drawIndexedInstanced;
	currentDrawCall->UpdateInstanceOffset( 0 );
	uint32_t				currentNInstances = 0;
	m_drawableAux.AddPipelineState(&startingTextCompo.m_pipeState_tex_bs_ss);

	uint32_t nSorted = (uint32_t) m_sortArray.size();

	for( uint32_t it = 0; it < nSorted; ++it ){

		SpriteTextCompo & textCompo = (*m_poolAccess.GetAllocated( m_sortArray[it].index ));

		memcpy(	(void*)&((game::spriteInstance*)mapped.pData)[currentNInstances],
				(void*)(textCompo.m_pRenderData),
				sizeof(game::spriteInstance)*textCompo.m_nCurremtDrawnChars );

		// combine text compos that share the same sort key into a single drawable
		// (note that they now being copied linearly on the VB)

		if(	currentKey != textCompo.m_sortKey.intRepresentation){

			currentDrawCall->UpdateNumberOfInstances(currentNInstances);
			m_drawableAux.SetSortKey(currentKey);
			m_drawableAux.SetDrawCall(currentDrawCall);
			
			m_pSpriteRendererRef->Render( &m_drawableAux, m_pCameraRef );

			// update next drawable data

			currentKey = textCompo.m_sortKey.intRepresentation;
			currentDrawCall = &textCompo.m_drawIndexedInstanced;
			currentDrawCall->UpdateInstanceOffset( currentNInstances );
			currentNInstances = textCompo.m_nCurremtDrawnChars;

			m_drawableAux.PopLastPipelineState();
			m_drawableAux.AddPipelineState(&textCompo.m_pipeState_tex_bs_ss);
		}
		else
			currentNInstances += textCompo.m_nCurremtDrawnChars;
	}

	currentDrawCall->UpdateNumberOfInstances(currentNInstances);
	m_drawableAux.SetSortKey(currentKey);
	m_drawableAux.SetDrawCall(currentDrawCall);

	m_pSpriteRendererRef->Render( &m_drawableAux, m_pCameraRef );

	m_drawableAux.PopLastPipelineState();

	ConcludeMapping();
}

void SpriteTextSys::PrepareSortedTexts_OptimizedForStaticLinearlyAllocated()
{
	//MapAllInstMemBlockToVB();

	//SortTexts();

	//// create instanced drawables for all consecutive texts with the same key
	//// that make use of its entire instance span (sxLen == maxLen)
	//// 
	//// this works for static text only (never deleted, not dynamicaly created), cause they
	//// will always be consecutive on the mem block..but if you delete and create in rt you cant
	//// guarantee that..(so youd need to map the instances individually)

	//SpriteTextCompo & textCompo = (*m_poolAccess.GetAllocated( m_sortArray[0].index ));
	//uint64_t currentKey = textCompo.m_sortKey.intRepresentation;
	//DrawIndexedInstanced * currentDrawCall = &textCompo.m_drawIndexedInstanced;

	//uint32_t nSorted = (uint32_t) m_sortArray.size();
	//for( uint32_t it = 1; it < nSorted; ++it ){

	//	SpriteTextCompo & textCompo = (*m_poolAccess.GetAllocated( m_sortArray[it].index ));

	//	if( textCompo.m_nCurrentSzLen != textCompo.m_maxChars
	//		||
	//		currentKey != textCompo.m_sortKey.intRepresentation){

	//			//Drawable newDrawable;

	//			//TODO

	//			//m_pSpriteRendererRef->Render( , m_pCameraRef );

	//			currentKey = textCompo.m_sortKey.intRepresentation;
	//	}
	//}
}
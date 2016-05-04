#include "SpriteTextCompo.h"





#include "../Object.h"
#include "../../namespace sprite/text/SpriteTextComposer.h"
#include "../../namespace text/TextParser.h"
//#include "../namespace gen/gen_macros.h"

#include "ColorComponent.h"
#include "TransformComponent.h"
#include "SpriteComponent_.h"

using namespace game;
using namespace sprite;
using namespace text;
using namespace std;
using namespace DirectX;

game::SpriteTextCompo::SpriteTextCompo()
	:
m_drawIndexedInstanced(6)
{
	m_type = COMPONENT_TYPE(SpriteTextCompo);
}

void game::SpriteTextCompo::Initialize(
	wchar_t * pSzMemBlock_p, spriteInstance * pInstMemBlock_p,
	uint32_t maxChars_p, const wchar_t * szText_p,
	uint32_t fontID_p,
	SpriteTextComposer * pTextComposer_p,
	sprite::E_BLENDTYPE eBlend_p, sprite::E_SAMPLERTYPE eSampler_p,
	sprite::SpriteRenderer * pSpriteRenderer_p )
{
	m_szText = pSzMemBlock_p;
	m_pRenderData = pInstMemBlock_p;
	m_maxChars = maxChars_p;
	m_sortKey.intRepresentation = 0LL;

	m_nCurrentSzLen = gen::stringUtil::CountWString( szText_p, maxChars_p );
	memcpy( m_szText, szText_p, sizeof(wchar_t)*m_nCurrentSzLen );
	m_szText[m_nCurrentSzLen] = 0x0000;

	m_fontID = fontID_p;

	SetColor( XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );

	XMStoreFloat4x4( &m_currentTrafo, XMMatrixIdentity() );
	m_previousTrafo = m_currentTrafo;

	m_nCurremtDrawnChars = pTextComposer_p->Compose( m_szText, m_pRenderData, m_maxChars, m_fontID, m_textRes.x, m_textRes.y );

	m_sortKey.bitfield.textureID = pTextComposer_p->m_fonts.Get(m_fontID).GetTextureID();

	m_bUpdateBindData = true;

	//m_drawIndexedInstanced.UpdateNumberOfInstances( m_nCurrentSzLen ); // not really used

	m_pipeState_texture_blend_sampler.AddBinderCommand( &pSpriteRenderer_p->m_tex2D_cache.Get(m_sortKey.bitfield.textureID) );
	m_pipeState_texture_blend_sampler.AddBinderCommand( &pSpriteRenderer_p->m_blends_cache.GetBlendBind(eBlend_p) );
	m_pipeState_texture_blend_sampler.AddBinderCommand( &pSpriteRenderer_p->m_samplers_cache.GetSamplerBind(eSampler_p) );

	m_sortKey.bitfield.transparency = eBlend_p;
}


void game::SpriteTextCompo::SetText( const wchar_t * szNewText_p, sprite::SpriteTextComposer * pTextComposer_p )
{
	// TODO: skip if its the same text?
	m_nCurrentSzLen = gen::stringUtil::CountWString( szNewText_p, m_maxChars );
	
	assert( gen::stringUtil::CountWString(szNewText_p) <= m_maxChars );
	
	memcpy( m_szText, szNewText_p, sizeof(wchar_t)*m_nCurrentSzLen );
	m_szText[m_nCurrentSzLen] = 0x0000;

	m_nCurremtDrawnChars = pTextComposer_p->Compose( m_szText, m_pRenderData, m_maxChars, m_fontID, m_textRes.x, m_textRes.y );

	m_bUpdateBindData = true;
}


void game::SpriteTextCompo::SetColor( DirectX::XMFLOAT4 color_p )
{
	m_previousColor = m_currentColor = color_p;
}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void game::SpriteTextCompo::VOnAttach()
{
	EventMachine<ComponentEventData>::EventHandler colorDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteTextCompo, &SpriteTextCompo::OnColorEventDelegate>(this);
	m_pObjectOwner->RegisterForComponentEvent(colorDelegate, COMPONENT_TYPE(ColorComponent));

	EventMachine<ComponentEventData>::EventHandler trafoDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteTextCompo, &SpriteTextCompo::OnTransformEventDelegate>(this);
	m_pObjectOwner->RegisterForComponentEvent(trafoDelegate, COMPONENT_TYPE(TransformComponent));
}

void game::SpriteTextCompo::VOnDetach()
{
	EventMachine<ComponentEventData>::EventHandler colorDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteTextCompo, &SpriteTextCompo::OnColorEventDelegate>(this);
	m_pObjectOwner->UnregisterForComponentEvent(colorDelegate, COMPONENT_TYPE(ColorComponent));

	EventMachine<ComponentEventData>::EventHandler trafoDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteTextCompo, &SpriteTextCompo::OnTransformEventDelegate>(this);
	m_pObjectOwner->UnregisterForComponentEvent(trafoDelegate, COMPONENT_TYPE(TransformComponent));
}

void game::SpriteTextCompo::OnColorEventDelegate( const Event<ComponentEventData> & event_p )
{
	ColorComponent * pColor = event_p.GetDataAs<ColorComponent*>();

	m_currentColor = pColor->GetFinalColor();
	if( pColor->GonnaSnap() ) m_previousColor = m_currentColor;
	else m_previousColor = pColor->GetPreviousFinalColor();
}

void game::SpriteTextCompo::OnTransformEventDelegate( const Event<ComponentEventData> & event_p )
{
	TransformComponent * pTrafo = event_p.GetDataAs<TransformComponent*>();

	m_currentTrafo = pTrafo->GetFinal();
	if( pTrafo->GonnaSnap() ) m_previousTrafo = m_currentTrafo;
	else m_previousTrafo = pTrafo->GetPreviousFinal();
}

void game::SpriteTextCompo::InterpolateColor( double dInterp )
{
	XMFLOAT4 renderColor( m_pRenderData->color );
	XMVECTOR vRenderColor = XMLoadFloat4( &renderColor );
	XMVECTOR vCurrentColor = XMLoadFloat4( &m_currentColor );

	if( XMVector4NotEqual( vRenderColor, vCurrentColor )  ){

		XMVECTOR vPreviousColor = XMLoadFloat4( &m_previousColor );

		vCurrentColor = XMVectorLerp( vPreviousColor, vCurrentColor, (float)dInterp );

		XMStoreFloat4( &renderColor, vCurrentColor );

		m_pRenderData->color[0] = renderColor.x;
		m_pRenderData->color[1] = renderColor.y;
		m_pRenderData->color[2] = renderColor.z;
		m_pRenderData->color[3] = renderColor.w;

		m_bUpdateBindData = true;
	}
}

void game::SpriteTextCompo::InterpolateWorld( double dInterp_p )
{
	XMFLOAT4X4 renderTrafo(m_pRenderData->mWorld);
	XMMATRIX mRenderTrafo = XMLoadFloat4x4(&renderTrafo);
	XMMATRIX mCurrentTrafo = XMLoadFloat4x4( &m_currentTrafo );

	if( XMVector4NotEqual(mRenderTrafo.r[0], mCurrentTrafo.r[0])
		||
		XMVector4NotEqual(mRenderTrafo.r[1], mCurrentTrafo.r[1])
		||
		XMVector4NotEqual(mRenderTrafo.r[2], mCurrentTrafo.r[2])
		||
		XMVector4NotEqual(mRenderTrafo.r[3], mCurrentTrafo.r[3]) ){

		XMVECTOR vScale, qRot, vPos; 
		keepAssert( DirectX::XMMatrixDecompose( &vScale, &qRot, &vPos, mCurrentTrafo ) );

		XMMATRIX mPrevousTrafo = XMLoadFloat4x4( &m_previousTrafo );
		XMVECTOR vPrevScale, qPrevRot, vPrevPos;
		keepAssert( DirectX::XMMatrixDecompose( &vPrevScale, &qPrevRot, &vPrevPos, mPrevousTrafo ) );

		// interpolate independent components

		XMVECTOR vFactor = XMVectorReplicate( (float)dInterp_p); // do only once

		XMVECTOR vlPos =  XMVectorLerpV( vPrevPos, vPos, vFactor );
		XMVECTOR vlOrient = XMQuaternionSlerpV( qPrevRot, qRot, vFactor );
		XMVECTOR vlScale = XMVectorLerpV( vPrevScale, vScale, vFactor );

		// compose matrix again

		mRenderTrafo = XMMatrixAffineTransformation(vlScale, g_XMZero, vlOrient, vlPos );
		XMStoreFloat4x4( &renderTrafo, mRenderTrafo );

		memcpy(m_pRenderData->mWorld, &renderTrafo, sizeof(float)*16);

		// send to GPU?
		m_bUpdateBindData = true;
	}
}

void game::SpriteTextCompo::UpdateInterpolatedRenderDataToAllInstances()
{
	assert( sizeof(game::spriteInstance) == 112 );
	for( uint32_t it = 1; it < m_nCurrentSzLen; ++it ){

		//memcpy( m_pRenderData[it].mWorld, m_pRenderData[0].mWorld, sizeof(float)*16 );
		//memcpy( m_pRenderData[it].color, m_pRenderData[0].color, sizeof(float)*4 );
		memcpy( m_pRenderData[it].mWorld, m_pRenderData[0].mWorld, sizeof(float)*20 );
		
	}
}

//========================================================================
// 
//========================================================================

game::SpriteTextCompoFactory::~SpriteTextCompoFactory()
{
	if( m_pDxInstVBBuffer ) m_pDxInstVBBuffer->Release();
}

void game::SpriteTextCompoFactory::InitializeDxVBBuffer( dx::Device * pDevice_p )
{
	dx::BufferResource::CreationParams vbParams = {0};

	vbParams.desc.bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbParams.desc.bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbParams.desc.bufferDesc.ByteWidth = sizeof(spriteInstance)*E_MAXTEXTCHARS;
	vbParams.desc.bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//bufferData.pSysMem = nullptr;
	vbParams.pInitialData = nullptr;//&bufferData;

	m_pDxInstVBBuffer = dx::BufferResource::Create( pDevice_p->GetDevice(), vbParams );
	//pDevice_p->m_pCacheBuffer->Acquire(vbParams, m_pDxInstVBBuffer);
}

game::pool_Component_ptr game::SpriteTextCompoFactory::VCreateComponent( text::GfigElementA * pGFig_p )
{
	pool_SpriteTextCompo__ptr pText(m_pool);

	// font name

	uint32_t fontID;
	GfigElementA * pParam = nullptr;
	keepAssert( pGFig_p->GetSubElement( "font", pParam ) );
	m_pTextComposer->m_fonts.Get( pParam->m_value.c_str(), &fontID );

	// text

	keepAssert( pGFig_p->GetSubElement( "text", pParam ) );

	wstring szW( pParam->m_value.begin(), pParam->m_value.end() );

	// max len

	uint32_t maxLen;
	if( pGFig_p->GetSubElement( "max len", pParam ) ){

		maxLen = atoi( pParam->m_value.c_str() );
	}
	else{

		maxLen = (uint32_t)(pParam->m_value.length()+1);
	}

	E_BLENDTYPE eBlend = E_BLEND_ALPHA_BLENDED;
	if( pGFig_p->GetSubElement( "blend", pParam) ){

		eBlend = SpriteComponent_Factory::GetBlendType( pParam );
	}
	E_SAMPLERTYPE eSampler = E_SAMPLER_NONE;
	if( pGFig_p->GetSubElement( "sampler", pParam) ){

		eSampler = SpriteComponent_Factory::GetSamplerType( pParam );
	}

	pText->m_drawIndexedInstanced.UpdateInstanceOffset( m_spriteInstStack.Size() ); // very important

	pText->Initialize(
		m_szTextStack.StackAlloc(maxLen),
		m_spriteInstStack.StackAlloc(maxLen),
		maxLen,
		szW.c_str(),
		fontID,
		m_pTextComposer,
		eBlend, eSampler,
		m_pSpriteRenderer
		);

	if( pGFig_p->GetSubElement( "color", pParam ) ){

		ColorComponentFactory::UpdateRGBAFromGfig( pText->m_currentColor, pParam );

		pText->SetColor( pText->m_currentColor );
	}
	if( pGFig_p->GetSubElement( "d", pParam ) ){

		pText->m_sortKey.bitfield.Zdepth = atoi( pParam->m_value.c_str() );
	}

	return pText;
}

game::pool_Component_ptr game::SpriteTextCompoFactory::VCloneComponent( const Component * pCompo_p )
{
	pool_SpriteTextCompo__ptr pText(m_pool);
	SpriteTextCompo * pOther = (SpriteTextCompo*)pCompo_p;


	pText->m_currentTrafo = pOther->m_currentTrafo;
	pText->m_previousTrafo = pOther->m_previousTrafo;

	pText->m_currentColor = pOther->m_currentColor;
	pText->m_previousColor = pOther->m_previousColor;

	pText->m_fontID = pOther->m_fontID;

	pText->m_maxChars = pOther->m_maxChars;
	pText->m_nCurrentSzLen = pOther->m_nCurrentSzLen;

	pText->m_szText = m_szTextStack.StackAlloc(pText->m_maxChars);
	pText->m_pRenderData = m_spriteInstStack.StackAlloc(pText->m_maxChars);

	memcpy( pText->m_szText, pOther->m_szText, pText->m_nCurrentSzLen * sizeof(wchar_t) );
	memcpy( pText->m_pRenderData, pOther->m_pRenderData, pText->m_nCurrentSzLen * sizeof(spriteInstance) );

	return pText;
}

void game::SpriteTextCompoFactory::VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p )
{
	GfigElementA * pParam = nullptr;

	SpriteTextCompo *pText = (SpriteTextCompo*)pCompo_p;

	bool bNeedRecompose = false;

	// CANT update max len

	// text (must update before font)

	if( pGFig_p->GetSubElement( "text", pParam ) ){

		wstring szW( pParam->m_value.begin(), pParam->m_value.end() );
		
		pText->m_nCurrentSzLen = (uint32_t)szW.length();

		memcpy( pText->m_szText, szW.c_str(), pText->m_nCurrentSzLen * sizeof(wchar_t) );
		pText->m_szText[pText->m_nCurrentSzLen] = 0x0000;

		bNeedRecompose = true;
	}

	// font name

	if( pGFig_p->GetSubElement( "font", pParam ) ){

		m_pTextComposer->m_fonts.Get( pParam->m_value.c_str(), &pText->m_fontID );

		bNeedRecompose = true;
	}

	if( pGFig_p->GetSubElement( "color", pParam ) ){

		ColorComponentFactory::UpdateRGBAFromGfig( pText->m_currentColor, pParam );

		pText->SetColor( pText->m_currentColor );
	}


	if( bNeedRecompose ){

		m_pTextComposer->Compose( pText->m_szText, pText->m_pRenderData, pText->m_maxChars, pText->m_fontID );
	}	
}

void game::SpriteTextCompoFactory::VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p )
{
	// text
	// font
	// if maxChars != nSzCurrentLen
	// max len
	// if color != white
	// color
	
	SpriteTextCompo & text = *((SpriteTextCompo*)pCompo_p);

	pGFig_p->m_subElements.emplace_back( GfigElementA(COMPONENT_NAME(SpriteTextCompo)) );

	GfigElementA & gTextCompo = pGFig_p->m_subElements.back();
	{
		gTextCompo.m_subElements.emplace_back(
			GfigElementA("font", m_pTextComposer->m_fonts.GetFontName(text.m_fontID).c_str())
			);

		std::string szA( text.m_szText, text.m_szText+text.m_nCurrentSzLen );

		gTextCompo.m_subElements.emplace_back(
			GfigElementA("text", szA.c_str() )
			);

		if( text.m_maxChars != text.m_nCurrentSzLen ){

			gTextCompo.m_subElements.emplace_back(
				GfigElementA("max len", to_string((_ULonglong)text.m_maxChars).c_str() )
				);
		}

		GfigElementA gColor("color");
		ColorComponentFactory::SerializeRGBA(text.m_currentColor, gColor);
		if( gColor.m_subElements.size() ){

			gTextCompo.m_subElements.push_back(
				std::move(gColor)
				);
		}

		GfigElementA gBlend("blend");
		if(	SerializeBlendType((E_BLENDTYPE)text.m_sortKey.bitfield.transparency, gBlend ) ){

			gTextCompo.m_subElements.push_back(std::move(gBlend));
		}

		GfigElementA gSample("sampler");
		E_SAMPLERTYPE eSampler = m_pSpriteRenderer->m_samplers_cache.InfereSamplerType( (dx::BindPSSampler *)*(text.m_pipeState_texture_blend_sampler.Begin()+2));
		if( SpriteComponent_Factory::SerializeSamplerType( eSampler, gSample) ){

			gTextCompo.m_subElements.push_back(std::move(gSample));
		}

		if( text.m_sortKey.bitfield.Zdepth != (uint64_t)0 ){

			gTextCompo.m_subElements.push_back(
				GfigElementA("d", std::to_string( (_ULonglong)text.m_sortKey.bitfield.Zdepth).c_str() )
				);
		}
		
	}

}
bool game::SpriteTextCompoFactory::SerializeBlendType( const E_BLENDTYPE eBlend_p, text::GfigElementA & gFig_p )
{
	if( eBlend_p == E_BLEND_ALPHA_BLENDED ) return false;

	if( eBlend_p == E_BLEND_NONE_DEFAULT ){
		gFig_p.m_value = "none";
	}
	else if( eBlend_p == E_BLEND_ADDITIVE ){

		gFig_p.m_value = "add";
	}

	return true;
}
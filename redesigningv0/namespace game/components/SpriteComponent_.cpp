#include "SpriteComponent_.h"
#include "../Object.h"
#include "ColorComponent.h"
#include "TransformComponent.h"
#include "SpriteAnimationComponent.h"
#include "../../namespace text/TextParser.h"

using namespace std;
using namespace gen;
using namespace game;
using namespace sprite;
using namespace dx;
using namespace DirectX;
using namespace text;

game::SpriteComponent_::SpriteComponent_()
{
	m_type = COMPONENT_TYPE(SpriteComponent_);
}

SpriteComponent_::SpriteComponent_(	Device * pDevice_p,
	const char * szTexture_p, float fWidth_p, float fHeight_p, XMFLOAT4 uvRect_p,
	E_BLENDTYPE blendType_p, E_SAMPLERTYPE sampler_p,
	SpriteRenderer * pSpriteRenderer_p )
{
	m_type = COMPONENT_TYPE(SpriteComponent_);

	m_sortKey.intRepresentation = 0LL;

	m_BlendModeID= m_FilterModeID= m_ShaderID = 0;
	//m_pSpriteRendererRef = pSpriteRenderer_p;
	m_iShaderPermutation = 0;

	m_renderData.m_mWorld = XMMatrixIdentity();

	m_renderData.m_res.x = fWidth_p;
	m_renderData.m_res.y = fHeight_p;
	m_renderData.m_uvRect = uvRect_p;
	m_renderData.m_bUpdate = true;
	m_renderData.m_color.x = m_renderData.m_color.y = m_renderData.m_color.z = m_renderData.m_color.w = 1.0f;

	m_previousColor = m_currentColor = m_renderData.m_color;
	XMStoreFloat4x4( &m_currentTrafo, m_renderData.m_mWorld );
	m_previousTrafo = m_currentTrafo;

	// creates a ID3DBuffer for the vs constant buffer

	BufferResource::CreationParams cbufferParams;
	ZeroMemory(&cbufferParams, sizeof(BufferResource::CreationParams));
	cbufferParams.desc.bufferDesc.ByteWidth = DrawableCbuffer::s_SIZE;
	cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT; // 
	cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferParams.desc.bufferDesc.CPUAccessFlags =  D3D11_CPU_ACCESS_WRITE; //0; //
	cbufferParams.desc.bufferDesc.StructureByteStride = 0;
	cbufferParams.desc.bufferDesc.MiscFlags = 0;

	ID3D11Buffer * pBuffer = NULL;
	pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, pBuffer );
	//pDevice_p->GetDevice()->CreateBuffer( &cbufferParams.desc.bufferDesc, nullptr, &pBuffer );

	// initialize pipe state for this sprite
	m_VSDrawableCbufferBinder.Initialize( pBuffer, &m_renderData );
	m_pipeState.AddBinderCommand( &m_VSDrawableCbufferBinder );

	int iTextureID;
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_texs.Get(szTexture_p, &iTextureID) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_samplers.GetSamplerBind(sampler_p) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_blends.GetBlendBind(blendType_p) );

	m_TextureID = iTextureID;
	m_sortKey.bitfield.textureID = m_TextureID;
	m_sortKey.bitfield.transparency = blendType_p;
}

void SpriteComponent_::OnDraw( double dInterpolation_p )
{
	XMMATRIX mCurrentTrafo = XMLoadFloat4x4( &m_currentTrafo );
	if( memcmp( &m_renderData.m_mWorld, &mCurrentTrafo, sizeof(XMMATRIX)) ){

		// interpolate

		// decompose matrices

		Trafo mCurrent;
		mCurrent.FromMatrix(mCurrentTrafo);
		Trafo mPrevious;
		mPrevious.FromMatrix(m_previousTrafo);

		XMVECTOR vPos, vPrevPos, vScale, vPrevScale, qRot, qPrevRot;

		vPos = XMLoadFloat4( &mCurrent.position );
		qRot = XMLoadFloat4( &mCurrent.qRotation );
		vScale = XMLoadFloat4( &mCurrent.scale );
		//
		vPrevPos = XMLoadFloat4( &mPrevious.position );
		qPrevRot = XMLoadFloat4( &mPrevious.qRotation );
		vPrevScale = XMLoadFloat4( &mPrevious.scale );

		// interpolate independent components

		XMVECTOR vFactor = XMVectorReplicate( (float)dInterpolation_p); // do only once

		XMVECTOR vlPos =  XMVectorLerpV( vPrevPos, vPos, vFactor );
		XMVECTOR vlOrient = XMQuaternionSlerpV( qPrevRot, qRot, vFactor );
		XMVECTOR vlScale = XMVectorLerpV( vPrevScale, vScale, vFactor );

		// compose matrix again

		m_renderData.m_mWorld = XMMatrixAffineTransformation(vlScale, g_XMZero, vlOrient, vlPos );

		// send to GPU?
		m_renderData.m_bUpdate = true;
	}

	// interpolate color

	if( m_renderData.m_color.x != m_currentColor.x
		||
		m_renderData.m_color.y != m_currentColor.y
		||
		m_renderData.m_color.z != m_currentColor.z
		||
		m_renderData.m_color.w != m_currentColor.w  ){

			XMVECTOR colorPrevious = XMLoadFloat4( &m_previousColor );
			XMVECTOR colorCurrent = XMLoadFloat4( &m_currentColor );

			colorCurrent = XMVectorLerp( colorPrevious, colorCurrent, (float)dInterpolation_p );

			XMStoreFloat4( &m_renderData.m_color, colorCurrent );

			// send to GPU?
			m_renderData.m_bUpdate = true;
	}
}
void SpriteComponent_::OnDraw( double dInterpolation_p, Camera * /*pCamera_p*/ )
{
	XMMATRIX mCurrentTrafo = XMLoadFloat4x4( &m_currentTrafo );
	if( memcmp( &m_renderData.m_mWorld, &mCurrentTrafo, sizeof(XMMATRIX)) ){

		// interpolate

		Trafo mCurrent;
		mCurrent.FromMatrix(mCurrentTrafo);
		Trafo mPrevious;
		mPrevious.FromMatrix(m_previousTrafo);

		XMVECTOR vPos, vPrevPos, vScale, vPrevScale, qRot, qPrevRot;

		vPos = XMLoadFloat4( &mCurrent.position );
		qRot = XMLoadFloat4( &mCurrent.qRotation );
		vScale = XMLoadFloat4( &mCurrent.scale );

		vPrevPos = XMLoadFloat4( &mPrevious.position );
		vPrevScale = XMLoadFloat4( &mPrevious.scale );
		qPrevRot = XMLoadFloat4( &mPrevious.qRotation );


		XMVECTOR vFactor = XMVectorReplicate( (float)dInterpolation_p); // do only once

		XMVECTOR vlPos =  XMVectorLerpV( vPrevPos, vPos, vFactor );
		XMVECTOR vlScale = XMVectorLerpV( vPrevScale, vScale, vFactor );
		XMVECTOR vlOrient = XMQuaternionSlerpV( qPrevRot, qRot, vFactor );

		m_renderData.m_mWorld = XMMatrixAffineTransformation(vlScale, g_XMZero, vlOrient, vlPos );

		// send to GPU?
		m_renderData.m_bUpdate = true;
	}

	// interpolate color
	if( m_renderData.m_color.x != m_currentColor.x
		||
		m_renderData.m_color.y != m_currentColor.y
		||
		m_renderData.m_color.z != m_currentColor.z
		||
		m_renderData.m_color.w != m_currentColor.w ){

			XMVECTOR colorPrevious = XMLoadFloat4( &m_previousColor );
			XMVECTOR colorCurrent = XMLoadFloat4( &m_currentColor );

			colorCurrent = XMVectorLerp( colorPrevious, colorCurrent, (float)dInterpolation_p );

			XMStoreFloat4( &m_renderData.m_color, colorCurrent );

			// send to GPU?
			m_renderData.m_bUpdate = true;
	}
}

void SpriteComponent_::VOnAttach()
{
	//gen::Delegate1Param<const game::Event<InGameEventData>&> eventHandlerDelegate =
	//	gen::Delegate1Param<const game::Event<InGameEventData> &>::Build<CardsLayer, &CardsLayer::OnEvent>(this);

	EventMachine<ComponentEventData>::EventHandlerDelegate colorDelegate =
		EventMachine<ComponentEventData>::EventHandlerDelegate::Build<SpriteComponent_, &SpriteComponent_::OnColorEventDelegate>(this);
	m_pObjectOwner->RegisterForComponentEvent(colorDelegate, COMPONENT_TYPE(ColorComponent));

	EventMachine<ComponentEventData>::EventHandlerDelegate trafoDelegate =
		EventMachine<ComponentEventData>::EventHandlerDelegate::Build<SpriteComponent_, &SpriteComponent_::OnTransformEventDelegate>(this);
	m_pObjectOwner->RegisterForComponentEvent(trafoDelegate, COMPONENT_TYPE(TransformComponent));

	EventMachine<ComponentEventData>::EventHandlerDelegate animDelegate =
		EventMachine<ComponentEventData>::EventHandlerDelegate::Build<SpriteComponent_, &SpriteComponent_::OnAnimEventDelegate>(this);
	m_pObjectOwner->RegisterForComponentEvent(animDelegate, COMPONENT_TYPE(SpriteAnimationComponent));
}

void SpriteComponent_::OnColorEventDelegate( const Event<ComponentEventData> & event_p )
{
	ColorComponent * pColor = event_p.GetDataAs<ColorComponent*>();

	m_previousColor = pColor->GetPreviousFinalColor();
	m_currentColor = pColor->GetFinalColor();
}

void game::SpriteComponent_::OnTransformEventDelegate( const Event<ComponentEventData> & event_p )
{
	TransformComponent * pTrafo = event_p.GetDataAs<TransformComponent*>();
	
	m_previousTrafo = pTrafo->GetPreviousFinal();
	m_currentTrafo = pTrafo->GetFinal();
	if( pTrafo->GonnaSnap() ) m_previousTrafo = m_currentTrafo;
}

void game::SpriteComponent_::OnAnimEventDelegate( const Event<ComponentEventData> & event_p )
{
	SpriteAnimationComponent * pAnim = event_p.GetDataAs<SpriteAnimationComponent*>();

	SpriteFrame spriteFrame = pAnim->GetFrame();

	m_renderData.m_res.x = spriteFrame.fW;
	m_renderData.m_res.y = spriteFrame.fH;
	m_renderData.m_uvRect = spriteFrame.uvRect;
	m_renderData.m_padding.x = spriteFrame.xOffset;
	m_renderData.m_padding.y = spriteFrame.yOffset;
	m_renderData.m_bUpdate = true;

	(*(++m_pipeState.Begin())) = pAnim->GetSprite().pBindPSSRV;
	m_TextureID = pAnim->GetSprite().iID;
	m_sortKey.bitfield.textureID = m_TextureID;
}

void game::SpriteComponent_::SetColor( DirectX::XMFLOAT4 color_p )
{
	m_previousColor = m_currentColor = color_p;
}

void SpriteComponent_::Init( dx::Device * pDevice_p, const char * szTexture_p, float fWidth_p, float fHeight_p, DirectX::XMFLOAT4 uvRect_p, sprite::E_BLENDTYPE blendType_p, sprite::E_SAMPLERTYPE sampler_p, sprite::SpriteRenderer * pSpriteRenderer_p )
{
	m_sortKey.intRepresentation = 0LL;

	m_BlendModeID= m_FilterModeID= m_ShaderID = 0;
	m_iShaderPermutation = 0;

	m_renderData.m_mWorld = XMMatrixIdentity();

	m_renderData.m_res.x = fWidth_p;
	m_renderData.m_res.y = fHeight_p;
	m_renderData.m_uvRect = uvRect_p;
	m_renderData.m_bUpdate = true;
	m_renderData.m_color.x = m_renderData.m_color.y = m_renderData.m_color.z = m_renderData.m_color.w = 1.0f;

	m_previousColor = m_currentColor = m_renderData.m_color;
	XMStoreFloat4x4( &m_currentTrafo, m_renderData.m_mWorld );
	m_previousTrafo = m_currentTrafo;

	// creates a ID3DBuffer for the vs constant buffer

	BufferResource::CreationParams cbufferParams;
	ZeroMemory(&cbufferParams, sizeof(BufferResource::CreationParams));
	cbufferParams.desc.bufferDesc.ByteWidth = DrawableCbuffer::s_SIZE;
	cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT; // 
	cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferParams.desc.bufferDesc.CPUAccessFlags =  D3D11_CPU_ACCESS_WRITE; //0; //
	cbufferParams.desc.bufferDesc.StructureByteStride = 0;
	cbufferParams.desc.bufferDesc.MiscFlags = 0;

	ID3D11Buffer * pBuffer = NULL;
	pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, pBuffer );
	//pDevice_p->GetDevice()->CreateBuffer( &cbufferParams.desc.bufferDesc, nullptr, &pBuffer );

	// initialize pipe state for this sprite
	m_pipeState.Reset();
	m_VSDrawableCbufferBinder.Initialize( pBuffer, &m_renderData );
	m_pipeState.AddBinderCommand( &m_VSDrawableCbufferBinder );

	int iTextureID;
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_texs.Get(szTexture_p, &iTextureID) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_samplers.GetSamplerBind(sampler_p) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_blends.GetBlendBind(blendType_p) );

	m_TextureID = iTextureID;
	m_sortKey.bitfield.textureID = m_TextureID;
	m_sortKey.bitfield.transparency = blendType_p;
}
//========================================================================
// 
//========================================================================
pool_Component_ptr game::SpriteComponent_Factory::VCreateComponent( GfigElementA * pGFig_p )
{
	//SpriteComponent_ * pSprite = m_pool.Allocate();
	pool_SpriteCompo__ptr pSprite( m_pool );

	float w, h;
	XMFLOAT4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	sprite::E_BLENDTYPE eBlend = E_BLEND_NONE_DEFAULT;
	sprite::E_SAMPLERTYPE eSampler = E_SAMPLER_NONE;

	GfigElementA * pParam = nullptr;

	assert( pGFig_p->GetSubElement( "w", pParam ) );
	w = (float)atof( pParam->m_value.c_str() );
	assert( pGFig_p->GetSubElement( "h", pParam ) );
	h = (float)atof( pParam->m_value.c_str() );

	if( pGFig_p->GetSubElement( "uvRect", pParam ) ){

		uvRect = GetXYWH( pParam );
	}
	if( pGFig_p->GetSubElement( "blend", pParam ) ){

		eBlend = GetBlendType( pParam );
	}
	if( pGFig_p->GetSubElement( "sampler", pParam ) ){

		eSampler = GetSamplerType( pParam );
	}

	assert( pGFig_p->GetSubElement( "texture", pParam ) );

	pSprite->Init( m_pDeviceRef_p, pParam->m_value.c_str(), w, h, uvRect, eBlend, eSampler, m_pRendererRef );

	//return MAKE_STACK_SHAREDPTR( SpriteComponent_, pSprite );
	return pSprite;
}

DirectX::XMFLOAT4 game::SpriteComponent_Factory::GetXYWH( text::GfigElementA * pGFig_p )
{
	XMFLOAT4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	GfigElementA * pElement;

	if( pGFig_p->GetSubElement( "x", pElement) ){
		uvRect.x = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "y", pElement) ){
		uvRect.y = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "w", pElement) ){
		uvRect.z = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "h", pElement) ){
		uvRect.w = (float)atof(pElement->m_value.c_str());
	}

	return uvRect;
}

sprite::E_BLENDTYPE game::SpriteComponent_Factory::GetBlendType( text::GfigElementA * pGFig_p )
{
	sprite::E_BLENDTYPE eBlend = E_BLEND_NONE_DEFAULT;

	if( pGFig_p->m_value == "alpha" ){

		eBlend = E_BLEND_ALPHA_BLENDED;
	}
	else if( pGFig_p->m_value == "add" ){

		eBlend = E_BLEND_ADDITIVE;
	}

	return eBlend;
}

sprite::E_SAMPLERTYPE game::SpriteComponent_Factory::GetSamplerType( text::GfigElementA * pGFig_p )
{
	sprite::E_SAMPLERTYPE eSampler = E_SAMPLER_NONE;

	if( pGFig_p->m_value == "linear" ){

		eSampler = E_SAMPLER_LINEAR;
	}

	return eSampler;
}

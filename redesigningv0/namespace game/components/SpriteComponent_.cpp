#include "PCHF.h"
#include "SpriteComponent_.h"
#include "../Object.h"
#include "ColorComponent.h"
#include "TransformComponent.h"
#include "SpriteAnimationComponent.h"
#include "SpriteAnimCompo_.h"
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
	m_pBuffer = nullptr;
	m_type = COMPONENT_TYPE(SpriteComponent_);
	m_pCamera = nullptr;
}
SpriteComponent_::SpriteComponent_(
	Device * pDevice_p,
	const char * szTexture_p, float fWidth_p, float fHeight_p, XMFLOAT4 uvRect_p,
	E_BLENDTYPE blendType_p, E_SAMPLERTYPE sampler_p,
	SpriteRenderer * pSpriteRenderer_p )
{
	m_pBuffer = nullptr;
	m_type = COMPONENT_TYPE(SpriteComponent_);
	m_pCamera = nullptr;

	m_sortKey.intRepresentation = 0LL;

	m_ShaderID = 0;
	m_iShaderPermutation = 0;

	m_renderData.m_mWorld = XMMatrixIdentity();
	m_renderData.m_res.x = fWidth_p;
	m_renderData.m_res.y = fHeight_p;
	m_renderData.m_uvRect = uvRect_p;
	m_renderData.m_color.x = m_renderData.m_color.y = m_renderData.m_color.z = m_renderData.m_color.w = 1.0f;
	m_renderData.m_bUpdate = true;

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

	m_pBuffer = dx::BufferResource::Create( pDevice_p->GetDevice(), cbufferParams );

	// initialize pipe state for this sprite
	m_VSDrawableCbufferBinder.Initialize( m_pBuffer, &m_renderData );
	m_pipeState.AddBinderCommand( &m_VSDrawableCbufferBinder );

	unsigned int iTextureID;
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_tex2D_cache.Get(szTexture_p, &iTextureID) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_samplers_cache.GetSamplerBind(sampler_p) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_blends_cache.GetBlendBind(blendType_p) );

	m_TextureID = iTextureID;
	m_BlendModeID = blendType_p;
	m_SamplerModeID = sampler_p;
	m_sortKey.bitfield.textureID = m_TextureID;
	m_sortKey.bitfield.blending = blendType_p;
}

void SpriteComponent_::VOnAttach()
{
	EventMachine<ComponentEventData>::EventHandler colorDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteComponent_, &SpriteComponent_::OnColorEventDelegate>(this);
	m_pObjectOwner->RegisterForComponentEvent(colorDelegate, COMPONENT_TYPE(ColorComponent));

	EventMachine<ComponentEventData>::EventHandler trafoDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteComponent_, &SpriteComponent_::OnTransformEventDelegate>(this);
	m_pObjectOwner->RegisterForComponentEvent(trafoDelegate, COMPONENT_TYPE(TransformComponent));

	EventMachine<ComponentEventData>::EventHandler animDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteComponent_, &SpriteComponent_::OnAnimEventDelegate>(this);
	m_pObjectOwner->RegisterForComponentEvent(animDelegate, COMPONENT_TYPE(SpriteAnimationComponent));

	EventMachine<ComponentEventData>::EventHandler anim_Delegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteComponent_, &SpriteComponent_::OnAnim_EventDelegate>(this);
	m_pObjectOwner->RegisterForComponentEvent(anim_Delegate, COMPONENT_TYPE(SpriteAnimCompo_));
}
void game::SpriteComponent_::VOnDetach()
{
	EventMachine<ComponentEventData>::EventHandler colorDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteComponent_, &SpriteComponent_::OnColorEventDelegate>(this);
	m_pObjectOwner->UnregisterForComponentEvent(colorDelegate, COMPONENT_TYPE(ColorComponent));

	EventMachine<ComponentEventData>::EventHandler trafoDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteComponent_, &SpriteComponent_::OnTransformEventDelegate>(this);
	m_pObjectOwner->UnregisterForComponentEvent(trafoDelegate, COMPONENT_TYPE(TransformComponent));

	EventMachine<ComponentEventData>::EventHandler animDelegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteComponent_, &SpriteComponent_::OnAnimEventDelegate>(this);
	m_pObjectOwner->UnregisterForComponentEvent(animDelegate, COMPONENT_TYPE(SpriteAnimationComponent));

	EventMachine<ComponentEventData>::EventHandler anim_Delegate =
		EventMachine<ComponentEventData>::EventHandler::Build<SpriteComponent_, &SpriteComponent_::OnAnim_EventDelegate>(this);
	m_pObjectOwner->UnregisterForComponentEvent(anim_Delegate, COMPONENT_TYPE(SpriteAnimCompo_));
}

void SpriteComponent_::OnColorEventDelegate( const Event<ComponentEventData> & event_p )
{
	ColorComponent * pColor = event_p.GetDataAs<ColorComponent*>();

	m_currentColor = pColor->GetFinalColor();
	if( pColor->GonnaSnap() ) m_previousColor = m_currentColor;
	else m_previousColor = pColor->GetPreviousFinalColor();
}
void game::SpriteComponent_::OnTransformEventDelegate( const Event<ComponentEventData> & event_p )
{
	TransformComponent * pTrafo = event_p.GetDataAs<TransformComponent*>();
	
	m_currentTrafo = pTrafo->GetFinal();
	if( pTrafo->GonnaSnap() )	m_previousTrafo = m_currentTrafo;
	else m_previousTrafo = pTrafo->GetPreviousFinal();
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

	if( m_bHFlip ){
		gen::FlipUVRectHorz( ((float*)(&m_renderData.m_uvRect)) );
		m_renderData.m_padding.x = -spriteFrame.xOffset;
		
	}
	if( m_bVFlip ){
		gen::FlipUVRectVertc( ((float*)(&m_renderData.m_uvRect)) );
		m_renderData.m_padding.y = -spriteFrame.yOffset;
	}

	(*(++m_pipeState.Begin())) = pAnim->GetSprite().pBindPSSRV;
	m_TextureID = pAnim->GetSprite().iID;
	m_sortKey.bitfield.textureID = m_TextureID;
}

void game::SpriteComponent_::SetColor( DirectX::XMFLOAT4 color_p )
{
	m_previousColor = m_currentColor = color_p;
}

void game::SpriteComponent_::FlipHorzToogle()
{
	m_bHFlip = !m_bHFlip;
	gen::FlipUVRectHorz( ((float*)(&m_renderData.m_uvRect)) );
	m_renderData.m_padding.x = -m_renderData.m_padding.x;
	m_renderData.m_bUpdate = true;
}
void game::SpriteComponent_::FlipVertcToogle()
{
	m_bVFlip = !m_bVFlip;
	gen::FlipUVRectVertc( ((float*)(&m_renderData.m_uvRect)) );
	m_renderData.m_padding.y = -m_renderData.m_padding.y;
	m_renderData.m_bUpdate = true;
}
void game::SpriteComponent_::FlipHorz()
{
	if( !m_bHFlip ){
	
		m_bHFlip = true;
		gen::FlipUVRectHorz( ((float*)(&m_renderData.m_uvRect)) );
		m_renderData.m_padding.x = -m_renderData.m_padding.x;
		m_renderData.m_bUpdate = true;
	}
}
void game::SpriteComponent_::FlipVertc()
{
	if( !m_bVFlip ){
	
		m_bVFlip = true;
		gen::FlipUVRectVertc( ((float*)(&m_renderData.m_uvRect)) );
		m_renderData.m_padding.y = -m_renderData.m_padding.y;
		m_renderData.m_bUpdate = true;
	}
}
void game::SpriteComponent_::UnFlipHorz()
{
	if( m_bHFlip ){

		m_bHFlip = false;
		gen::FlipUVRectHorz( ((float*)(&m_renderData.m_uvRect)) );
		m_renderData.m_padding.x = -m_renderData.m_padding.x;
		m_renderData.m_bUpdate = true;
	}
}
void game::SpriteComponent_::UnFlipVertc()
{
	if( m_bVFlip ){

		m_bVFlip = false;
		gen::FlipUVRectVertc( ((float*)(&m_renderData.m_uvRect)) );
		m_renderData.m_padding.y = -m_renderData.m_padding.y;
		m_renderData.m_bUpdate = true;
	}
}

game::SpriteComponent_::~SpriteComponent_()
{
	if( m_pBuffer )m_pBuffer->Release();
}

void game::SpriteComponent_::InterpolateWorld( double dInterp_p )
{
	XMMATRIX mCurrentTrafo = XMLoadFloat4x4( &m_currentTrafo );

	//if( memcmp( &m_renderData.m_mWorld, &mCurrentTrafo, sizeof(XMMATRIX)) ){
	if( XMVector4NotEqual(m_renderData.m_mWorld.r[0], mCurrentTrafo.r[0])
		||
		XMVector4NotEqual(m_renderData.m_mWorld.r[1], mCurrentTrafo.r[1])
		||
		XMVector4NotEqual(m_renderData.m_mWorld.r[2], mCurrentTrafo.r[2])
		||
		XMVector4NotEqual(m_renderData.m_mWorld.r[3], mCurrentTrafo.r[3]) ){

		XMVECTOR vScale, qRot, vPos; 
		keepAssert( DirectX::XMMatrixDecompose( &vScale, &qRot, &vPos, mCurrentTrafo ) );

		XMMATRIX mPrevousTrafo = XMLoadFloat4x4( &m_previousTrafo );
		XMVECTOR vPrevScale, qPrevRot, vPrevPos;
		keepAssert( DirectX::XMMatrixDecompose( &vPrevScale, &qPrevRot, &vPrevPos, mPrevousTrafo ) );

		// interpolate independent components

		XMVECTOR vFactor = XMVectorReplicate( (float)dInterp_p); // do only once

		vScale = XMVectorLerpV( vPrevScale, vScale, vFactor );
		qRot = XMQuaternionSlerpV( qPrevRot, qRot, vFactor );
		vPos =  XMVectorLerpV( vPrevPos, vPos, vFactor );
		
		// compose matrix again

		m_renderData.m_mWorld = XMMatrixAffineTransformation(vScale, g_XMZero, qRot, vPos );

		// send to GPU?
		m_renderData.m_bUpdate = true;
	}
}
void game::SpriteComponent_::InterpolateColor( double dInterp )
{
	XMVECTOR vRenderColor = XMLoadFloat4( & m_renderData.m_color );
	XMVECTOR vCurrentColor = XMLoadFloat4( &m_currentColor );

	if( XMVector4NotEqual( vRenderColor, vCurrentColor )  ){

		XMVECTOR vPreviousColor = XMLoadFloat4( &m_previousColor );

		vCurrentColor = XMVectorLerp( vPreviousColor, vCurrentColor, (float)dInterp );

		XMStoreFloat4( &m_renderData.m_color, vCurrentColor );

		m_renderData.m_bUpdate = true;
	}
}

void game::SpriteComponent_::OnAnim_EventDelegate( const Event<ComponentEventData> & event_p )
{
	SpriteAnimCompo_ * pAnim = event_p.GetDataAs<SpriteAnimCompo_*>();

	SpriteFrame_ spriteFrame = pAnim->GetCurrentFrame();

	m_renderData.m_res.x = spriteFrame.fW;
	m_renderData.m_res.y = spriteFrame.fH;
	m_renderData.m_uvRect = spriteFrame.uvRect;
	m_renderData.m_padding.x = spriteFrame.xOffset;
	m_renderData.m_padding.y = spriteFrame.yOffset;
	m_renderData.m_bUpdate = true;

	if( m_bHFlip ){
		gen::FlipUVRectHorz( ((float*)(&m_renderData.m_uvRect)) );
		m_renderData.m_padding.x = -spriteFrame.xOffset;

	}
	if( m_bVFlip ){
		gen::FlipUVRectVertc( ((float*)(&m_renderData.m_uvRect)) );
		m_renderData.m_padding.y = -spriteFrame.yOffset;
	}

	const TextureID_Binder_Pair_ & sprite = pAnim->GetSprite((iframe)spriteFrame.iSpriteUsedIndex);
	(*(++m_pipeState.Begin())) = sprite.pBindPSSRV;
	m_TextureID = sprite.iID;
	m_sortKey.bitfield.textureID = m_TextureID;
}

void SpriteComponent_::Initialize( dx::Device * pDevice_p, const char * szTexture_p, float fWidth_p, float fHeight_p, DirectX::XMFLOAT4 uvRect_p, sprite::E_BLENDTYPE blendType_p, sprite::E_SAMPLERTYPE sampler_p, sprite::SpriteRenderer * pSpriteRenderer_p )
{
	m_pCamera = nullptr;
	m_sortKey.intRepresentation = 0LL;

	m_bHFlip = m_bVFlip = false;

	//m_BlendModeID= m_SamplerModeID= 
	m_ShaderID = 0;
	m_iShaderPermutation = 0;

	m_renderData.m_mWorld = XMMatrixIdentity();
	m_renderData.m_res.x = fWidth_p;
	m_renderData.m_res.y = fHeight_p;
	m_renderData.m_uvRect = uvRect_p;
	m_renderData.m_color.x = m_renderData.m_color.y = m_renderData.m_color.z = m_renderData.m_color.w = 1.0f;
	m_renderData.m_bUpdate = true;

	m_previousColor = m_currentColor = m_renderData.m_color;
	XMStoreFloat4x4( &m_currentTrafo, m_renderData.m_mWorld );
	m_previousTrafo = m_currentTrafo;

	if( !m_pBuffer ){
	
		// creates a ID3DBuffer for the vs constant buffer

		BufferResource::CreationParams cbufferParams;
		ZeroMemory(&cbufferParams, sizeof(BufferResource::CreationParams));
		cbufferParams.desc.bufferDesc.ByteWidth = DrawableCbuffer::s_SIZE;
		cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT; // 
		cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbufferParams.desc.bufferDesc.CPUAccessFlags =  D3D11_CPU_ACCESS_WRITE; //0; //
		cbufferParams.desc.bufferDesc.StructureByteStride = 0;
		cbufferParams.desc.bufferDesc.MiscFlags = 0;

		//ID3D11Buffer * pBuffer = NULL;
		//pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, pBuffer );
	
		m_pBuffer = dx::BufferResource::Create( pDevice_p->GetDevice(), cbufferParams );
	}

	// initialize pipe state for this sprite

	m_pipeState.Reset();
	m_VSDrawableCbufferBinder.Initialize( m_pBuffer/*pBuffer*/, &m_renderData );
	m_pipeState.AddBinderCommand( &m_VSDrawableCbufferBinder );

	unsigned int iTextureID;
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_tex2D_cache.Get(szTexture_p, &iTextureID) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_samplers_cache.GetSamplerBind(sampler_p) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_blends_cache.GetBlendBind(blendType_p) );

	m_TextureID = iTextureID;
	m_BlendModeID = blendType_p;
	m_SamplerModeID = sampler_p;
	m_sortKey.bitfield.textureID = m_TextureID;
	m_sortKey.bitfield.blending = blendType_p;
}
void SpriteComponent_::Initialize( dx::Device * pDevice_p, game::TextureID_Binder_Pair * pTexture_p, float fWidth_p, float fHeight_p, DirectX::XMFLOAT4 uvRect_p, sprite::E_BLENDTYPE blendType_p, sprite::E_SAMPLERTYPE sampler_p, sprite::SpriteRenderer * pSpriteRenderer_p )
{
	m_pCamera = nullptr;
	m_sortKey.intRepresentation = 0LL;

	m_bHFlip = m_bVFlip = false;

	//m_BlendModeID= m_SamplerModeID= 
	m_ShaderID = 0;
	m_iShaderPermutation = 0;

	m_renderData.m_mWorld = XMMatrixIdentity();
	m_renderData.m_res.x = fWidth_p;
	m_renderData.m_res.y = fHeight_p;
	m_renderData.m_uvRect = uvRect_p;
	m_renderData.m_color.x = m_renderData.m_color.y = m_renderData.m_color.z = m_renderData.m_color.w = 1.0f;
	m_renderData.m_bUpdate = true;

	m_previousColor = m_currentColor = m_renderData.m_color;
	XMStoreFloat4x4( &m_currentTrafo, m_renderData.m_mWorld );
	m_previousTrafo = m_currentTrafo;

	// creates a ID3DBuffer for the vs constant buffer

	if( !m_pBuffer ){
	
		BufferResource::CreationParams cbufferParams;
		ZeroMemory(&cbufferParams, sizeof(BufferResource::CreationParams));
		cbufferParams.desc.bufferDesc.ByteWidth = DrawableCbuffer::s_SIZE;
		cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT; // 
		cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbufferParams.desc.bufferDesc.CPUAccessFlags =  D3D11_CPU_ACCESS_WRITE; //0; //
		cbufferParams.desc.bufferDesc.StructureByteStride = 0;
		cbufferParams.desc.bufferDesc.MiscFlags = 0;

		//ID3D11Buffer * pBuffer = NULL;
		//pDevice_p->m_pCacheBuffer->Acquire( cbufferParams, pBuffer );
	
		m_pBuffer = dx::BufferResource::Create( pDevice_p->GetDevice(), cbufferParams );
	}

	// initialize pipe state for this sprite

	m_pipeState.Reset();
	m_VSDrawableCbufferBinder.Initialize( m_pBuffer, &m_renderData );
	m_pipeState.AddBinderCommand( &m_VSDrawableCbufferBinder );

	int iTextureID = pTexture_p->iID;
	m_pipeState.AddBinderCommand( pTexture_p->pBindPSSRV );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_samplers_cache.GetSamplerBind(sampler_p) );
	m_pipeState.AddBinderCommand( &pSpriteRenderer_p->m_blends_cache.GetBlendBind(blendType_p) );

	m_TextureID = iTextureID;
	m_BlendModeID = blendType_p;
	m_SamplerModeID = sampler_p;
	m_sortKey.bitfield.textureID = m_TextureID;
	m_sortKey.bitfield.blending = blendType_p;
}

//========================================================================
// 
//========================================================================

game::SpriteComponent_Factory::SpriteComponent_Factory( unsigned int maxComponents_p, dx::Device * pDevice_p, sprite::SpriteRenderer * pSpriteRenderer_p ) :
m_pool(maxComponents_p),
	m_pDeviceRef_p(pDevice_p), m_pRendererRef(pSpriteRenderer_p)
{
	m_defaults.Initialize(
		pDevice_p, "default.png", 100.0f, 100.0f, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		E_BLEND_NONE_DEFAULT, E_SAMPLER_NONE,
		pSpriteRenderer_p);
}

pool_Component_ptr game::SpriteComponent_Factory::VCreateComponent( GfigElementA * pGFig_p )
{
	//SpriteComponent_ * pSprite = m_pool.Allocate();
	pool_SpriteCompo__ptr pSprite( m_pool );

	float w = 0.0f, h = 0.0f;
	XMFLOAT4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	sprite::E_BLENDTYPE eBlend = E_BLEND_NONE_DEFAULT;
	sprite::E_SAMPLERTYPE eSampler = E_SAMPLER_NONE;
	float xOffset = 0.0f; float yOffset = 0.0f;

	GfigElementA * pParam = nullptr;

	if( pGFig_p->GetSubElement( "w", pParam ) )
		w = (float)atof( pParam->m_value.c_str() );
	if( pGFig_p->GetSubElement( "h", pParam ) )
		h = (float)atof( pParam->m_value.c_str() );

	if( pGFig_p->GetSubElement( "xoff", pParam ) ){

		xOffset = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "yoff", pParam ) ){

		yOffset = (float)atof( pParam->m_value.c_str() );
	}

	if( pGFig_p->GetSubElement( "uvRect", pParam ) ){

		uvRect = GetXYWH( pParam );
	}
	if( pGFig_p->GetSubElement( "blend", pParam ) ){

		eBlend = GetBlendType( pParam );
	}
	if( pGFig_p->GetSubElement( "sampler", pParam ) ){

		eSampler = GetSamplerType( pParam );
	}

	keepAssert( pGFig_p->GetSubElement( "texture", pParam ) );

	pSprite->Initialize( m_pDeviceRef_p, pParam->m_value.c_str(), w, h, uvRect, eBlend, eSampler, m_pRendererRef );
	pSprite->m_renderData.m_padding.x = xOffset;
	pSprite->m_renderData.m_padding.y = yOffset;
	//return MAKE_STACK_SHAREDPTR( SpriteComponent_, pSprite );

	if( w == 0.0f ){

		TextureBinders::Resolution res = m_pRendererRef->m_tex2D_cache.GetTextureRes( pSprite->m_TextureID );
		pSprite->m_renderData.m_res.x = (float)res.w;
		pSprite->m_renderData.m_res.y = (float)res.h;
	}

	if( pGFig_p->GetSubElement( "color", pParam ) ){

		ColorComponentFactory::UpdateRGBAFromGfig( pSprite->m_renderData.m_color, pParam);
		pSprite->SetColor( pSprite->m_renderData.m_color );
	}
	if( pGFig_p->GetSubElement( "d", pParam ) ){

		pSprite->m_sortKey.bitfield.Zdepth = atoi( pParam->m_value.c_str() );
	}

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

void game::SpriteComponent_Factory::CreateSprite( SpriteComponent_ & sprite_p, text::GfigElementA * pGFig_p, dx::Device * pDevice_p, sprite::SpriteRenderer * pRendererRef )
{
	float w, h;
	XMFLOAT4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	sprite::E_BLENDTYPE eBlend = E_BLEND_NONE_DEFAULT;
	sprite::E_SAMPLERTYPE eSampler = E_SAMPLER_NONE;
	float xOffset = 0.0f; float yOffset = 0.0f;

	GfigElementA * pParam = nullptr;

	keepAssert( pGFig_p->GetSubElement( "w", pParam ) );
	w = (float)atof( pParam->m_value.c_str() );
	keepAssert( pGFig_p->GetSubElement( "h", pParam ) );
	h = (float)atof( pParam->m_value.c_str() );

	if( pGFig_p->GetSubElement( "xoff", pParam ) ){

		xOffset = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "yoff", pParam ) ){

		yOffset = (float)atof( pParam->m_value.c_str() );
	}

	if( pGFig_p->GetSubElement( "uvRect", pParam ) ){

		uvRect = GetXYWH( pParam );
	}
	if( pGFig_p->GetSubElement( "blend", pParam ) ){

		eBlend = GetBlendType( pParam );
	}
	if( pGFig_p->GetSubElement( "sampler", pParam ) ){

		eSampler = GetSamplerType( pParam );
	}

	keepAssert( pGFig_p->GetSubElement( "texture", pParam ) );

	sprite_p.Initialize( pDevice_p, pParam->m_value.c_str(), w, h, uvRect, eBlend, eSampler, pRendererRef );
	sprite_p.m_renderData.m_padding.x = xOffset;
	sprite_p.m_renderData.m_padding.y = yOffset;
}

void game::SpriteComponent_Factory::LoadInstanceData( spriteInstance & inst_p, text::GfigElementA * pGFig_p )
{
	GfigElementA * pParam = nullptr;

	keepAssert( pGFig_p->GetSubElement( "w", pParam ) );
	inst_p.res[0] = (float)atof( pParam->m_value.c_str() );
	keepAssert( pGFig_p->GetSubElement( "h", pParam ) );
	inst_p.res[1] = (float)atof( pParam->m_value.c_str() );

	if( pGFig_p->GetSubElement( "xoff", pParam ) ){

		inst_p.padding[0] = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "yoff", pParam ) ){

		inst_p.padding[1] = (float)atof( pParam->m_value.c_str() );
	}

	if( pGFig_p->GetSubElement( "uvRect", pParam ) ){

		(*((XMFLOAT4*)inst_p.uvRect)) = GetXYWH( pParam );
	}
}

game::pool_Component_ptr game::SpriteComponent_Factory::VCloneComponent( const Component * pCompo_p )
{
	pool_SpriteCompo__ptr pSprite( m_pool );
	SpriteComponent_ * pOther = (SpriteComponent_*)pCompo_p;

	BufferResource::CreationParams cbufferParams;
	ZeroMemory(&cbufferParams, sizeof(BufferResource::CreationParams));
	cbufferParams.desc.bufferDesc.ByteWidth = DrawableCbuffer::s_SIZE;
	cbufferParams.desc.bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT; // 
	cbufferParams.desc.bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferParams.desc.bufferDesc.CPUAccessFlags =  D3D11_CPU_ACCESS_WRITE; //0; //
	cbufferParams.desc.bufferDesc.StructureByteStride = 0;
	cbufferParams.desc.bufferDesc.MiscFlags = 0;

	pSprite->m_pBuffer = dx::BufferResource::Create( m_pDeviceRef_p->GetDevice(), cbufferParams );
	
	pSprite->m_VSDrawableCbufferBinder.Initialize( pSprite->m_pBuffer, &pSprite->m_renderData );

	pSprite->m_pipeState.AddBinderCommand( &pSprite->m_VSDrawableCbufferBinder );

	//

	vBinderPtrs::iterator itBinders = pOther->m_pipeState.Begin();
	++itBinders;
	pSprite->m_pipeState.AddBinderCommand( *itBinders ); ++itBinders;
	pSprite->m_pipeState.AddBinderCommand( *itBinders ); ++itBinders;
	pSprite->m_pipeState.AddBinderCommand( *itBinders );

	//

	pSprite->m_TextureID = pOther->m_TextureID;
	pSprite->m_BlendModeID = pOther->m_BlendModeID;
	pSprite->m_SamplerModeID = pOther->m_SamplerModeID;
	pSprite->m_ShaderID = pOther->m_ShaderID;
	pSprite->m_iShaderPermutation = pOther->m_iShaderPermutation;


	pSprite->m_sortKey = pOther->m_sortKey;
	pSprite->m_renderData = pOther->m_renderData;
	pSprite->m_pCamera = pOther->m_pCamera;

	pSprite->m_currentTrafo = pOther->m_currentTrafo;
	pSprite->m_previousTrafo = pOther->m_previousTrafo;
	pSprite->m_currentColor = pOther->m_currentColor;
	pSprite->m_previousColor = pOther->m_previousColor;

	pSprite->m_bHFlip = pOther->m_bHFlip;
	pSprite->m_bVFlip = pOther->m_bVFlip;

	return pSprite;
}

void game::SpriteComponent_Factory::VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p )
{
	GfigElementA * pParam = nullptr;

	SpriteComponent_ *pSprite = (SpriteComponent_*)pCompo_p;

	if( pGFig_p->GetSubElement( "w", pParam ) ) pSprite->m_renderData.m_res.x = (float)atof( pParam->m_value.c_str() );
	if( pGFig_p->GetSubElement( "h", pParam ) ) pSprite->m_renderData.m_res.y = (float)atof( pParam->m_value.c_str() );

	if( pGFig_p->GetSubElement( "xoff", pParam ) ) pSprite->m_renderData.m_padding.x = (float)atof( pParam->m_value.c_str() );
	if( pGFig_p->GetSubElement( "yoff", pParam ) ) pSprite->m_renderData.m_padding.y = (float)atof( pParam->m_value.c_str() );

	if( pGFig_p->GetSubElement( "uvRect", pParam ) ) UpdateXYWHFromGfig( pSprite->m_renderData.m_uvRect, pParam );
	if( pGFig_p->GetSubElement( "blend", pParam ) ){
	
		sprite::E_BLENDTYPE eBlend = GetBlendType( pParam );
	
		if( pSprite->m_BlendModeID != (unsigned)eBlend ){

			pSprite->m_BlendModeID = eBlend;
			pSprite->m_sortKey.bitfield.blending = eBlend;

			auto pipeIt = pSprite->m_pipeState.Begin();
			pipeIt+=3;
			*pipeIt = &m_pRendererRef->m_blends_cache.GetBlendBind(eBlend);
		}
	}
	if( pGFig_p->GetSubElement( "sampler", pParam ) ){

		sprite::E_SAMPLERTYPE eSampler = GetSamplerType( pParam );

		if( pSprite->m_SamplerModeID != (unsigned)eSampler ){

			pSprite->m_SamplerModeID = eSampler;

			auto pipeIt = pSprite->m_pipeState.Begin();
			pipeIt+=2;
			*pipeIt = &m_pRendererRef->m_samplers_cache.GetSamplerBind(eSampler);
		}
	}

	if( pGFig_p->GetSubElement( "texture", pParam ) ){
		
		if( m_pRendererRef->m_tex2D_cache.GetTextureName( pSprite->m_TextureID ) != pParam->m_value.c_str() ){

			auto pipeIt = pSprite->m_pipeState.Begin();
			pipeIt++;
			*pipeIt = &m_pRendererRef->m_tex2D_cache.Get( pParam->m_value.c_str(), &pSprite->m_TextureID );
		}
	}

	if( pGFig_p->GetSubElement( "color", pParam ) ){

		ColorComponentFactory::UpdateRGBAFromGfig( pSprite->m_renderData.m_color, pParam);
		pSprite->SetColor( pSprite->m_renderData.m_color );
	}

	if( pGFig_p->GetSubElement( "d", pParam ) ){

		pSprite->m_sortKey.bitfield.Zdepth = atoi( pParam->m_value.c_str() );
	}
}

void game::SpriteComponent_Factory::UpdateXYWHFromGfig( DirectX::XMFLOAT4 & xywh_p, text::GfigElementA * pGFig_p )
{
	GfigElementA * pElement;

	if( pGFig_p->GetSubElement( "x", pElement) ){
		xywh_p.x = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "y", pElement) ){
		xywh_p.y = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "w", pElement) ){
		xywh_p.z = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "h", pElement) ){
		xywh_p.w = (float)atof(pElement->m_value.c_str());
	}
}
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void game::SpriteComponent_Factory::VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p )
{
	/*
	[SpriteComponent_
		[texture = C:\Users\Gateway\Google Drive\LegaliaContents\lvlPiecesAtlas.png] #*
		[w = 737] #*
		[h = 167] #*
		[uvRect
			[x = 0]
			[y = 0.709961]
			[w = 0.359863]
			[h = 0.163086]
		]
		[d = 1]
		[blend = alpha]
		[sampler = linear]
		[xoff = -3.5]
		[yoff = 0]
	]
	# #*
	# required fields
	*/

	SpriteComponent_ & sprite = *((SpriteComponent_*)pCompo_p);

	pGFig_p->m_subElements.emplace_back( GfigElementA(COMPONENT_NAME(SpriteComponent_)) );

	GfigElementA & gSpriteCompo = pGFig_p->m_subElements.back();
	{
		//GfigElementA gTexture( "texture", m_pRendererRef->m_tex2D_cache.GetTextureName( sprite.GetTextureID() ).c_str() );
		gSpriteCompo.m_subElements.push_back(
			GfigElementA( "texture", m_pRendererRef->m_tex2D_cache.GetTextureName( sprite.GetTextureID() ).c_str() )
			);

		TextureBinders::Resolution res = m_pRendererRef->m_tex2D_cache.GetTextureRes( sprite.m_TextureID );
		if( (float)res.w != sprite.m_renderData.m_res.x
			||
			(float)res.h != sprite.m_renderData.m_res.y ){

				gSpriteCompo.m_subElements.push_back(
					GfigElementA( "w", std::to_string((long double)sprite.m_renderData.m_res.x).c_str() )
					);

				gSpriteCompo.m_subElements.push_back(
					GfigElementA( "h", std::to_string((long double)sprite.m_renderData.m_res.y).c_str() )
					);
		}


		GfigElementA gRect(	"uvRect" );
		SerializeXYZW( sprite.m_renderData.m_uvRect, m_defaults.m_renderData.m_uvRect, gRect );
		if( gRect.m_subElements.size())
			gSpriteCompo.m_subElements.push_back(std::move(gRect));


		GfigElementA gBlend( "blend" );
		if( SerializeBlendType( (E_BLENDTYPE)sprite.m_BlendModeID, (E_BLENDTYPE)m_defaults.m_BlendModeID, gBlend ) ){
			gSpriteCompo.m_subElements.push_back(std::move(gBlend));
		}

		GfigElementA gSampler( "sampler" );
		if( SerializeSamplerType( (E_SAMPLERTYPE)sprite.m_SamplerModeID, (E_SAMPLERTYPE)m_defaults.m_SamplerModeID, gSampler ) ){
			gSpriteCompo.m_subElements.push_back(std::move(gSampler));
		}

		if( sprite.m_renderData.m_padding.x != 0.0f ){
			gSpriteCompo.m_subElements.push_back(GfigElementA("xoff", std::to_string((long double)sprite.m_renderData.m_padding.x).c_str()) );
		}
		if( sprite.m_renderData.m_padding.y != 0.0f ){
			gSpriteCompo.m_subElements.push_back(GfigElementA("yoff", std::to_string((long double)sprite.m_renderData.m_padding.y).c_str()) );
		}
		if( sprite.m_sortKey.bitfield.Zdepth != 0.0f ){
			gSpriteCompo.m_subElements.push_back(GfigElementA("d", std::to_string((_ULonglong)sprite.m_sortKey.bitfield.Zdepth).c_str()) );
		}

		GfigElementA gColor("color");
		ColorComponentFactory::SerializeRGBA(sprite.m_currentColor, m_defaults.m_currentColor, gColor);
		if( gColor.m_subElements.size() ){

			gSpriteCompo.m_subElements.push_back(std::move(gColor));
		}
	}
}

void game::SpriteComponent_Factory::VSerialize( const Component * pCompo_p, const Component * pDefaults_p, text::GfigElementA * pGFig_p )
{
	SpriteComponent_ & sprite = *((SpriteComponent_*)pCompo_p);
	SpriteComponent_ & def = *((SpriteComponent_*)pDefaults_p);

	pGFig_p->m_subElements.emplace_back( GfigElementA(COMPONENT_NAME(SpriteComponent_)) );

	GfigElementA & gSpriteCompo = pGFig_p->m_subElements.back();
	{
		if( sprite.m_TextureID != def.m_TextureID ){

			gSpriteCompo.m_subElements.push_back(
				GfigElementA( "texture", m_pRendererRef->m_tex2D_cache.GetTextureName( sprite.GetTextureID() ).c_str() )
				);
		}
		if( sprite.m_renderData.m_res.x != def.m_renderData.m_res.x ){

			gSpriteCompo.m_subElements.push_back(
				GfigElementA( "w", std::to_string((long double)sprite.m_renderData.m_res.x).c_str() )
				);
		}
		if( sprite.m_renderData.m_res.y != def.m_renderData.m_res.y ){

				gSpriteCompo.m_subElements.push_back(
					GfigElementA( "h", std::to_string((long double)sprite.m_renderData.m_res.y).c_str() )
					);
		}

		GfigElementA gRect(	"uvRect" );
		SerializeXYZW( sprite.m_renderData.m_uvRect, def.m_renderData.m_uvRect, gRect );
		if( gRect.m_subElements.size())
			gSpriteCompo.m_subElements.push_back(std::move(gRect));

		GfigElementA gBlend( "blend" );
		if(SerializeBlendType( (E_BLENDTYPE)sprite.m_BlendModeID, (E_BLENDTYPE)def.m_BlendModeID, gBlend ) )
			gSpriteCompo.m_subElements.push_back(std::move(gBlend));
		

		GfigElementA gSampler( "sampler" );
		if( SerializeSamplerType( (E_SAMPLERTYPE)sprite.m_SamplerModeID, (E_SAMPLERTYPE)def.m_SamplerModeID, gSampler ) )
			gSpriteCompo.m_subElements.push_back(std::move(gSampler));
		

		if( sprite.m_renderData.m_padding.x != def.m_renderData.m_padding.x ){
			gSpriteCompo.m_subElements.push_back(GfigElementA("xoff", std::to_string((long double)sprite.m_renderData.m_padding.x).c_str()) );
		}
		if( sprite.m_renderData.m_padding.y != def.m_renderData.m_padding.y ){
			gSpriteCompo.m_subElements.push_back(GfigElementA("yoff", std::to_string((long double)sprite.m_renderData.m_padding.y).c_str()) );
		}
		if( sprite.m_sortKey.bitfield.Zdepth != def.m_sortKey.bitfield.Zdepth  ){
			gSpriteCompo.m_subElements.push_back(GfigElementA("d", std::to_string((_ULonglong)sprite.m_sortKey.bitfield.Zdepth).c_str()) );
		}

		GfigElementA gColor("color");
		ColorComponentFactory::SerializeRGBA(sprite.m_currentColor, def.m_currentColor, gColor);
		if( gColor.m_subElements.size() ){
			gSpriteCompo.m_subElements.push_back(std::move(gColor));
		}
	}

	if( gSpriteCompo.m_subElements.size() == 0 ) pGFig_p->m_subElements.pop_back();
}

void game::SpriteComponent_Factory::SerializeXYZW( const DirectX::XMFLOAT4 & xywh_p, const DirectX::XMFLOAT4 & defs_p, text::GfigElementA & gFig_p )
{
	// check if values are different than default values, since default values dont need to be loaded

	if( xywh_p.x != defs_p.x ){

		gFig_p.m_subElements.push_back(GfigElementA("x", std::to_string((long double)xywh_p.x ).c_str()) );
	}
	if( xywh_p.y != defs_p.y ){

		gFig_p.m_subElements.push_back(GfigElementA("y", std::to_string((long double)xywh_p.y ).c_str()) );
	}
	if( xywh_p.z != defs_p.z ){

		gFig_p.m_subElements.push_back(GfigElementA("w", std::to_string((long double)xywh_p.z ).c_str()) );
	}
	if( xywh_p.w != defs_p.w ){

		gFig_p.m_subElements.push_back(GfigElementA("h", std::to_string((long double)xywh_p.w ).c_str()) );
	}
}

bool game::SpriteComponent_Factory::SerializeBlendType( const E_BLENDTYPE eBlend_p, const E_BLENDTYPE def_p, text::GfigElementA & gFig_p )
{
	if( eBlend_p == def_p ) return false;

	if( eBlend_p == E_BLEND_ALPHA_BLENDED ){
		gFig_p.m_value = "alpha";
	}
	else if( eBlend_p == E_BLEND_ADDITIVE ){

		gFig_p.m_value = "add";
	}
	else{

		gFig_p.m_value = "none";
	}

	return true;
}

bool game::SpriteComponent_Factory::SerializeSamplerType( const sprite::E_SAMPLERTYPE eSampler_p, const sprite::E_SAMPLERTYPE def_p, GfigElementA & gFig_p )
{
	if( eSampler_p == def_p ) return false;

	if( eSampler_p == E_SAMPLER_LINEAR ){
		gFig_p.m_value = "linear";
	}
	else{

		gFig_p.m_value = "none";
	}

	return true;
}
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
Component * game::SpriteComponent_Factory::VGetDefaultCompo()
{
	return &m_defaults;
}

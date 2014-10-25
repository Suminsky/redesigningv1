#include "SpriteAnimationComponent.h"
#include "../../namespace sprite/SpriteRenderer.h"
#include "../../namespace text/TextParser.h"
//#include "SpriteComponent_.h"

using namespace game;
using namespace sprite;
using namespace text;


game::SpriteAnimationComponent::SpriteAnimationComponent()
{
	m_type = COMPONENT_TYPE(SpriteAnimationComponent);

	m_iPreviousClip = m_iCurrentClip = 0;
	m_previousFrame = m_currentFrame = (unsigned)-1;
}

unsigned int SpriteAnimationComponent::GetClipIndexFromName( const char * szClipName_p )
{
	for( int it = 0, nClips = (int)m_vClips.size();
		it < nClips;
		++it ){

			if( m_vClips[it].configData.szName == szClipName_p )
				return it;
	}

	assert(0);

	return 0;
}

void game::SpriteAnimationComponent::Update( animTimeUnit delta_p )
{
	// get current clip

	AnimationClip * pCurrentClip = &m_vClips[m_iCurrentClip];

	pCurrentClip->stateData.deltaAccum += delta_p;

	while( pCurrentClip->stateData.deltaAccum > pCurrentClip->configData.SPF ){

		pCurrentClip->stateData.deltaAccum -= pCurrentClip->configData.SPF;
		++pCurrentClip->stateData.itCurrentFrame;

		unsigned int itLastFrame = (unsigned int)pCurrentClip->configData.vFrames.size() -1;
		if( pCurrentClip->stateData.itCurrentFrame > itLastFrame ){

			if( OnFrameWrap( pCurrentClip ) ){

				break;
			}
		}
	}

	//m_previousFrame = m_currentFrame;
	m_currentFrame = pCurrentClip->GetCurrentFrame();
}

bool game::SpriteAnimationComponent::OnFrameWrap( AnimationClip *& pCurrentClip_p )
{
	switch( pCurrentClip_p->configData.eWrapMode ){

	case E_ANIMWRAPMODE_LOOP:

		pCurrentClip_p->stateData.itCurrentFrame = 0;
		return false;
		break;

	case E_ANIMWRAPMODE_ONCE:

		//Stop();
		//m_iCurrentClip = m_iPreviousClip;	// set previous clip as current again
		PlayClip( m_iPreviousClip );		// play it again

		pCurrentClip_p = &m_vClips[m_iCurrentClip];
		return false;
		break;

	case E_ANIMWRAPMODE_CLAMPLAST:{

		Stop();

		unsigned int itLastFrame = (unsigned int)pCurrentClip_p->configData.vFrames.size() -1;
		pCurrentClip_p->stateData.itCurrentFrame = itLastFrame;

		return true;
								  }break;

	case E_ANIMWRAPMODE_CLAMFIRST:

		Stop();
		return true;

		break;
	}

	return false;
}

void game::SpriteAnimationComponent::PlayClip( unsigned int iClip_p, bool bPauseCurrent_dontStop_p /*= false */ )
{
	if( bPauseCurrent_dontStop_p ){

		Pause();
	}
	else{

		Stop();
	}

	m_iPreviousClip = m_iCurrentClip;
	m_iCurrentClip = iClip_p;

	m_vClips[m_iCurrentClip].stateData.eState = E_ANIMSTATE_PLAYING;
}

void game::SpriteAnimationComponent::StopClip( unsigned int iClip_p )
{
	AnimationClip & clip = m_vClips[iClip_p];

	clip.stateData.deltaAccum = (animTimeUnit)0.0;
	clip.stateData.itCurrentFrame = 0;
	clip.stateData.eState = E_ANIMSTATE_STOPPED;
}

void game::SpriteAnimationComponent::PauseClip( unsigned int iClip_p )
{
	AnimationClip & clip = m_vClips[iClip_p];

	clip.stateData.eState = E_ANIMSTATE_PAUSED;
}

void game::SpriteAnimationComponent::SetSpriteFrame( frame frame_p, bool bPauseCurrent_dontStop_p /*= true */ )
{
	if( bPauseCurrent_dontStop_p ){

		Pause();
	}
	else{

		Stop();
	}

	//m_previousFrame = m_currentFrame;
	m_currentFrame = frame_p;
}

void game::SpriteAnimationComponent::SetClipFrameIndex( unsigned int frameIndex_p )
{
	AnimationClip & clip = m_vClips[m_iCurrentClip];

	clip.stateData.itCurrentFrame = frameIndex_p;
}



//========================================================================
// 
//========================================================================
game::SpriteAnimationComponentFactory::SpriteAnimationComponentFactory( unsigned int maxComponents_p, sprite::SpriteRenderer * pSpriteRenderer_p )
	:
m_pool(maxComponents_p),
m_pSpriteRenderer(pSpriteRenderer_p)
{

}

game::pool_Component_ptr game::SpriteAnimationComponentFactory::VCreateComponent( GfigElementA * pGFig_p )
{
	//SpriteAnimationComponent * pAnimCompo = m_pool.Allocate();
	pool_SpriteAnimCompo_ptr pAnimCompo( m_pool );

	GfigElementA * pGFigParam = nullptr;

	keepAssert( pGFig_p->GetSubElement( "sprites", pGFigParam ) );
	LoadSpritesFromGFig( pGFigParam, pAnimCompo.Get() );

	keepAssert( pGFig_p->GetSubElement( "frames", pGFigParam ) );
	LoadFramesFromGFig( pGFigParam, pAnimCompo.Get() );

	keepAssert( pGFig_p->GetSubElement( "clips", pGFigParam ) );
	LoadClipsFromGFig( pGFigParam, pAnimCompo.Get() );

	//return  MAKE_STACK_SHAREDPTR( SpriteAnimationComponent, pAnimCompo );
	return pAnimCompo;
}

void game::SpriteAnimationComponentFactory::LoadSpritesFromGFig( text::GfigElementA * pGFig_p, SpriteAnimationComponent * compo_p )
{
	for( int it = 0, nSprites = (int)pGFig_p->m_subElements.size();
		 it < nSprites;
		 ++it ){

			 GfigElementA & spriteGFig = pGFig_p->m_subElements[it];

			 TextureID_Binder_Pair spriteUsed;
			 spriteUsed.pBindPSSRV = &m_pSpriteRenderer->m_tex2D_cache.Get( spriteGFig.m_name.c_str(), &spriteUsed.iID );
			 compo_p->m_vSprites.push_back(spriteUsed);
	}
}

void game::SpriteAnimationComponentFactory::LoadFramesFromGFig( text::GfigElementA * pGFig_p, SpriteAnimationComponent * compo_p )
{
	// to work as state machine, the params are holded, so if they not present in some, the same
	// previous value is used
	
	SpriteFrame newFrame;
	newFrame.iSpriteUsedIndex = 0;
	newFrame.xOffset = newFrame.yOffset = 0.0f;
	newFrame.fW = newFrame.fH = 32.0f; // really random value that should never be used
	newFrame.uvRect = DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );

	for( int it = 0, nFrames = (int)pGFig_p->m_subElements.size();
		it < nFrames;
		++it ){

			GfigElementA & frameGFig = pGFig_p->m_subElements[it];

			
			GfigElementA * pGFigParam;

			if( frameGFig.GetSubElement( "sprite", pGFigParam ) ){

				newFrame.iSpriteUsedIndex = atoi( pGFigParam->m_value.c_str() );
			}
			//
			if( frameGFig.GetSubElement( "xoff", pGFigParam ) ){

				newFrame.xOffset = (float)atof( pGFigParam->m_value.c_str() );
			}
			if( frameGFig.GetSubElement( "yoff", pGFigParam ) ){

				newFrame.yOffset = (float)atof( pGFigParam->m_value.c_str() );
			}
			//
			if( frameGFig.GetSubElement( "w", pGFigParam ) ){

				newFrame.fW = (float)atof( pGFigParam->m_value.c_str() );
			}
			if( frameGFig.GetSubElement( "h", pGFigParam ) ){

				newFrame.fH = (float)atof( pGFigParam->m_value.c_str() );
			}
			//
			if( frameGFig.GetSubElement( "uvRect", pGFigParam ) ){

				LoadXYWHFromGfig( pGFigParam, newFrame.uvRect );
			}

			compo_p->m_vFrames.push_back( newFrame );
	}
}

void game::SpriteAnimationComponentFactory::LoadClipsFromGFig( text::GfigElementA * pGFig_p, SpriteAnimationComponent * compo_p )
{
	// to work as state machine, the params are holded, so if they not present in some, the same
	// previous value is used
	
	AnimationClip animClip;
	animClip.configData.eWrapMode = E_ANIMWRAPMODE_LOOP;
	animClip.configData.SPF = 0.083333; // 12 fps

	for( int it = 0, nClips = (int)pGFig_p->m_subElements.size();
		it < nClips;
		++it ){

			GfigElementA & clipGFig = pGFig_p->m_subElements[it];

			GfigElementA * pGFigParam;

			animClip.configData.szName = clipGFig.m_name;

			if( clipGFig.GetSubElement( "wrap", pGFigParam ) ){

				animClip.configData.eWrapMode = GetWrapModeFromGFig( pGFigParam );
			}
			//
			if( clipGFig.GetSubElement( "spf", pGFigParam ) ){

				animClip.configData.SPF = (animTimeUnit)atof( pGFigParam->m_value.c_str() );
			}
			//

			keepAssert( clipGFig.GetSubElement( "frames", pGFigParam ) );

			animClip.configData.vFrames.resize(0);
			for( int it = 0, nFrames = (int) pGFigParam->m_subElements.size();
				 it < nFrames;
				 ++it ){

					 GfigElementA & frame = pGFigParam->m_subElements[it];

					 animClip.configData.vFrames.push_back( atoi(frame.m_name.c_str()) );
			}


			compo_p->m_vClips.push_back( animClip );
	}
}

game::E_ANIMWRAPMODE game::SpriteAnimationComponentFactory::GetWrapModeFromGFig( text::GfigElementA * pGFig_p )
{
	if( pGFig_p->m_value == s_szAnimWrapModes[E_ANIMWRAPMODE_LOOP] ){

		return E_ANIMWRAPMODE_LOOP;
	}
	/*else if( pGFig_p->m_value == "pingpong" ){

		return E_ANIMWRAPMODE_PINGPONG;
	}*/
	else if( pGFig_p->m_value == s_szAnimWrapModes[E_ANIMWRAPMODE_ONCE] ){

		return E_ANIMWRAPMODE_ONCE;
	}
	else if( pGFig_p->m_value == s_szAnimWrapModes[E_ANIMWRAPMODE_CLAMFIRST] ){

		return E_ANIMWRAPMODE_CLAMFIRST;
	}
	else if( pGFig_p->m_value == s_szAnimWrapModes[E_ANIMWRAPMODE_CLAMPLAST] ){

		return E_ANIMWRAPMODE_CLAMPLAST;
	}

	return E_ANIMWRAPMODE_LOOP;
}

void game::SpriteAnimationComponentFactory::LoadXYWHFromGfig( text::GfigElementA * pGFig_p, DirectX::XMFLOAT4 & rect_p )
{
	GfigElementA * pElement;

	if( pGFig_p->GetSubElement( "x", pElement) ){
		rect_p.x = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "y", pElement) ){
		rect_p.y = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "w", pElement) ){
		rect_p.z = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "h", pElement) ){
		rect_p.w = (float)atof(pElement->m_value.c_str());
	}
}

void game::SpriteAnimationComponentFactory::LoadFrameFromGfig( text::GfigElementA * pGFig_p, SpriteFrame & spriteFrame_p )
{
	spriteFrame_p.iSpriteUsedIndex = 0;
	spriteFrame_p.xOffset = spriteFrame_p.yOffset = 0.0f;
	spriteFrame_p.fW = spriteFrame_p.fH = 32.0f; // really random value that should never be used
	spriteFrame_p.uvRect = DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );

	GfigElementA * pGFigParam;

	if( pGFig_p->GetSubElement( "sprite", pGFigParam ) ){

		spriteFrame_p.iSpriteUsedIndex = atoi( pGFigParam->m_value.c_str() );
	}
	//
	if( pGFig_p->GetSubElement( "xoff", pGFigParam ) ){

		spriteFrame_p.xOffset = (float)atof( pGFigParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "yoff", pGFigParam ) ){

		spriteFrame_p.yOffset = (float)atof( pGFigParam->m_value.c_str() );
	}
	//
	if( pGFig_p->GetSubElement( "w", pGFigParam ) ){

		spriteFrame_p.fW = (float)atof( pGFigParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "h", pGFigParam ) ){

		spriteFrame_p.fH = (float)atof( pGFigParam->m_value.c_str() );
	}
	//
	if( pGFig_p->GetSubElement( "uvRect", pGFigParam ) ){

		LoadXYWHFromGfig( pGFigParam, spriteFrame_p.uvRect );
	}
}

void game::SpriteAnimationComponentFactory::LoadFrame( SpriteFrame & frame_p, SpriteAnimationComponent * compo_p )
{
	compo_p->m_vFrames.push_back( frame_p );
}

void game::SpriteAnimationComponentFactory::LoadClip( AnimationClip::ConfigData & clip_p, SpriteAnimationComponent * compo_p )
{
	AnimationClip animClip;
	animClip.configData = clip_p;

	compo_p->m_vClips.push_back( animClip );
}

void game::SpriteAnimationComponentFactory::LoadSprite( TextureID_Binder_Pair & ID_Binder_p, SpriteAnimationComponent * compo_p )
{
	compo_p->m_vSprites.push_back( ID_Binder_p );
}

int game::SpriteAnimationComponentFactory::LoadFrameIfNew( SpriteFrame & frame_p, SpriteAnimationComponent * compo_p )
{
	int iSize = (int)compo_p->m_vFrames.size();
	for( int it = 0; it < iSize; ++it ){

		SpriteFrame & frameNew = compo_p->m_vFrames[it];
		if( frameNew == frame_p  )
			return it;
	}


	compo_p->m_vFrames.push_back( frame_p );
	return iSize;
}

void game::SpriteAnimationComponentFactory::LoadClipFrame( int iClip_p, frame frame_p, SpriteAnimationComponent * compo_p )
{
	compo_p->m_vClips[iClip_p].configData.vFrames.push_back( frame_p );
}

void game::SpriteAnimationComponentFactory::UpdateClipConfig_NoFrames( int iClip_p, AnimationClip::ConfigData & clip_p, SpriteAnimationComponent * compo_p )
{
	compo_p->m_vClips[iClip_p].configData.eWrapMode = clip_p.eWrapMode;
	compo_p->m_vClips[iClip_p].configData.szName = clip_p.szName;
	compo_p->m_vClips[iClip_p].configData.SPF = clip_p.SPF;
}

int game::SpriteAnimationComponentFactory::LoadSpriteIfNew( TextureID_Binder_Pair & ID_Binder_p, SpriteAnimationComponent * compo_p )
{
	int iSize = (int)compo_p->m_vSprites.size();
	for( int it = 0; it < iSize; ++it ){

		TextureID_Binder_Pair & spriteNew = compo_p->m_vSprites[it];
		if( spriteNew == ID_Binder_p  )
			return it;
	}


	compo_p->m_vSprites.push_back( ID_Binder_p );
	return iSize;
}

void game::SpriteAnimationComponentFactory::ClearAllFrameInfo_KeepClipsConfig( SpriteAnimationComponent * compo_p )
{
	compo_p->m_vSprites.clear();
	compo_p->m_vFrames.clear();
	
	for( int it = 0, iSize = (int)compo_p->m_vClips.size(); it < iSize; ++it ){

		compo_p->m_vClips[it].configData.vFrames.clear();
		compo_p->m_vClips[it].stateData.itCurrentFrame = 0;
		compo_p->m_vClips[it].stateData.deltaAccum = (animTimeUnit)0.0;
	}
}

AnimationClip::ConfigData game::SpriteAnimationComponentFactory::GetClipConfigData( int iClip_p, SpriteAnimationComponent * compo_p )
{
	return compo_p->m_vClips[iClip_p].configData;
}

void game::SpriteAnimationComponentFactory::ReplaceFrames( SpriteAnimationComponent * compo_p, std::vector<SpriteFrame> && vFrames_p )
{
	compo_p->m_vFrames = std::move( vFrames_p );
}

void game::SpriteAnimationComponentFactory::ReplaceSprites( SpriteAnimationComponent * compo_p, std::vector<TextureID_Binder_Pair> && vSprites_p )
{
	compo_p->m_vSprites = std::move( vSprites_p );
}

game::pool_Component_ptr game::SpriteAnimationComponentFactory::VCloneComponent( const Component * pCompo_p )
{
	pool_SpriteAnimCompo_ptr pAnimCompo( m_pool );
	SpriteAnimationComponent * pOther = (SpriteAnimationComponent*)pCompo_p;

	pAnimCompo->m_currentFrame = pOther->m_currentFrame;
	pAnimCompo->m_previousFrame = pOther->m_previousFrame;
	pAnimCompo->m_iCurrentClip = pOther->m_iCurrentClip;
	pAnimCompo->m_iPreviousClip = pOther->m_iPreviousClip;

	pAnimCompo->m_vClips = pOther->m_vClips;
	pAnimCompo->m_vFrames = pOther->m_vFrames;
	pAnimCompo->m_vSprites = pOther->m_vSprites;
	
	return pAnimCompo;
}

void game::SpriteAnimationComponentFactory::VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p )
{
	GfigElementA * pGFigParam = nullptr;

	SpriteAnimationComponent * pAnimCompo = (SpriteAnimationComponent*)pCompo_p;

	// TODO
	// not sure how this should work, sprites and frames are added, clips too, but clips reference to frames and frames ref
	// to sprites are all on the gfig...so if the user doesnt take in account that the sprites and frames will be ADDED, not
	// replaced, shit will happen

	if( pGFig_p->GetSubElement( "sprites", pGFigParam ) )
		LoadSpritesFromGFig( pGFigParam, pAnimCompo );

	if( pGFig_p->GetSubElement( "frames", pGFigParam ) )
		LoadFramesFromGFig( pGFigParam, pAnimCompo );

	if( pGFig_p->GetSubElement( "clips", pGFigParam ) )
		LoadClipsFromGFig( pGFigParam, pAnimCompo );
}
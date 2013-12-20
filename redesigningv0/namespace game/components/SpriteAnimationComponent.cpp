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

	AnimationClip & currentClip = m_vClips[m_iCurrentClip];

	currentClip.stateData.deltaAccum += delta_p;

	while( currentClip.stateData.deltaAccum > currentClip.configData.SPF ){

		currentClip.stateData.deltaAccum -= currentClip.configData.SPF;
		++currentClip.stateData.itCurrentFrame;

		unsigned int itLastFrame = (unsigned int)currentClip.configData.vFrames.size() -1;
		if( currentClip.stateData.itCurrentFrame > itLastFrame ){

			if( OnFrameWrap( currentClip ) ){

				break;
			}
		}
	}

	//m_previousFrame = m_currentFrame;
	m_currentFrame = currentClip.GetCurrentFrame();
}

bool game::SpriteAnimationComponent::OnFrameWrap( AnimationClip & currentClip )
{
	switch( currentClip.configData.eWrapMode ){

	case E_ANIMWRAPMODE_LOOP:

		currentClip.stateData.itCurrentFrame = 0;
		return false;
		break;

	case E_ANIMWRAPMODE_ONCE:

		Stop();
		m_iCurrentClip = m_iPreviousClip;	// set previous clip as current again
		Play();								// play it again

		currentClip = m_vClips[m_iCurrentClip];
		return false;
		break;

	case E_ANIMWRAPMODE_CLAMPLAST:{

		Stop();
		unsigned int itLastFrame = (unsigned int)currentClip.configData.vFrames.size() -1;
		//m_previousFrame = m_currentFrame;
		currentClip.stateData.itCurrentFrame = itLastFrame;
		//m_currentFrame = currentClip.configData.vFrames[itLastFrame];

		return true;
								  }break;

	case E_ANIMWRAPMODE_CLAMFIRST:

		Stop();
		//m_previousFrame = m_currentFrame;
		//m_currentFrame = currentClip.GetCurrentFrame();
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

void game::SpriteAnimationComponent::SetClipFrame( unsigned int frameIndex_p )
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

game::shared_Component_ptr game::SpriteAnimationComponentFactory::VCreateComponent( GfigElementA * pGFig_p )
{
	SpriteAnimationComponent * pAnimCompo = m_pool.Allocate();

	GfigElementA * pGFigParam = nullptr;

	assert( pGFig_p->GetSubElement( "sprites", pGFigParam ) );
	LoadSpritesFromGFig( pGFigParam, pAnimCompo );

	assert( pGFig_p->GetSubElement( "frames", pGFigParam ) );
	LoadFramesFromGFig( pGFigParam, pAnimCompo );

	assert( pGFig_p->GetSubElement( "clips", pGFigParam ) );
	LoadClipsFromGFig( pGFigParam, pAnimCompo );

	return  MAKE_STACK_SHAREDPTR( SpriteAnimationComponent, pAnimCompo );
}

void game::SpriteAnimationComponentFactory::LoadSpritesFromGFig( text::GfigElementA * pGFig_p, SpriteAnimationComponent * compo_p )
{
	for( int it = 0, nSprites = (int)pGFig_p->m_subElements.size();
		 it < nSprites;
		 ++it ){

			 GfigElementA & spriteGFig = pGFig_p->m_subElements[it];

			 TextureID_Binder_Pair spriteUsed;
			 spriteUsed.pBindPSSRV = &m_pSpriteRenderer->m_texs.Get( spriteGFig.m_name.c_str(), &spriteUsed.iID );
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

			assert( clipGFig.GetSubElement( "frames", pGFigParam ) );

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
	if( pGFig_p->m_value == "loop" ){

		return E_ANIMWRAPMODE_LOOP;
	}
	/*else if( pGFig_p->m_value == "pingpong" ){

		return E_ANIMWRAPMODE_PINGPONG;
	}*/
	else if( pGFig_p->m_value == "once" ){

		return E_ANIMWRAPMODE_ONCE;
	}
	else if( pGFig_p->m_value == "clampfirst" ){

		return E_ANIMWRAPMODE_CLAMFIRST;
	}
	else if( pGFig_p->m_value == "clamplast" ){

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
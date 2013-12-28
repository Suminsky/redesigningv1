#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/11/25
	created:	25:11:2013   9:09
	file:		SpriteAnimationComponent.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>
#include <vector>
#include <DirectXMath.h>

// private includes
#include "../Component.h"
#include "../../namespace gen/Pool.h"
#include "../ComponentFactory.h"
#include "../../namespace sprite/binders cache/TextureBinders.h"

namespace gen{
	template<typename T>	class Pool;
}
namespace sprite{

	class SpriteRenderer;
}

namespace game{

	class SpriteAnimationSystem;
	class SpriteAnimationComponentFactory;

	typedef unsigned int frame;
	typedef double		animTimeUnit;

	enum E_ANIMWRAPMODE{

		E_ANIMWRAPMODE_LOOP,
		//E_ANIMWRAPMODE_PINGPONG,
		E_ANIMWRAPMODE_CLAMPLAST,		// after finish, stop in the last frame
		E_ANIMWRAPMODE_CLAMFIRST,		// after finish, it resets to first frame
		E_ANIMWRAPMODE_ONCE,			// after finish, play the previous clip that was playing
	};
	enum E_ANIMSTATE{

		E_ANIMSTATE_STOPPED,
		E_ANIMSTATE_PLAYING,
		E_ANIMSTATE_PAUSED,
	};

	//========================================================================
	// 
	//========================================================================
	struct AnimationClip{

		struct StateData{

			unsigned int		itCurrentFrame;
			animTimeUnit		deltaAccum;
			E_ANIMSTATE			eState;

			//------------------------------------------------------------------------
			// 
			//------------------------------------------------------------------------
			StateData()
				:
			itCurrentFrame(0),deltaAccum((animTimeUnit)0.0),eState(E_ANIMSTATE_STOPPED)
			{}

		}stateData;

		struct ConfigData{

			E_ANIMWRAPMODE		eWrapMode;
			animTimeUnit		SPF;
			std::vector<frame>	vFrames;
			std::string			szName;

		}configData;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		frame GetCurrentFrame() const{

			return configData.vFrames[stateData.itCurrentFrame];
		}
	};

	//========================================================================
	// 
	//========================================================================
	struct SpriteFrame{

		unsigned int iSpriteUsedIndex;
		float fW, fH;			// if w and h differs from original sprite,
		float xOffset, yOffset;	// offsets are need to let the sprite on the relative same position
		DirectX::XMFLOAT4 uvRect;
	};

	//========================================================================
	// 
	//========================================================================
	struct TextureID_Binder_Pair{

		dx::BindPSShaderResourceView *	pBindPSSRV;
		int								iID;
	};

	//========================================================================
	// 
	//========================================================================
	class SpriteAnimationComponent: public Component{

		friend class SpriteAnimationSystem;
		friend class SpriteAnimationComponentFactory;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteAnimationComponent();

		//------------------------------------------------------------------------
		// aux
		//------------------------------------------------------------------------
		unsigned int GetClipIndexFromName( const char * szClipName_p );

		//------------------------------------------------------------------------
		// animation ctrl
		//------------------------------------------------------------------------
		void PlayClip( unsigned int iClip_p, bool bPauseCurrent_dontStop_p = false );
		void StopClip( unsigned int iClip_p );
		void PauseClip( unsigned int iClip_p );

		void Play(){ PlayClip(m_iCurrentClip);}
		void Stop(){ StopClip(m_iCurrentClip);}
		void Pause(){ PauseClip(m_iCurrentClip);}

		//------------------------------------------------------------------------
		// meticulous ctrl
		//------------------------------------------------------------------------
		void SetSpriteFrame( frame frame_p, bool bPauseCurrent_dontStop_p = true );
		void SetClipFrame( unsigned int frameIndex_p );

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		SpriteFrame GetFrame(){	return m_vFrames[m_currentFrame]; }

		TextureID_Binder_Pair GetSprite(){ return m_vSprites[m_vFrames[m_currentFrame].iSpriteUsedIndex]; }

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Update( animTimeUnit delta_p );

		//------------------------------------------------------------------------
		// return true if needs to quit updating
		//------------------------------------------------------------------------
		bool OnFrameWrap( AnimationClip & currentClip );

		unsigned int								m_iCurrentClip;
		unsigned int								m_iPreviousClip;
		frame										m_currentFrame;
		frame										m_previousFrame;

		std::vector<SpriteFrame>					m_vFrames;
		std::vector<AnimationClip>					m_vClips;
		std::vector<TextureID_Binder_Pair>			m_vSprites;
	};

	typedef gen::pool_ptr<SpriteAnimationComponent> pool_SpriteAnimCompo_ptr;
	typedef std::shared_ptr<SpriteAnimationComponent> shared_SpriteAnimationComponent_ptr;
	typedef std::weak_ptr<SpriteAnimationComponent> weak_SpriteAnimationComponent_ptr;

	//========================================================================
	// 
	//========================================================================
	class SpriteAnimationComponentFactory: public AComponentFactory{

		friend class SpriteAnimationSystem;

	public:

		gen::Pool<SpriteAnimationComponent>	  m_pool;
		sprite::SpriteRenderer						* m_pSpriteRenderer;

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteAnimationComponentFactory( unsigned int maxComponents_p, sprite::SpriteRenderer * pSpriteRenderer_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void LoadSpritesFromGFig( text::GfigElementA * pGFig_p, SpriteAnimationComponent * compo_p );
		void LoadFramesFromGFig(  text::GfigElementA * pGFig_p, SpriteAnimationComponent * compo_p );
		void LoadClipsFromGFig(   text::GfigElementA * pGFig_p, SpriteAnimationComponent * compo_p );
		E_ANIMWRAPMODE GetWrapModeFromGFig( text::GfigElementA * pGFig_p );
		void LoadClipFramesFromGFig(   text::GfigElementA * pGFig_p, SpriteAnimationComponent * compo_p );
		void LoadXYWHFromGfig( text::GfigElementA * pGFig_p, DirectX::XMFLOAT4 & rect_p );

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		pool_Component_ptr VCreateComponent(){ assert(0); return pool_Component_ptr();} //TODO
		pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );
	};

	typedef std::shared_ptr<SpriteAnimationComponentFactory> shared_SpriteAnimationComponentFactory_ptr;
	typedef std::weak_ptr<SpriteAnimationComponentFactory> weak_SpriteAnimationComponentFactory_ptr;
}
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

		E_ANIMWRAPMODE_LOOP = 0,
		//E_ANIMWRAPMODE_PINGPONG,
		E_ANIMWRAPMODE_CLAMPLAST,		// after finish, stop in the last frame
		E_ANIMWRAPMODE_CLAMFIRST,		// after finish, it resets to first frame
		E_ANIMWRAPMODE_ONCE,			// after finish, play the previous clip that was playing
	};

	static const char* s_szAnimWrapModes[] = {

		"loop",
		"clamplast",
		"clampfirst",
		"once"
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

		bool UVRectCompareDifferent ( const DirectX::XMFLOAT4 & a, const DirectX::XMFLOAT4 & b ) const {

			if( a.x != b.x 
				||
				a.y != b.y
				||
				a.z != b.z
				||
				a.w != b.w	)

				return true;

			return false;
		}
		bool operator == ( const SpriteFrame & other_p ) const{

			if( iSpriteUsedIndex != other_p.iSpriteUsedIndex 
				||
				fW != other_p.fW
				||
				fH != other_p.fH
				||
				xOffset != other_p.xOffset
				||
				yOffset != other_p.yOffset
				||
				UVRectCompareDifferent(uvRect, other_p.uvRect) )

				return false;

			return true;
		}
	};

	//========================================================================
	// 
	//========================================================================
	struct TextureID_Binder_Pair{

		dx::BindPSShaderResourceView *	pBindPSSRV;
		int								iID;

		bool operator == ( const TextureID_Binder_Pair & other_p ) const{

			return (iID == other_p.iID);
		}
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
		int GetNSprites() const { return (int) m_vSprites.size(); }

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

		gen::Pool<SpriteAnimationComponent>		m_pool;
		sprite::SpriteRenderer					* m_pSpriteRenderer;

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
		static void LoadXYWHFromGfig( text::GfigElementA * pGFig_p, DirectX::XMFLOAT4 & rect_p );

		static void LoadFrameFromGfig( text::GfigElementA * pGFig_p, SpriteFrame & spriteFrame_p );

		static void LoadSprite( TextureID_Binder_Pair & ID_Binder_p, SpriteAnimationComponent * compo_p );
		static int LoadSpriteIfNew( TextureID_Binder_Pair & ID_Binder_p, SpriteAnimationComponent * compo_p );
		static void LoadFrame( SpriteFrame & frame_p, SpriteAnimationComponent * compo_p );
		static int LoadFrameIfNew( SpriteFrame & frame_p, SpriteAnimationComponent * compo_p );
		static void LoadClip( AnimationClip::ConfigData & clip_p, SpriteAnimationComponent * compo_p );
		static void LoadClipFrame( int iClip_p, frame frame_p, SpriteAnimationComponent * compo_p );
		static void UpdateClipConfig( int iClip, AnimationClip::ConfigData & clip_p, SpriteAnimationComponent * compo_p );

		//------------------------------------------------------------------------
		// to be overridden
		//------------------------------------------------------------------------
		pool_Component_ptr VCreateComponent(){ /*assert(0); */return pool_Component_ptr(m_pool);} //TODO
		pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );
	};

	typedef std::shared_ptr<SpriteAnimationComponentFactory> shared_SpriteAnimationComponentFactory_ptr;
	typedef std::weak_ptr<SpriteAnimationComponentFactory> weak_SpriteAnimationComponentFactory_ptr;
}
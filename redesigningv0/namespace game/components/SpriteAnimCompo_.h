#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2016/09/27
	created:	27:9:2016   23:23
	file:		SpriteAnimCompo_.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../Component.h"
#include "../../namespace gen/Pool.h"
#include "../ComponentFactory.h"

#include "SpriteAnimCache.h"

namespace sprite{

	class SpriteRenderer;
}

namespace game{

	struct AnimClipState{

		animTimeUnit		deltaAccum;				// 4		// 0
		animTimeUnit		speedMod;				// 4		// 4
		E_ANIMSTATE_			eState;				// 4		// 8
		uint32_t			iCurrentFrame;			// 4		// 12
																// 16 (m of 4)
	};
	struct AnimClip{

		AnimClipState		state;					// 16		// 0
		AnimClipConfig *	pConfig;				// 8		// 16
																// 24
		iframe GetCurrentFrame()const{
			return pConfig->pFrameSequence[state.iCurrentFrame];
		}
	};

	//========================================================================
	// 
	//========================================================================
	class SpriteAnimCompo_ : public Component{

		friend class SpriteAnimCompo_Factory;
		friend class SpriteAnimationSystem;

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		SpriteAnimCompo_(){

			m_type = COMPONENT_TYPE(SpriteAnimCompo_);

			m_iPreviousClip = m_iCurrentClip = 0;
			m_iPreviousFrame = m_iCurrentFrame = (iframe)-1;
		}

		//------------------------------------------------------------------------
		// Controls
		//------------------------------------------------------------------------
		void StopClip( iframe iClip ){

			AnimClip & clip = m_pClips[iClip];

			clip.state.deltaAccum = (animTimeUnit)0.0;
			clip.state.eState = E_ANIMSTATE__STOPPED;
			clip.state.iCurrentFrame = 0;
		}
		void PauseClip( iframe iClip ){

			AnimClip & clip = m_pClips[iClip];

			clip.state.eState = E_ANIMSTATE__PAUSED;
		}
		void ResumeClip( iframe iClip, bool pauseCurrent = false ){

			pauseCurrent ? PauseClip(m_iCurrentClip) : StopClip(m_iCurrentClip);

			m_iPreviousClip = m_iCurrentClip;
			m_iCurrentClip = iClip;

			AnimClip & clip = m_pClips[iClip];
			clip.state.eState = E_ANIMSTATE__PLAYING;
		}
		void PlayClip( iframe iClip, bool pauseCurrent = false ){

			pauseCurrent ? PauseClip(m_iCurrentClip) : StopClip(m_iCurrentClip);

			m_iPreviousClip = m_iCurrentClip;
			m_iCurrentClip = iClip;

			AnimClip & clip = m_pClips[iClip];
			clip.state.deltaAccum = (animTimeUnit)0.0;
			clip.state.eState = E_ANIMSTATE__PLAYING;
			clip.state.iCurrentFrame = 0;
		}
		
		void Stop(){StopClip(m_iCurrentClip);}
		void Pause(){PauseClip(m_iCurrentClip);}
		void Resume(){
			AnimClip & clip = m_pClips[m_iCurrentClip];
			clip.state.eState = E_ANIMSTATE__PLAYING;
		}
		void Play(){
			AnimClip & clip = m_pClips[m_iCurrentClip];
			clip.state.deltaAccum = (animTimeUnit)0.0;
			clip.state.eState = E_ANIMSTATE__PLAYING;
			clip.state.iCurrentFrame = 0;
		}

		//------------------------------------------------------------------------
		// info
		//------------------------------------------------------------------------
		bool IsPlaying() const{
			AnimClip & clip = m_pClips[m_iCurrentClip];
			return clip.state.eState == E_ANIMSTATE__PLAYING;
		}


		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		const SpriteFrame_& GetCurrentFrame()const{
			return m_pSpriteFrames[m_iCurrentFrame];
		}
		const TextureID_Binder_Pair_ & GetSprite( iframe sprite )const{
			return m_pSprites[sprite];
		}
		iframe GetCurrentClip()const{ return m_iCurrentClip;}
		uint32_t GetCurrentClipSequence()const{ return m_pClips[m_iCurrentClip].state.iCurrentFrame;}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void SetClipSpeedMod( animTimeUnit mod ){
			AnimClip & clip = m_pClips[m_iCurrentClip];
			clip.state.speedMod = mod;
		}

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Update( animTimeUnit delta_p){

			AnimClip * pClip = &m_pClips[m_iCurrentClip];
			pClip->state.deltaAccum += delta_p * pClip->state.speedMod;

			while( pClip->state.deltaAccum > pClip->pConfig->SPF){

				pClip->state.deltaAccum -= pClip->pConfig->SPF;
				++pClip->state.iCurrentFrame;

				if( pClip->state.iCurrentFrame == pClip->pConfig->nFrames ){

					if( OnClipWrap(pClip) ) break;
				}
			}

			m_iCurrentFrame = pClip->GetCurrentFrame();
		}

		bool OnClipWrap( AnimClip *& pClip ){

			switch( pClip->pConfig->eWrapMode ){

			case E_ANIMWRAPMODE__LOOP:
				pClip->state.iCurrentFrame = 0;
				return false;
				break;
			case E_ANIMWRAPMODE__CLAMPLAST:
				pClip->state.deltaAccum = (animTimeUnit)0.0;
				pClip->state.eState = E_ANIMSTATE__STOPPED;
				pClip->state.iCurrentFrame = pClip->pConfig->nFrames-1;
				return true;
				break;
			case E_ANIMWRAPMODE__CLAMFIRST:
				pClip->state.deltaAccum = (animTimeUnit)0.0;
				pClip->state.eState = E_ANIMSTATE__STOPPED;
				pClip->state.iCurrentFrame = 0;
				return true;
				break;
			case E_ANIMWRAPMODE__ONCE:
				PlayClip(m_iPreviousClip);
				pClip = &m_pClips[m_iCurrentClip];
				return false;
				break;
			}
			return true;
		}

		iframe		m_iPreviousClip;	// 0
		iframe		m_iCurrentClip;		// 1
		iframe		m_iPreviousFrame;	// 2
		iframe		m_iCurrentFrame;	// 3
		

		uint32_t	m_nClips;			// 4
		AnimClip *	m_pClips;			// 8
		SpriteFrame_ * m_pSpriteFrames;	// 16
		TextureID_Binder_Pair_ * m_pSprites; // 24
	
		uint16_t	m_nFrames;			// 32
		uint16_t	m_nSprites;			// 34
		uint32_t	m_fileID;			// 36
										// 40
	};

	typedef gen::pool_ptr<SpriteAnimCompo_> pool_SpriteAnimCompo__ptr;

	//========================================================================
	//========================================================================
	//========================================================================
	//========================================================================
	//========================================================================
	//========================================================================
	//========================================================================
	//========================================================================
	//======================================================================== 


	class SpriteAnimCompo_Factory: public IComponentFactory{

		enum{

			E_MAXANIMCLIPS = 256
		};

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteAnimCompo_Factory( unsigned int maxComponents_p, sprite::SpriteRenderer * pSpriteRenderer_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void LoadAnimFromGfig( SpriteAnimCompo_ * pCompo,  const char* szFilename );
		static void LoadStateFromGFig( text::GfigElementA * pGFig_p, SpriteAnimCompo_ * compo_p );
		static void LoadClipStateFromGfig( text::GfigElementA * pGFig_p, AnimClipState * state_p );
		static E_ANIMSTATE_ GetClipAnimStateFromGfig( text::GfigElementA * pGFig_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		static void SerializeAnimState( SpriteAnimCompo_ * pCompo, SpriteAnimCompo_ * pDef, text::GfigElementA * pGFig_p, AnimClipState * pClipDef_p );
		static void SerializeClipState( int itC, AnimClipState * state_p, AnimClipState * pDef, text::GfigElementA * pGFig_p );

		//------------------------------------------------------------------------
		// overridden
		//------------------------------------------------------------------------
		pool_Component_ptr VCreateComponent(){ return pool_Component_ptr(m_pool);}
		pool_Component_ptr VCreateComponent( text::GfigElementA * pGFig_p );
		pool_Component_ptr VCloneComponent( const Component * pCompo_p );
		void VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p );

		void VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p );
		virtual void VSerialize( const Component * pCompo_p, const Component * pDefaults_p, text::GfigElementA * pGFig_p );

		virtual Component* VGetDefaultCompo(){ return &m_defaults;}

		gen::Pool<SpriteAnimCompo_>		m_pool;
		sprite::SpriteRenderer			* m_pSpriteRenderer;
		SpriteAnimCompo_				m_defaults;
		AnimClipState					m_defaultClipState;

		gen::Stack<AnimClip, E_MAXANIMCLIPS> m_clipsMemory;

		SpriteAnimCache					m_cache;
	};

	typedef std::shared_ptr<SpriteAnimCompo_Factory> shared_SpriteAnimCompo_Factory_ptr;
}
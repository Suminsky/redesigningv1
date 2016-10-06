#include "PCHF.h"
#include "SpriteAnimCompo_.h"
#include "../../namespace text/TextParser.h"
#include "../../namespace sprite/SpriteRenderer.h"

using namespace game;
using namespace sprite;
using namespace text;
using namespace gen;

SpriteAnimCompo_Factory::SpriteAnimCompo_Factory( unsigned int maxComponents_p, SpriteRenderer * pSpriteRenderer_p )
	:
m_pool(maxComponents_p),
m_pSpriteRenderer(pSpriteRenderer_p),
m_cache(&pSpriteRenderer_p->m_tex2D_cache)
{
	m_defaults.m_iPreviousClip = m_defaults.m_iCurrentClip = 0;
	m_defaults.m_iPreviousFrame = m_defaults.m_iCurrentFrame = (iframe)-1;
	m_defaults.m_pClips = nullptr;
	m_defaults.m_fileID = (uint32_t)-1;
	m_defaults.m_pClips = nullptr;

	m_defaultClipState.deltaAccum = (animTimeUnit)0.0;
	m_defaultClipState.speedMod = (animTimeUnit)1.0;
	m_defaultClipState.eState = E_ANIMSTATE__STOPPED;
	m_defaultClipState.iCurrentFrame = 0;

	memset( m_clipsMemory.StackAlloc(E_MAXANIMCLIPS), 0, sizeof(AnimClip)*E_MAXANIMCLIPS );

	m_clipsMemory.Reset();
}

pool_Component_ptr SpriteAnimCompo_Factory::VCreateComponent( GfigElementA * pGFig_p )
{
	pool_SpriteAnimCompo__ptr pAnimCompo( m_pool );

	GfigElementA * pGFigParam = nullptr;

	if( pGFig_p->GetSubElement("file", pGFigParam)){

		LoadAnimFromGfig( pAnimCompo.Get(), pGFigParam->m_value.c_str());
	}
	if( pGFig_p->GetSubElement("state", pGFigParam)){

		LoadStateFromGFig(pGFigParam, pAnimCompo.Get());
	}

	return pAnimCompo;
}

pool_Component_ptr SpriteAnimCompo_Factory::VCloneComponent( const Component * pCompo_p )
{
	pool_SpriteAnimCompo__ptr pAnimCompo( m_pool );
	SpriteAnimCompo_ * pOther = (SpriteAnimCompo_*)pCompo_p;

	pAnimCompo->m_iPreviousClip= pOther->m_iPreviousClip;
	pAnimCompo->m_iCurrentClip = pOther->m_iCurrentClip;
	pAnimCompo->m_iPreviousFrame=pOther->m_iPreviousFrame;
	pAnimCompo->m_iCurrentFrame= pOther->m_iCurrentFrame;

	pAnimCompo->m_nClips		=pOther->m_nClips;
	pAnimCompo->m_pClips = m_clipsMemory.StackAlloc( pAnimCompo->m_nClips );
	memcpy( pAnimCompo->m_pClips, pOther->m_pClips, sizeof(AnimClip) * pAnimCompo->m_nClips);

	pAnimCompo->m_pSpriteFrames =pOther->m_pSpriteFrames;
	pAnimCompo->m_pSprites		=pOther->m_pSprites;
	pAnimCompo->m_nFrames		=pOther->m_nFrames;
	pAnimCompo->m_nSprites		=pOther->m_nSprites;

	pAnimCompo->m_fileID		=pOther->m_fileID;

	return pAnimCompo;
}

void SpriteAnimCompo_Factory::VUpdateComponent( Component * pCompo_p, GfigElementA * pGFig_p )
{
	SpriteAnimCompo_ * pAnimCompo = (SpriteAnimCompo_*)pCompo_p;

	GfigElementA * pGFigParam;
	if( pGFig_p->GetSubElement("state", pGFigParam)){

		LoadStateFromGFig(pGFigParam, pAnimCompo);
	}
}

void SpriteAnimCompo_Factory::VSerialize( const Component * pCompo_p, GfigElementA * pGFig_p )
{
	SpriteAnimCompo_ & anim = *((SpriteAnimCompo_*)pCompo_p);

	pGFig_p->m_subElements.emplace_back( GfigElementA(COMPONENT_NAME(SpriteAnimCompo_)) );
	GfigElementA & animGfig = pGFig_p->m_subElements.back();

	animGfig.m_subElements.emplace_back(GfigElementA("file", m_cache.GetFilename(anim.m_fileID)) );

	SerializeAnimState(&anim, &m_defaults, &animGfig, &m_defaultClipState);
}

void game::SpriteAnimCompo_Factory::VSerialize( const Component * pCompo_p, const Component * pDefaults_p, text::GfigElementA * pGFig_p )
{
	SpriteAnimCompo_ & anim = *((SpriteAnimCompo_*)pCompo_p);
	SpriteAnimCompo_ & def = *((SpriteAnimCompo_*)pDefaults_p);

	pGFig_p->m_subElements.emplace_back( GfigElementA(COMPONENT_NAME(SpriteAnimCompo_)) );
	GfigElementA & animGfig = pGFig_p->m_subElements.back();
	
	if( anim.m_fileID != def.m_fileID ){

		animGfig.m_subElements.emplace_back(GfigElementA("file", m_cache.GetFilename(anim.m_fileID)) );
		SerializeAnimState(&anim, &m_defaults, &animGfig, &m_defaultClipState);
	}
	else{

		SerializeAnimState(&anim, &def, &animGfig, &m_defaultClipState);
	}

	if( animGfig.m_subElements.size() == 0 )
		pGFig_p->m_subElements.pop_back();
}

//========================================================================
void SpriteAnimCompo_Factory::SerializeAnimState( SpriteAnimCompo_ * pCompo, SpriteAnimCompo_ * pDef, GfigElementA * pGFig_p, AnimClipState * pClipDef_p )
{
	GfigElementA stateGfig("state");

	if( pCompo->m_iCurrentClip != pDef->m_iCurrentClip ){

		stateGfig.m_subElements.push_back( GfigElementA("clip", std::to_string((_Longlong)pCompo->m_iCurrentClip).c_str() ) );
	}
	if( pCompo->m_iPreviousClip != pDef->m_iPreviousClip ){

		stateGfig.m_subElements.push_back( GfigElementA("prev clip", std::to_string((_Longlong) pCompo->m_iPreviousClip).c_str() ) );
	}
	if( pCompo->m_iCurrentFrame != pDef->m_iCurrentFrame){

		stateGfig.m_subElements.push_back( GfigElementA("frame", std::to_string((_Longlong)pCompo->m_iCurrentFrame).c_str() ) );
	}
	if( pCompo->m_iPreviousFrame != pDef->m_iPreviousFrame ){

		stateGfig.m_subElements.push_back( GfigElementA("prev frame", std::to_string((_Longlong)pCompo->m_iPreviousFrame).c_str() ) );
	}

	GfigElementA clipStatesGfig("clip states");
	if( pDef->m_pClips == nullptr ){

		for( int c = 0, n = pCompo->m_nClips; c < n; ++c ){

			SerializeClipState( c, &pCompo->m_pClips[c].state, pClipDef_p, &clipStatesGfig );
		}
	}
	else{

		assert( pCompo->m_fileID == pDef->m_fileID );
		for( int c = 0, n = pCompo->m_nClips; c < n; ++c ){

			SerializeClipState( c, &pCompo->m_pClips[c].state, &pDef->m_pClips[c].state, &clipStatesGfig );
		}
	}

	if( clipStatesGfig.m_subElements.size() != 0 ){

		stateGfig.m_subElements.push_back(std::move(clipStatesGfig));
	}
	if( stateGfig.m_subElements.size() != 0 ){

		pGFig_p->m_subElements.push_back(std::move(stateGfig));
	}
}

void SpriteAnimCompo_Factory::SerializeClipState( int itC, AnimClipState * state_p, AnimClipState * pDef, GfigElementA * pGFig_p )
{
	GfigElementA clipStateGfig( std::to_string((_Longlong)itC));

	if( state_p->deltaAccum != pDef->deltaAccum ){

		clipStateGfig.m_subElements.push_back( GfigElementA("delta", std::to_string((long double)state_p->deltaAccum).c_str() ) );
	}
	if( state_p->speedMod != pDef->speedMod ){

		clipStateGfig.m_subElements.push_back( GfigElementA("speed", std::to_string((long double)state_p->speedMod).c_str()));
	}

	const char * szAnimClipStates[] = {
		"stopped",
		"playing",
		"paused"
	};
	if( state_p->eState != pDef->eState ){

		clipStateGfig.m_subElements.push_back( GfigElementA("state", szAnimClipStates[state_p->eState] ));
	}
	if( state_p->iCurrentFrame != pDef->iCurrentFrame ){

		clipStateGfig.m_subElements.push_back( GfigElementA("frame", std::to_string((_Longlong)state_p->iCurrentFrame).c_str()));
	}

	if( clipStateGfig.m_subElements.size() != 0 ){

		pGFig_p->m_subElements.push_back(std::move(clipStateGfig));
	}
}

//========================================================================
void SpriteAnimCompo_Factory::LoadAnimFromGfig(  SpriteAnimCompo_ * pCompo, const char* szFilename )
{
	uint32_t id = 0;
	const uint8_t * pData = m_cache.GetAnimData(szFilename, &id);

	// load anim to the compo here
	DataStream databuff;
	databuff.m_data = (unsigned char *)pData;
	databuff.m_currentByteIndex = 0;

	AnimDataHeader *pHeader = databuff.GetNextChunkPtr<AnimDataHeader>();
	pCompo->m_nClips = pHeader->nClips;
	pCompo->m_pClips = m_clipsMemory.StackAlloc( pCompo->m_nClips ); // allocate clip state mem

	// initialize each clip cgfPointer to clip config

	for( int c = 0, n = pHeader->nClips; c < n; ++c ){

		pCompo->m_pClips[c].pConfig = pHeader->ppClipsPtrs[c];
		pCompo->m_pClips[c].state.speedMod = 1.0f;
	}

	pCompo->m_pSpriteFrames = pHeader->pFrames;
	pCompo->m_pSprites = pHeader->pSprites;
	pCompo->m_nFrames = pHeader->nFrames;
	pCompo->m_nSprites = pHeader->nSprites;
	pCompo->m_fileID = id;
}

void SpriteAnimCompo_Factory::LoadStateFromGFig( GfigElementA * pGFig_p, SpriteAnimCompo_ * compo_p )
{
	GfigElementA * pGFigParam;

	if( pGFig_p->GetSubElement("clip", pGFigParam ) ){
		compo_p->m_iCurrentClip = (iframe)atoi(pGFigParam->m_value.c_str());
	}
	if( pGFig_p->GetSubElement("prev clip", pGFigParam ) ){
		compo_p->m_iPreviousClip = (iframe)atoi(pGFigParam->m_value.c_str());
	}

	if( pGFig_p->GetSubElement("frame", pGFigParam ) ){
		compo_p->m_iCurrentFrame = (iframe)atoi(pGFigParam->m_value.c_str());
	}
	if( pGFig_p->GetSubElement("prev frame", pGFigParam ) ){
		compo_p->m_iPreviousFrame = (iframe)atoi(pGFigParam->m_value.c_str());
	}

	if( pGFig_p->GetSubElement("clip states", pGFigParam ) ){
		
		for( int c = 0, n = (int)pGFigParam->m_subElements.size(); c < n; ++c ){

			int itClip = atoi(pGFigParam->m_subElements[c].m_name.c_str());

			AnimClipState * pClip = &compo_p->m_pClips[itClip].state;

			LoadClipStateFromGfig( &pGFigParam->m_subElements[c], pClip );
		}
	}
}

void SpriteAnimCompo_Factory::LoadClipStateFromGfig( GfigElementA * pGFig_p, AnimClipState * state_p )
{
	//unsigned int		itCurrentFrame;
	//animTimeUnit		deltaAccum;
	//animTimeUnit		speedMod;
	//E_ANIMSTATE_			eState;

	GfigElementA * pGFigParam;

	if( pGFig_p->GetSubElement( "frame", pGFigParam ) ){
		state_p->iCurrentFrame = atoi( pGFigParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "delta", pGFigParam ) ){
		state_p->deltaAccum = (animTimeUnit)atof( pGFigParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "speed", pGFigParam ) ){
		state_p->speedMod = (animTimeUnit)atof( pGFigParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "state", pGFigParam ) ){
		state_p->eState = GetClipAnimStateFromGfig(pGFigParam);
	}
}

E_ANIMSTATE_ SpriteAnimCompo_Factory::GetClipAnimStateFromGfig( GfigElementA * pGFig_p )
{
	const char * szAnimClipStates[] = {
		"stopped",
		"playing",
		"paused"
	};
	E_ANIMSTATE_ states[] = {
		E_ANIMSTATE__STOPPED,
		E_ANIMSTATE__PLAYING,
		E_ANIMSTATE__PAUSED
	};

	for( int it = 0; it < 3; ++it ){

		if( pGFig_p->m_value == szAnimClipStates[it] ){

			return states[it];
		}
	}

	return E_ANIMSTATE__STOPPED;
}
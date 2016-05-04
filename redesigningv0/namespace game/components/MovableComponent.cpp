#include "MovableComponent.h"

#include "../Object.h"
#include "../../namespace text/TextParser.h"


using namespace text;
using namespace DirectX;


void MovableComponent::VOnAttach()
{
	pTrafoRef = m_pObjectOwner->GetFirstOfComponent<TransformComponent>().Get();
}

void MovableComponent::SetGravity( float fGrav_p )
{
	m_config.fGravity = fGrav_p;
}

void MovableComponent::SetMaxVel( const float param1 )
{
	m_config.fMaxVel = param1;
}

//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================
//======================================================================== 
//========================================================================
//========================================================================
//========================================================================

game::pool_Component_ptr MovableFactory::VCreateComponent( text::GfigElementA * pGFig_p )
{
	gen::pool_ptr<MovableComponent> pMovable( m_pool );
	GfigElementA * pParam = nullptr;

	MovableComponent::config cfg;// = {0};
	MovableComponent::state state;// = {0};
	memset( &cfg, 0, sizeof(MovableComponent::config));
	memset( &state, 0, sizeof(MovableComponent::state));

	if( pGFig_p->GetSubElement( "gravity", pParam ) ){

		cfg.fGravity = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "jump speed", pParam ) ){

		cfg.fJumpSpeed = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "max fall vel", pParam ) ){

		cfg.fMaxFallVel = (float)atof( pParam->m_value.c_str() );
	}

	if( pGFig_p->GetSubElement( "max vel", pParam ) ){

		cfg.fMaxVel = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "accel", pParam ) ){

		cfg.fAccel = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "deccel", pParam ) ){

		cfg.fDeccel = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "stop vel", pParam ) ){

		cfg.fVelStopThreshold = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "air deccel", pParam ) ){

		cfg.fAirDeccel = (float)atof( pParam->m_value.c_str() );
	}

	if( pGFig_p->GetSubElement( "face dir", pParam ) ){

		state.vFaceDir = GetXYZFromGfig( pParam );
	}
	if( pGFig_p->GetSubElement( "move dir", pParam ) ){

		state.vMoveDir = GetXYZFromGfig( pParam );
	}
	if( pGFig_p->GetSubElement( "vel", pParam ) ){

		state.vVel = GetXYZFromGfig( pParam );
	}
	if( pGFig_p->GetSubElement( "grounded", pParam ) ){

		state.bGrounded = true;
	}


	pMovable->m_state = state;
	pMovable->m_config = cfg;

	return pMovable;
}

XMFLOAT3 MovableFactory::GetXYZFromGfig( GfigElementA * pGFig_p )
{
	XMFLOAT3 vector(0.0f, 0.0f, 0.0f);

	GfigElementA * pElement;

	if( pGFig_p->GetSubElement( "x", pElement) ){
		vector.x = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "y", pElement) ){
		vector.y = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "z", pElement) ){
		vector.z = (float)atof(pElement->m_value.c_str());
	}

	return vector;
}

game::pool_Component_ptr MovableFactory::VCloneComponent( const Component * pCompo_p )
{
	gen::pool_ptr<MovableComponent> pMovable( m_pool );

	MovableComponent * pOther = (MovableComponent*)pCompo_p;

	pMovable->m_config = pOther->m_config;
	pMovable->m_state = pOther->m_state;

	return pMovable;
}

void MovableFactory::VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p )
{

	MovableComponent * pMov = (MovableComponent*)pCompo_p;
	GfigElementA * pParam = nullptr;

	if( pGFig_p->GetSubElement( "gravity", pParam ) ){

		pMov->m_config.fGravity = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "jump speed", pParam ) ){

		pMov->m_config.fJumpSpeed = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "max fall vel", pParam ) ){

		pMov->m_config.fMaxFallVel = (float)atof( pParam->m_value.c_str() );
	}

	if( pGFig_p->GetSubElement( "max vel", pParam ) ){

		pMov->m_config.fMaxVel = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "accel", pParam ) ){

		pMov->m_config.fAccel = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "deccel", pParam ) ){

		pMov->m_config.fDeccel = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "stop vel", pParam ) ){

		pMov->m_config.fVelStopThreshold = (float)atof( pParam->m_value.c_str() );
	}
	if( pGFig_p->GetSubElement( "air deccel", pParam ) ){

		pMov->m_config.fAirDeccel = (float)atof( pParam->m_value.c_str() );
	}
//

	if( pGFig_p->GetSubElement( "face dir", pParam ) ){

		pMov->m_state.vFaceDir = GetXYZFromGfig( pParam );
	}
	if( pGFig_p->GetSubElement( "move dir", pParam ) ){

		pMov->m_state.vMoveDir = GetXYZFromGfig( pParam );
	}
	if( pGFig_p->GetSubElement( "vel", pParam ) ){

		pMov->m_state.vVel = GetXYZFromGfig( pParam );
	}
	if( pGFig_p->GetSubElement( "grounded", pParam ) ){

		pMov->m_state.bGrounded = true;
	}
}

void MovableFactory::VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p )
{
	/*
	[MovableComponent
		[gravity = 2000]#2000
		[speed = 0]
		[jump speed = 500]
		[jump accel = 100]
		[max fall vel = 4000]
		[face dir
			[x = 1][y = 0][z = 0]
		]
		[move dir
			[x = 0][y = 0][z = 0]
		]
	]
	 */

	MovableComponent & mov = *((MovableComponent*)pCompo_p);

	pGFig_p->m_subElements.emplace_back( GfigElementA(COMPONENT_NAME(MovableComponent)) );

	GfigElementA & gMovCompo = pGFig_p->m_subElements.back();

	// TODO, update missing members
	const float values[] = {
		mov.m_config. fAccel,
		mov.m_config. fMaxVel,
		mov.m_config. fDeccel,
		mov.m_config. fVelStopThreshold,
		mov.m_config. fGravity,
		mov.m_config. fJumpSpeed,
		mov.m_config. fMaxFallVel,
		mov.m_config. fAirDeccel
	};
	const char* names[] = {
		"accel",
		"max vel",
		"deccel",
		"stop vel",
		"gravity",
		"jump speed",
		"max fall vel",
		"air deccel"
	};

	for( int it = 0; it < 8; ++it ){

		if( values[it] != 0.0f )
			gMovCompo.m_subElements.push_back(GfigElementA(names[it], std::to_string((long double)values[it]).c_str()) );
	}

	GfigElementA gFaceDir("face dir");
	SerlializeXYZ(mov.m_state.vFaceDir, gFaceDir);
	if(gFaceDir.m_subElements.size())
		gMovCompo.m_subElements.push_back(std::move(gFaceDir));

	GfigElementA gMoveDir("move dir");
	SerlializeXYZ(mov.m_state.vMoveDir, gMoveDir);
	if(gMoveDir.m_subElements.size())
		gMovCompo.m_subElements.push_back(std::move(gMoveDir));

	GfigElementA gVel("vel");
	SerlializeXYZ(mov.m_state.vVel, gVel);
	if(gVel.m_subElements.size())
		gMovCompo.m_subElements.push_back(std::move(gVel));

	if( mov.m_state.bGrounded ){

		gMovCompo.m_subElements.push_back(GfigElementA("grounded"));
	}
}

void MovableFactory::SerlializeXYZ( const DirectX::XMFLOAT3 & xyz_p, text::GfigElementA & gfig_p )
{
	// check if values are different than default values, since default values dont need to be loaded
	const float values[] = {
		xyz_p.x, xyz_p.y, xyz_p.z
	};
	const char * names[] = {
		"x", "y", "z",
	};

	for( int it = 0; it < 3; ++it ){

		if( values[it] != 0.0f )
			gfig_p.m_subElements.push_back(GfigElementA(names[it], std::to_string((long double)values[it] ).c_str()) );
	}
}

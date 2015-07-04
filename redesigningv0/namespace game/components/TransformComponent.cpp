#include "TransformComponent.h"
#include "../../namespace text/TextParser.h"

using namespace game;
using namespace DirectX;
using namespace text;


game::TransformComponent::TransformComponent()
{
	m_type = COMPONENT_TYPE(TransformComponent);

	m_offset.position = m_local.position =		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_offset.qRotation = m_local.qRotation =	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_offset.scale = m_local.scale =			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_bSnap = true;

	UpdateWorldAndFinalTransformation( m_local.DeriveMatrix() );

	m_node.SetData(this);
}

game::TransformComponent::~TransformComponent()
{
	BREAKHERE;
}

void game::TransformComponent::UpdateWorldAndFinalTransformation( const XMFLOAT4X4 & mParentWorldTrafo_p )
{
	m_previousFinal = m_final;

	// create matrices

	XMMATRIX mLocal = m_local.DeriveMatrix();
	XMMATRIX mOffset = m_offset.DeriveMatrix();

	XMMATRIX mParentWorld = XMLoadFloat4x4( &mParentWorldTrafo_p );

	// world

	XMMATRIX mWorld = XMMatrixMultiply( mLocal, mParentWorld );
	XMStoreFloat4x4( &m_world, mWorld );

	// final

	XMStoreFloat4x4( &m_final, XMMatrixMultiply( mOffset, mWorld ) );

	// TODO: move snapping to here
}

bool game::TransformComponent::BUpdateWorldAndFinalTransformation( const DirectX::XMFLOAT4X4 & mParentWorldTrafo_p )
{
	XMMATRIX mFinal = XMLoadFloat4x4( &m_final );
	XMMATRIX mPrevFinal = XMLoadFloat4x4( &m_previousFinal );

	bool bPreviousUpdated =
		XMVector4NotEqual( mPrevFinal.r[3], mFinal.r[3] ) // position first
		||
		XMVector4NotEqual( mPrevFinal.r[0], mFinal.r[0] )
		||
		XMVector4NotEqual( mPrevFinal.r[1], mFinal.r[1] )
		||
		XMVector4NotEqual( mPrevFinal.r[2], mFinal.r[2] );

	m_previousFinal = m_final;

	// create matrices

	XMMATRIX mLocal = m_local.DeriveMatrix();
	XMMATRIX mOffset = m_offset.DeriveMatrix();

	XMMATRIX mParentWorld = XMLoadFloat4x4( &mParentWorldTrafo_p );

	// world

	XMMATRIX mWorld = XMMatrixMultiply( mLocal, mParentWorld );
	XMStoreFloat4x4( &m_world, mWorld );

	// final
	mFinal = XMMatrixMultiply( mOffset, mWorld );
	XMStoreFloat4x4( &m_final, mFinal );

	mPrevFinal = XMLoadFloat4x4( &m_previousFinal );

	return	bPreviousUpdated
		||
		XMVector4NotEqual( mPrevFinal.r[3], mFinal.r[3] ) // position first
		||
		XMVector4NotEqual( mPrevFinal.r[0], mFinal.r[0] )
		||
		XMVector4NotEqual( mPrevFinal.r[1], mFinal.r[1] )
		||
		XMVector4NotEqual( mPrevFinal.r[2], mFinal.r[2] );
}

void game::TransformComponent::UpdateWorldAndFinalTransformation( const XMMATRIX & mParentWorldTrafo_p )
{
	m_previousFinal = m_final;

	// create matrices

	XMMATRIX mLocal = m_local.DeriveMatrix();
	XMMATRIX mOffset = m_offset.DeriveMatrix();

	// world

	XMMATRIX mWorld = XMMatrixMultiply( mLocal, mParentWorldTrafo_p );
	XMStoreFloat4x4( &m_world, mWorld );

	// final

	XMStoreFloat4x4( &m_final, XMMatrixMultiply( mOffset, mWorld ) );

	// TODO: move snapping to here
}

bool game::TransformComponent::BUpdateWorldAndFinalTransformation( const DirectX::XMMATRIX & mParentWorldTrafo_p )
{
	XMMATRIX mFinal = XMLoadFloat4x4( &m_final );
	XMMATRIX mPrevFinal = XMLoadFloat4x4( &m_previousFinal );

	bool bPreviousUpdated =
		XMVector4NotEqual( mPrevFinal.r[3], mFinal.r[3] ) // position first
		||
		XMVector4NotEqual( mPrevFinal.r[0], mFinal.r[0] )
		||
		XMVector4NotEqual( mPrevFinal.r[1], mFinal.r[1] )
		||
		XMVector4NotEqual( mPrevFinal.r[2], mFinal.r[2] );

	m_previousFinal = m_final;

	// create matrices

	XMMATRIX mLocal = m_local.DeriveMatrix();
	XMMATRIX mOffset = m_offset.DeriveMatrix();

	// world

	XMMATRIX mWorld = XMMatrixMultiply( mLocal, mParentWorldTrafo_p );
	XMStoreFloat4x4( &m_world, mWorld );

	// final
	mFinal = XMMatrixMultiply( mOffset, mWorld );
	XMStoreFloat4x4( &m_final, mFinal );

	mPrevFinal = XMLoadFloat4x4( &m_previousFinal );

	return	bPreviousUpdated
		||
		XMVector4NotEqual( mPrevFinal.r[3], mFinal.r[3] ) // position first
		||
		XMVector4NotEqual( mPrevFinal.r[0], mFinal.r[0] )
		||
		XMVector4NotEqual( mPrevFinal.r[1], mFinal.r[1] )
		||
		XMVector4NotEqual( mPrevFinal.r[2], mFinal.r[2] );
}

void game::TransformComponent::UpdateWorldAndFinalTransformation()
{
	m_previousFinal = m_final;

	// create matrices

	XMMATRIX mLocal = m_local.DeriveMatrix();
	XMMATRIX mOffset = m_offset.DeriveMatrix();

	// world

	XMMATRIX mWorld = mLocal;
	XMStoreFloat4x4( &m_world, mWorld );

	// final

	XMStoreFloat4x4( &m_final, XMMatrixMultiply( mOffset, mWorld ) );

	// TODO: move snapping to here
}

bool game::TransformComponent::BUpdateWorldAndFinalTransformation()
{
	XMMATRIX mPrevFinal = XMLoadFloat4x4( &m_previousFinal );
	XMMATRIX mFinal = XMLoadFloat4x4( &m_final );
	
	bool bPreviousUpdated =
		XMVector4NotEqual( mPrevFinal.r[3], mFinal.r[3] ) // position first
		||
		XMVector4NotEqual( mPrevFinal.r[0], mFinal.r[0] )
		||
		XMVector4NotEqual( mPrevFinal.r[1], mFinal.r[1] )
		||
		XMVector4NotEqual( mPrevFinal.r[2], mFinal.r[2] );

	//

	m_previousFinal = m_final;

	// create matrices

	XMMATRIX mOffset = m_offset.DeriveMatrix();
	XMMATRIX mLocal = m_local.DeriveMatrix();
	

	// world

	XMMATRIX mWorld = mLocal;
	XMStoreFloat4x4( &m_world, mWorld );

	// final
	mFinal = XMMatrixMultiply( mOffset, mWorld );
	XMStoreFloat4x4( &m_final, mFinal );

	mPrevFinal = XMLoadFloat4x4( &m_previousFinal );

	return	bPreviousUpdated
			||
			XMVector4NotEqual( mPrevFinal.r[3], mFinal.r[3] ) // position first
			||
			XMVector4NotEqual( mPrevFinal.r[0], mFinal.r[0] )
			||
			XMVector4NotEqual( mPrevFinal.r[1], mFinal.r[1] )
			||
			XMVector4NotEqual( mPrevFinal.r[2], mFinal.r[2] );

}

void game::TransformComponent::AddChild( TransformComponent * pTrafo_p )
{
	m_node.AddNode( &pTrafo_p->m_node);
}

void game::TransformComponent::RemoveChild( TransformComponent * pTrafo_p )
{
	m_node.RemoveNode( &pTrafo_p->m_node );
}

TransformComponent * game::TransformComponent::GetParent()const
{
	return m_node.GetParent()->GetData();
}

void game::TransformComponent::UpdateWorldAndFinalTransformation_NoPrevious( const DirectX::XMMATRIX & mParentWorldTrafo_p )
{
	//m_previousFinal = m_final;

	// create matrices

	XMMATRIX mLocal = m_local.DeriveMatrix();
	XMMATRIX mOffset = m_offset.DeriveMatrix();

	// world

	XMMATRIX mWorld = XMMatrixMultiply( mLocal, mParentWorldTrafo_p );
	XMStoreFloat4x4( &m_world, mWorld );

	// final

	XMStoreFloat4x4( &m_final, XMMatrixMultiply( mOffset, mWorld ) );

	// TODO: move snapping to here
}

void game::TransformComponent::UpdateWorldAndFinalTransformation_NoPrevious()
{
	// create matrices

	XMMATRIX mLocal = m_local.DeriveMatrix();
	XMMATRIX mOffset = m_offset.DeriveMatrix();

	// world

	XMMATRIX mWorld = mLocal;
	XMStoreFloat4x4( &m_world, mWorld );

	// final

	XMStoreFloat4x4( &m_final, XMMatrixMultiply( mOffset, mWorld ) );

	// TODO: move snapping to here
}

//========================================================================
// 
//========================================================================
game::pool_Component_ptr game::TransformComponentFactory::VCreateComponent()
{
	return pool_Component_ptr(m_pool);
}

pool_Component_ptr TransformComponentFactory::VCreateComponent( GfigElementA * pGFig_p )
{
	gen::pool_ptr<TransformComponent> pTransform( m_pool );

	UpdateDataFromGfig( *pTransform, pGFig_p );

	return pTransform;
}

Trafo TransformComponentFactory::GetTrafoFromGfig( GfigElementA * pGFig_p )
{
	Trafo trafo;

	GfigElementA * pPropertie = nullptr;

	if( pGFig_p->GetSubElement( "pos", pPropertie ) ){

		trafo.position = GetXYZWFromGfig(pPropertie);
	}
	else{

		trafo.position = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ); 
	}
	if( pGFig_p->GetSubElement( "rotation", pPropertie ) ){

		trafo.qRotation = GetXYZWFromGfig(pPropertie);
	}
	else{

		trafo.qRotation = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ); 
	}
	if( pGFig_p->GetSubElement( "scale", pPropertie ) ){

		trafo.scale = GetXYZWFromGfig(pPropertie);
	}
	else{

		trafo.scale = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	return trafo;
}

DirectX::XMFLOAT4 game::TransformComponentFactory::GetXYZWFromGfig( text::GfigElementA * pGFig_p )
{
	XMFLOAT4 vector(0.0f, 0.0f, 0.0f, 1.0f);

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
	if( pGFig_p->GetSubElement( "w", pElement) ){
		vector.w = (float)atof(pElement->m_value.c_str());
	}

	return vector;
}

game::pool_Component_ptr game::TransformComponentFactory::VCloneComponent( const Component * pCompo_p )
{
	gen::pool_ptr<TransformComponent> pTransform( m_pool );
	TransformComponent * pOther = (TransformComponent*)pCompo_p;
	
	pTransform->m_local = pOther->m_local;
	pTransform->m_offset = pOther->m_offset;
	pTransform->m_final = pOther->m_final;
	pTransform->m_previousFinal = pOther->m_previousFinal;
	pTransform->m_bSnap = pOther->m_bSnap;


	return pTransform;
}

void game::TransformComponentFactory::UpdateDataFromGfig( TransformComponent & trafoCompo_p, text::GfigElementA * pGFig_p )
{
	GfigElementA * pParam = nullptr;

	if( pGFig_p->GetSubElement( "offset", pParam ) ){

		UpdateTrafoFromGfig( trafoCompo_p.m_offset, pParam );
	}
	if( pGFig_p->GetSubElement( "local", pParam ) ){

		UpdateTrafoFromGfig( trafoCompo_p.m_local, pParam );
	}

	if( !pParam ){

		UpdateTrafoFromGfig( trafoCompo_p.m_local, pGFig_p );
	}

	XMMATRIX mFinal = XMMatrixMultiply( trafoCompo_p.m_offset.DeriveMatrix(), trafoCompo_p.m_local.DeriveMatrix() );

	XMStoreFloat4x4( &trafoCompo_p.m_final, mFinal );
	trafoCompo_p.m_previousFinal = trafoCompo_p.m_final;

	trafoCompo_p.m_bSnap = true;
}

void game::TransformComponentFactory::UpdateTrafoFromGfig( Trafo & trafo_p, text::GfigElementA * pGFig_p )
{
	GfigElementA * pPropertie = nullptr;

	if( pGFig_p->GetSubElement( "pos", pPropertie ) ){

		 UpdateXYZWFromGfig( trafo_p.position, pPropertie);
	}
	if( pGFig_p->GetSubElement( "rotation", pPropertie ) ){

		 UpdateXYZWFromGfig( trafo_p.qRotation, pPropertie);
	}
	if( pGFig_p->GetSubElement( "scale", pPropertie ) ){

		UpdateXYZWFromGfig( trafo_p.scale, pPropertie);
	}
}

void game::TransformComponentFactory::UpdateXYZWFromGfig( DirectX::XMFLOAT4 & XYZW_p, text::GfigElementA * pGFig_p )
{
	GfigElementA * pElement;

	if( pGFig_p->GetSubElement( "x", pElement) ){
		XYZW_p.x = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "y", pElement) ){
		XYZW_p.y = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "z", pElement) ){
		XYZW_p.z = (float)atof(pElement->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "w", pElement) ){
		XYZW_p.w = (float)atof(pElement->m_value.c_str());
	}
}

void game::TransformComponentFactory::VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p )
{
	UpdateDataFromGfig( *((TransformComponent*)pCompo_p), pGFig_p );

	// TODO: not sure...
	/*if( (Object*pObj = pCompo_p->GetObjectOwner()) ){

		pObj->DispatchComponentEventImmediately( COMPONENT_TYPE(TransformComponent), pCompo_p )
	}*/
		

}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void game::TransformComponentFactory::VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p )
{

	/*
	[TransformComponent
		[offset
			[pos		[x = 0]	[y = 0]	[z = 0]	[w = 1] ]
			[scale  	[x = 1]	[y = 1]	[z = 1]	[w = 1] ]
			[rotation	[x = 0]	[y = 0]	[z = 0]	[w = 1] ]
		]
		[local
			[pos		[x = 0] [y = 0] [z = 0] [w = 1] ]
			[scale  	[x = 1]	[y = 1]	[z = 1]	[w = 1] ]
			[rotation	[x = 0]	[y = 0]	[z = 0]	[w = 1] ]
		]
	]
	*/

	TransformComponent & trafo = *((TransformComponent*)pCompo_p);

	pGFig_p->m_subElements.emplace_back( GfigElementA(COMPONENT_NAME(TransformComponent)) );

	GfigElementA & gTrafoCompo = pGFig_p->m_subElements.back();
	{
		GfigElementA gOffset(	"offset" );
		SerializeTrafo( trafo.m_offset, gOffset );
		if( gOffset.m_subElements.size())
			gTrafoCompo.m_subElements.push_back(std::move(gOffset));


		if( gTrafoCompo.m_subElements.size() ){

			GfigElementA gLocal( "local" );
			SerializeTrafo( trafo.m_local, gLocal );
			if( gLocal.m_subElements.size())
				gTrafoCompo.m_subElements.push_back(std::move(gLocal));
		}
		else{

			// if there isnt an offset, no need to have a "local" subelement

			SerializeTrafo( trafo.m_local, gTrafoCompo );
		}
	}	
}

void game::TransformComponentFactory::SerializeTrafo( const Trafo & trafo_p, text::GfigElementA & gFig_p )
{
	GfigElementA gPos("pos");
	SerializeXYZW(trafo_p.position, gPos);
	if( gPos.m_subElements.size())
		gFig_p.m_subElements.push_back(std::move(gPos));

	GfigElementA gRotation("rotation");
	SerializeXYZW(trafo_p.qRotation, gRotation);
	if( gRotation.m_subElements.size() )
		gFig_p.m_subElements.push_back(std::move(gRotation));


	GfigElementA gScale("scale");
	SerializeXYZW(trafo_p.scale, gScale, 1.0f);
	if( gScale.m_subElements.size() )
		gFig_p.m_subElements.push_back(std::move(gScale));
}

void game::TransformComponentFactory::SerializeXYZW( const DirectX::XMFLOAT4 & xyzw_p, text::GfigElementA & gFig_p, const float fDefault_p )
{
	// check if values are different than default values, since default values dont need to be loaded

	if( xyzw_p.x != fDefault_p ){

		gFig_p.m_subElements.push_back(GfigElementA("x", std::to_string((long double)xyzw_p.x ).c_str()) );
	}
	if( xyzw_p.y != fDefault_p ){
		//GfigElementA gY("y", std::to_string(xyzw_p.y ));
		gFig_p.m_subElements.push_back(GfigElementA("y", std::to_string((long double)xyzw_p.y ).c_str()) );
	}
	if( xyzw_p.z != fDefault_p ){
		
		gFig_p.m_subElements.push_back(GfigElementA("z", std::to_string((long double)xyzw_p.z ).c_str()) );
	}
	if( xyzw_p.w != 1.0f ){

		gFig_p.m_subElements.push_back(GfigElementA("w", std::to_string((long double)xyzw_p.w ).c_str()) );
	}
}

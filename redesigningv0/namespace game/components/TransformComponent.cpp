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

void game::TransformComponent::AddChild( TransformComponent * pTrafo_p )
{
	m_node.AddNode( &pTrafo_p->m_node);
}

void game::TransformComponent::RemoveChild( TransformComponent * pTrafo_p )
{
	m_node.RemoveNode( &pTrafo_p->m_node );
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
	//TransformComponent * pTransform = m_pool.Allocate();
	gen::pool_ptr<TransformComponent> pTransform( m_pool );

	GfigElementA * pParam = nullptr;


	if( pGFig_p->GetSubElement( "offset", pParam ) ){

		pTransform->m_offset = GetTrafoFromGfig( pParam );
	}
	if( pGFig_p->GetSubElement( "local", pParam ) ){

		pTransform->m_local = GetTrafoFromGfig( pParam );
	}

	if( !pParam ){

		pTransform->m_local = GetTrafoFromGfig( pGFig_p );
	}	

	//XMFLOAT4X4 moffset = pTransform->m_offset.DeriveMatrix(); XMFLOAT4X4 mlocal = pTransform->m_local.DeriveMatrix();
	XMMATRIX mFinal = XMMatrixMultiply( pTransform->m_offset.DeriveMatrix(), pTransform->m_local.DeriveMatrix() );

	XMStoreFloat4x4( &pTransform->m_final, mFinal );
	pTransform->m_previousFinal = pTransform->m_final;

	//return MAKE_STACK_SHAREDPTR( TransformComponent, pTransform );
	/*pool_Component_ptr ptr;
	ptr = pTransform;
	return ptr;*/

	return pTransform;
}

Trafo TransformComponentFactory::GetTrafoFromGfig( GfigElementA * pGFig_p )
{
	Trafo trafo;

	GfigElementA * pPropertie = nullptr;

	if( pGFig_p->GetSubElement( "pos", pPropertie ) ){

		trafo.position = GetXYZWFromGfig(pPropertie);
	}
	if( pGFig_p->GetSubElement( "rotation", pPropertie ) ){

		trafo.qRotation = GetXYZWFromGfig(pPropertie);
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

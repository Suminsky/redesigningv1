#include "ColorComponent.h"
#include "../../namespace text/TextParser.h"

using namespace game;
using namespace DirectX;
using namespace text;

game::ColorComponent::ColorComponent()
{
	m_type = COMPONENT_TYPE(ColorComponent);

	m_worldColor = m_previousFinalColor = m_offsetColor = m_localColor = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );

	m_node.SetData(this);
}


void game::ColorComponent::UpdateWorldAndFinalColor( const DirectX::XMFLOAT4 & parentWorldColor_p )
{
	m_previousFinalColor = m_finalColor;

	XMVECTOR vWorld = XMVectorMultiply( XMLoadFloat4(&m_localColor), XMLoadFloat4(&parentWorldColor_p) );
	DirectX::XMStoreFloat4( &m_worldColor, vWorld );

	XMVECTOR vOffset = XMLoadFloat4( &m_offsetColor );
	
	XMStoreFloat4( &m_finalColor, XMVectorMultiply( vOffset, vWorld ) );
}
//========================================================================
// 
//========================================================================
game::shared_Component_ptr game::ColorComponentFactory::VCreateComponent( text::GfigElementA * pGFig_p )
{
	ColorComponent * pColor = m_pool.Allocate();

	text::GfigElementA * pParam = nullptr;


	if( pGFig_p->GetSubElement( "offset", pParam ) ){

		pColor->m_offsetColor = GetRGBAFromGfig( pParam );
	}
	if( pGFig_p->GetSubElement( "local", pParam ) ){

		pColor->m_localColor = GetRGBAFromGfig( pParam );
	}
	
	if( !pParam ){

		pColor->m_localColor = GetRGBAFromGfig( pGFig_p );
	}


	XMVECTOR vFinal = XMVectorMultiply( XMLoadFloat4(&pColor->m_offsetColor), XMLoadFloat4(&pColor->m_localColor) );
	XMStoreFloat4( &pColor->m_finalColor, vFinal );
	pColor->m_previousFinalColor = pColor->m_finalColor;

	return MAKE_STACK_SHAREDPTR( ColorComponent, pColor );
}

XMFLOAT4 ColorComponentFactory::GetRGBAFromGfig( GfigElementA * pGFig_p )
{
	XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);

	GfigElementA * pChannel;

	if( pGFig_p->GetSubElement( "r", pChannel) ){
		color.x = (float)atof(pChannel->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "g", pChannel) ){
		color.y = (float)atof(pChannel->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "b", pChannel) ){
		color.z = (float)atof(pChannel->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "a", pChannel) ){
		color.w = (float)atof(pChannel->m_value.c_str());
	}

	return color;
}

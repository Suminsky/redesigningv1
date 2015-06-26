#include "ColorComponent.h"
#include "../../namespace text/TextParser.h"

using namespace game;
using namespace DirectX;
using namespace text;
using namespace std;

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

void game::ColorComponent::UpdateWorldAndFinalColor()
{
	m_previousFinalColor = m_finalColor;

	XMVECTOR vWorld = XMLoadFloat4(&m_localColor);
	DirectX::XMStoreFloat4( &m_worldColor, vWorld );

	XMVECTOR vOffset = XMLoadFloat4( &m_offsetColor );

	XMStoreFloat4( &m_finalColor, XMVectorMultiply( vOffset, vWorld ) );
}

//========================================================================
// 
//========================================================================
game::pool_Component_ptr game::ColorComponentFactory::VCreateComponent( text::GfigElementA * pGFig_p )
{
	pool_ColorCompo_ptr pColor(m_pool);

	VUpdateComponent(pColor.Get(), pGFig_p);

	return pColor;
}

game::pool_Component_ptr game::ColorComponentFactory::VCloneComponent( const Component* pCompo_p )
{
	pool_ColorCompo_ptr pColor(m_pool);
	ColorComponent * pOther = (ColorComponent*)pCompo_p;

	pColor->m_localColor = pOther->m_localColor;
	pColor->m_offsetColor = pOther->m_offsetColor;
	pColor->m_finalColor = pOther->m_finalColor;
	pColor->m_previousFinalColor = pOther->m_previousFinalColor;
	pColor->m_bSnap = pOther->m_bSnap;

	return pColor;
}

void game::ColorComponentFactory::VUpdateComponent( Component * pCompo_p, text::GfigElementA * pGFig_p )
{
	ColorComponent * pColor = (ColorComponent*)pCompo_p;

	text::GfigElementA * pParam = nullptr;

	if( pGFig_p->GetSubElement( "offset", pParam ) ){

		 UpdateRGBAFromGfig( pColor->m_offsetColor, pParam );
	}
	if( pGFig_p->GetSubElement( "local", pParam ) ){

		 UpdateRGBAFromGfig( pColor->m_localColor, pParam );
	}

	if( !pParam ){

		 UpdateRGBAFromGfig( pColor->m_localColor, pGFig_p );
	}


	XMVECTOR vFinal = XMVectorMultiply( XMLoadFloat4(&pColor->m_offsetColor), XMLoadFloat4(&pColor->m_localColor) );
	XMStoreFloat4( &pColor->m_finalColor, vFinal );
	pColor->m_previousFinalColor = pColor->m_finalColor;
}

void game::ColorComponentFactory::UpdateRGBAFromGfig( DirectX::XMFLOAT4 & color_p, text::GfigElementA * pGFig_p )
{
	GfigElementA * pChannel;

	if( pGFig_p->GetSubElement( "r", pChannel) ){
		color_p.x = (float)atof(pChannel->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "g", pChannel) ){
		color_p.y = (float)atof(pChannel->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "b", pChannel) ){
		color_p.z = (float)atof(pChannel->m_value.c_str());
	}
	if( pGFig_p->GetSubElement( "a", pChannel) ){
		color_p.w = (float)atof(pChannel->m_value.c_str());
	}
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
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------

void game::ColorComponentFactory::VSerialize( const Component * pCompo_p, text::GfigElementA * pGFig_p )
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

	ColorComponent & color = *((ColorComponent*)pCompo_p);

	pGFig_p->m_subElements.emplace_back( GfigElementA(COMPONENT_NAME(ColorComponent)) );

	GfigElementA & gColorCompo = pGFig_p->m_subElements.back();
	{
		GfigElementA gLocal(	"local" );
		SerializeRGBA( color.m_localColor, gLocal );
		if( gLocal.m_subElements.size())
			gColorCompo.m_subElements.push_back(std::move(gLocal));

		GfigElementA gOffset(	"offset" );
		SerializeRGBA( color.m_offsetColor, gOffset );
		if( gOffset.m_subElements.size())
			gColorCompo.m_subElements.push_back(std::move(gOffset));
	}	
}

void game::ColorComponentFactory::SerializeRGBA( const DirectX::XMFLOAT4 & rgba_p, text::GfigElementA & gFig_p )
{
	// check if values are different than default values, since default values dont need to be loaded

	if( rgba_p.x != 1.0f ){

		gFig_p.m_subElements.push_back(GfigElementA("r", std::to_string((long double)rgba_p.x ).c_str()) );
	}
	if( rgba_p.y != 1.0f ){
	
		gFig_p.m_subElements.push_back(GfigElementA("g", std::to_string((long double)rgba_p.y ).c_str()) );
	}
	if( rgba_p.z != 1.0f ){

		gFig_p.m_subElements.push_back(GfigElementA("b", std::to_string((long double)rgba_p.z ).c_str()) );
	}
	if( rgba_p.w != 1.0f ){

		gFig_p.m_subElements.push_back(GfigElementA("a", std::to_string((long double)rgba_p.w ).c_str()) );
	}
}

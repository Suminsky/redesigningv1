#include "ButtonComponent.h"

using namespace game;
using namespace DirectX;

game::SpriteButtonCompo::SpriteButtonCompo(	int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p,
	XMFLOAT4 uvHover_p, float wBorder_p /*= 0.0f*/, float hBorder_p /*= 0.0f */ )
	: m_pSpriteCompoRef(pSpriteCompo_p)
{
	assert(pSpriteCompo_p);
	m_type = COMPONENT_TYPE(SpriteButtonCompo);

	m_ID = iID_p;

	m_uv_Normal = pSpriteCompo_p->m_renderData.m_uvRect;
	m_uv_Hover = uvHover_p;
	m_uv_Pressed = uvHover_p;

	m_borderGap.x = wBorder_p;
	m_borderGap.y = hBorder_p;

	m_eState = E_STATE_NORMAL;
}

game::SpriteButtonCompo::SpriteButtonCompo(	int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p,
	XMFLOAT4 uvHover_p, XMFLOAT4 uvPressed_p, float wBorder_p /*= 0.0f*/, float hBorder_p /*= 0.0f */ )
	: m_pSpriteCompoRef(pSpriteCompo_p)
{
	assert(pSpriteCompo_p);
	m_type = COMPONENT_TYPE(SpriteButtonCompo);

	m_ID = iID_p;

	m_uv_Normal = pSpriteCompo_p->m_renderData.m_uvRect;
	m_uv_Hover = uvHover_p;
	m_uv_Pressed = uvPressed_p;

	m_borderGap.x = wBorder_p;
	m_borderGap.y = hBorder_p;

	m_eState = E_STATE_NORMAL;
}

void game::SpriteButtonCompo::Init(	int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p,
	DirectX::XMFLOAT4 uvHover_p, float wBorder_p, float hBorder_p ){

		assert(pSpriteCompo_p);
		m_pSpriteCompoRef = pSpriteCompo_p;

		m_ID = iID_p;

		m_uv_Normal = pSpriteCompo_p->m_renderData.m_uvRect;
		m_uv_Hover = uvHover_p;
		m_uv_Pressed = uvHover_p;

		m_borderGap.x = wBorder_p;
		m_borderGap.y = hBorder_p;

		m_eState = E_STATE_NORMAL;
}

void game::SpriteButtonCompo::Init(	int iID_p, const pool_SpriteCompo__ptr & pSpriteCompo_p,
	DirectX::XMFLOAT4 uvHover_p, DirectX::XMFLOAT4 uvPressed_p, float wBorder_p, float hBorder_p ){

		assert(pSpriteCompo_p);
		m_pSpriteCompoRef = pSpriteCompo_p;

		m_ID = iID_p;

		m_uv_Normal = pSpriteCompo_p->m_renderData.m_uvRect;
		m_uv_Hover = uvHover_p;
		m_uv_Pressed = uvPressed_p;

		m_borderGap.x = wBorder_p;
		m_borderGap.y = hBorder_p;

		m_eState = E_STATE_NORMAL;
}

bool game::SpriteButtonCompo::GetPointCollision( XMFLOAT2 mouseXY_p )
{
	float halfW = m_pSpriteCompoRef->m_renderData.m_res.x / 2.0f - m_borderGap.x;

	float x = XMVectorGetX(m_pSpriteCompoRef->m_renderData.m_mWorld.r[3]);
	float xOff = m_pSpriteCompoRef->m_renderData.m_padding.x;
	x += xOff;

	if( mouseXY_p.x >=  x - halfW
		&& mouseXY_p.x <= x + halfW ){

			float halfH =  m_pSpriteCompoRef->m_renderData.m_res.y / 2.0f - m_borderGap.y;
			float y = XMVectorGetY(m_pSpriteCompoRef->m_renderData.m_mWorld.r[3]);
			float yOff = m_pSpriteCompoRef->m_renderData.m_padding.y;
			y += yOff;

			if( mouseXY_p.y >= y - halfH
				&& mouseXY_p.y <= y + halfH ){

					return true;
			}
	}

	return false;
}
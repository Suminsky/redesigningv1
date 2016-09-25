#include "PCHF.h"
#include "AABB2D.h"

using namespace phys;

const DirectX::XMFLOAT2 AABB2D::s_left( -1.0f, 0.0f );
const DirectX::XMFLOAT2 AABB2D::s_right( 1.0f,  0.0f );
const DirectX::XMFLOAT2 AABB2D::s_down(  0.0f, -1.0f );
const DirectX::XMFLOAT2 AABB2D::s_up(  0.0f,  1.0f );
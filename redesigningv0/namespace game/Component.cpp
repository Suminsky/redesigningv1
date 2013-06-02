#include "Component.h"
#include "Object.h"

using namespace game;

OBJECT_LAYERINDEX Component::GetOwner()
{
	return m_pOwner->ID();
}

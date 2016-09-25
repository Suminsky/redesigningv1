#include "PCHF.h"
#include "Component.h"

#include "Object.h"

using namespace game;

void game::Component::VOnAttach()
{

}

void game::Component::VOnDetach()
{

}

void game::Component::Dettach()
{
	assert( m_pObjectOwner);
	m_pObjectOwner->DettachComponent(this);
}
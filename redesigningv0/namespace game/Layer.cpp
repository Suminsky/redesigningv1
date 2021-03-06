#include "PCHF.h"
#include "Layer.h"

using namespace game;

//========================================================================
// layer
//========================================================================

void game::Layer::Update( const double dDeltaTime_p )
{
	m_timer.Update( dDeltaTime_p );

	VOnUpdate( m_timer.GetTime(), m_timer.GetDelta() );

	// update systems

	for( int itSys = 0, iSize = (int)m_systems.m_systems.size();
		 itSys < iSize;
		 ++itSys ){

		m_systems.m_systems[itSys]->VOnUpdate( m_timer.GetTime(), m_timer.GetDelta() );
	}

	m_systems.DispatchSystemEvents();
	m_objects.DispatchObjectInternalEvents();

	VLateUpdate( m_timer.GetTime(), m_timer.GetDelta() );

	// cleaning should be done after draw.. TODO

	m_objects.CleanRemovedObjectComponents();

	if( ! m_objects.m_removedObjects.empty() )
		m_objects.CleanRemovedObjects();

	if( ! m_systems.m_removedSystems.empty() )
		m_systems.CleanRemovedSystems();
}

//========================================================================
// objects
//========================================================================

void game::Layer::AddObject( pool_Object_ptr && pObject_p )
{
	m_objects.AddObject( std::move(pObject_p) );
}

void game::Layer::AddObject( const pool_Object_ptr & pObject_p )
{
	m_objects.AddObject(pObject_p);
}

void game::Layer::RemoveObject( OBJECTINDEX objectCurrentIndex_p )
{
	m_objects.RemoveObject(objectCurrentIndex_p);
}

void game::Layer::RemoveObject( const pool_Object_ptr & pObject_p )
{
	RemoveObject(pObject_p->m_currentIndexOnLayer);
}

void game::Layer::RemoveObject( const Object * pObject_p )
{
	RemoveObject(pObject_p->m_currentIndexOnLayer);
}

void game::Layer::Draw( const double dInterpolation_p, const double dDelta_p )
{
	for( int itSys = 0, iSize = (int)m_systems.m_systems.size();
		itSys < iSize;
		++itSys ){

			m_systems.m_systems[itSys]->VOnDraw( dInterpolation_p );
	}

	VOnDraw( dInterpolation_p, dDelta_p );
}

void game::Layer::SerializeAttachedObjs( text::GfigElementA *pGfig_p )
{
	for( int it = 0, nObjs = (int)m_objects.m_objects.size();
		it < nObjs;
		++it ){

		m_objFactory.Serialize( m_objects.m_objects[it].Get(), pGfig_p );
	}
}

//========================================================================
// 
//========================================================================
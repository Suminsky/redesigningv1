#include "State.h"


using namespace game;



game::State::~State()
{

}

void game::State::VOnAttach()
{

}

void game::State::VOnUpdate( double, double )
{

}

void game::State::VLateUpdate( double, double )
{

}

void game::State::VOnDraw()
{

}

void game::State::VOnRemove()
{

}

void game::State::VOnResize()
{

}

//========================================================================
// layer stuff
//========================================================================

void game::State::AddLayer( shared_Layer_ptr && pNewLayer_p )
{
	assert( pNewLayer_p->m_bDettached );

	pNewLayer_p->m_bDettached = false;

	if( pNewLayer_p->m_currentLayerIndex == INVALID_LAYERINDEX
		||
		pNewLayer_p->m_pStateOwner != this ){

			pNewLayer_p->m_currentLayerIndex = (OBJECTINDEX)m_layers.size();
			pNewLayer_p->m_pStateOwner = this;

			pNewLayer_p->VOnAttach();

			m_layers.push_back( std::move(pNewLayer_p) );
	}
	else{

		// if I call OnDestroy only on clean, why call onInit here?
		// if layer not yet cleaned then ondestroy was never called, so
		// on init is called twice.. 

		pNewLayer_p->VOnAttach();
	}
}

void game::State::AddLayer( const shared_Layer_ptr & pNewLayer_p )
{
	assert( pNewLayer_p->m_bDettached );

	pNewLayer_p->m_bDettached = false;

	if( pNewLayer_p->m_currentLayerIndex == INVALID_LAYERINDEX
		||
		pNewLayer_p->m_pStateOwner != this ){

			pNewLayer_p->m_currentLayerIndex = (OBJECTINDEX)m_layers.size();
			pNewLayer_p->m_pStateOwner = this;

			m_layers.push_back( pNewLayer_p );
	}

	pNewLayer_p->VOnAttach();
}

void game::State::RemoveLayer( LAYERINDEX layerCurrentIndex_p )
{
	RemoveLayer( m_layers[layerCurrentIndex_p].get() );
}

void game::State::RemoveLayer( const shared_Layer_ptr & pNewLayer_p )
{
	RemoveLayer( pNewLayer_p.get() );
}

void game::State::RemoveLayer( Layer * pNewLayer_p )
{
	assert( pNewLayer_p->m_currentLayerIndex != INVALID_LAYERINDEX );
	assert( !pNewLayer_p->m_bDettached );

	pNewLayer_p->m_bDettached = true;

	m_removedLayers.push_back( pNewLayer_p );
}

//========================================================================
// 
//========================================================================

void game::State::Update( const double dDeltaTime_p )
{
	m_timer.Update( dDeltaTime_p );

	VOnUpdate(m_timer.GetTime(), m_timer.GetDelta() );

	// traverse layers and update them, if active

	for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

		if( m_layers[it]->m_bActive ){

			m_layers[it]->Update( m_timer.GetDelta() );
		}
	}

	VLateUpdate( m_timer.GetTime(), m_timer.GetDelta() );

	// clean dead layers

	// cleaning should be done after draw.. TODO

	if( !m_removedLayers.empty() )
		CleanRemovedLayers();

	//
}

void game::State::OnResize( int W_p, int H_p )
{
	VOnResize();

	for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

		m_layers[it]->VOnResize( W_p, H_p );
	}
}

void game::State::Remove()
{
	for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

		m_layers[it]->VOnRemove();
	}

	VOnRemove();
}

void game::State::Draw( const double dInterpolation_p, const double dDelta_p )
{
	for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

		if( m_layers[it]->m_bVisible ){

			m_layers[it]->Draw( dInterpolation_p, dDelta_p );
		}
	}

	VOnDraw();
}

void game::State::CleanRemovedLayers()
{
	unsigned int nRemoved =	(unsigned int)m_removedLayers.size(); // cache
	unsigned int nObjects =		(unsigned int)m_layers.size();

	int nSkipped = 0;
	for( unsigned int itR = 0, itLast = (nObjects-1); itR < nRemoved; ++itR ){

		if( !m_removedLayers[itR]->m_bDettached ){

			++nSkipped;
			continue;
		}

		if( m_removedLayers[itR]->m_currentLayerIndex == itLast ){

			m_removedLayers[itR]->VOnRemove();
			m_removedLayers[itR]->m_currentLayerIndex = INVALID_LAYERINDEX;
			--itLast;
			continue; // already "swapped"
		}

		std::swap( m_layers[m_removedLayers[itR]->m_currentLayerIndex], m_layers[itLast] );
		--itLast;

		// update the index of the swapped object (not the one sent to pop)

		m_layers[m_removedLayers[itR]->m_currentLayerIndex]->m_currentLayerIndex = m_removedLayers[itR]->m_currentLayerIndex;

		// since the removed vector store pointers, theres no dirt data to update
		// but we need to invalidate the discarded, cause its used as check when adding and removing..that
		// can be discarded TODO

		m_removedLayers[itR]->VOnRemove();
		m_removedLayers[itR]->m_currentLayerIndex = INVALID_LAYERINDEX;

	}

	// "trim"

	nRemoved -= nSkipped;
	m_layers.resize(nObjects - nRemoved);

	m_removedLayers.clear();
}

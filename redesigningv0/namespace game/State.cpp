#include "State.h"


using namespace game;


void game::State::AddLayer( shared_Layer_ptr && pNewLayer_p )
{
	assert( pNewLayer_p->m_bDead );

	pNewLayer_p->m_bDead = false;

	if( pNewLayer_p->m_currentLayerIndex == INVALID_LAYERINDEX
		||
		pNewLayer_p->m_pStateOwner != this ){

			pNewLayer_p->m_currentLayerIndex = (OBJECTINDEX)m_layers.size();
			pNewLayer_p->m_pStateOwner = this;

			pNewLayer_p->VOnInit();

			m_layers.push_back( std::move(pNewLayer_p) );
	}
	else{

		pNewLayer_p->VOnInit();
	}
}

void game::State::AddLayer( const shared_Layer_ptr & pNewLayer_p )
{
	assert( pNewLayer_p->m_bDead );

	pNewLayer_p->m_bDead = false;

	if( pNewLayer_p->m_currentLayerIndex == INVALID_LAYERINDEX
		||
		pNewLayer_p->m_pStateOwner != this ){

			pNewLayer_p->m_currentLayerIndex = (OBJECTINDEX)m_layers.size();
			pNewLayer_p->m_pStateOwner = this;

			m_layers.push_back( pNewLayer_p );
	}

	pNewLayer_p->VOnInit();
}

void game::State::RemoveLayer( LAYERINDEX layerCurrentIndex_p )
{
	assert( layerCurrentIndex_p != INVALID_LAYERINDEX );
	assert( !m_layers[layerCurrentIndex_p]->m_bDead );

	m_layers[layerCurrentIndex_p]->m_bDead = true;

	m_removedLayers.push_back( layerCurrentIndex_p );
}

void game::State::RemoveLayer( const shared_Layer_ptr & pNewLayer_p )
{
	RemoveLayer( pNewLayer_p->m_currentLayerIndex );
}

void game::State::RemoveLayer( const Layer * pNewLayer_p )
{
	RemoveLayer( pNewLayer_p->m_currentLayerIndex );
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

void game::State::Destroy()
{
	for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

		m_layers[it]->VOnDestroy();
	}

	VOnDestroy();
}

void game::State::Draw( const double dInterpolation_p )
{
	for( int it = 0, itEnd = (int)m_layers.size(); it != itEnd; ++ it ){

		if( m_layers[it]->m_bVisible ){

			m_layers[it]->Draw( dInterpolation_p );
		}
	}

	VOnDraw();
}

void game::State::CleanRemovedLayers()
{
	unsigned int nRemoved =	(unsigned int)m_removedLayers.size(); // cache
	unsigned int nObjects =		(unsigned int)m_layers.size();

	for( unsigned int itRemoved = 0, itLast = nObjects - 1;
		itRemoved < nRemoved;
		++itRemoved, --itLast ){

			if( !m_layers[m_removedLayers[itRemoved]]->m_bDead ){

				--nRemoved;
				continue; // untested
			}


			m_layers[m_removedLayers[itRemoved]]->VOnDestroy();
			m_layers[m_removedLayers[itRemoved]]->m_currentLayerIndex = INVALID_LAYERINDEX;
			m_layers[m_removedLayers[itRemoved]]->m_pStateOwner = nullptr;

			if( m_removedLayers[itRemoved] == itLast ){

				continue;
			}

			std::swap( m_layers[m_removedLayers[itRemoved]], m_layers[itLast] );

			if( m_layers[m_removedLayers[itRemoved]]->m_bDead ){

				// find the swaped task on the list to be destroyed, update the index

				for( unsigned int itToBeDestroyed = itRemoved; itToBeDestroyed < nRemoved; ++itToBeDestroyed ){

					if( m_removedLayers[itToBeDestroyed] == m_layers[m_removedLayers[itRemoved]]->m_currentLayerIndex ){

						m_removedLayers[itToBeDestroyed] = m_removedLayers[itRemoved];
					}
				}
			}
			else{

				m_layers[m_removedLayers[itRemoved]]->m_currentLayerIndex = m_removedLayers[itRemoved];
			}
	}

	// "trim"

	m_layers.resize(nObjects - nRemoved);

	m_removedLayers.clear();
}

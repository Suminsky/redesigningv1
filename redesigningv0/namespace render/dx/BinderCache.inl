
template< class dxTBinder, class DeviceCache >
void dx::BinderCache<dxTBinder, DeviceCache>::Init( int size_p, DeviceCache * pDeviceCache_p )
{
	m_pDeviceCache = pDeviceCache_p;
	m_size = size_p;
	m_cache.reserve( size_p );
	m_IDs.reserve( size_p );
}

template< class dxTBinder, class DeviceCache >
int dx::BinderCache<dxTBinder, DeviceCache>::Add( int iID_p )
{
	assert( m_cache.size() < m_size );

	dxTBinder binder( m_pDeviceCache->Acquire(iID_p) );

	m_cache.push_back( binder );
	m_IDs.push_back( iID_p );

	return (int)(m_cache.size()-1);
}

template< class dxTBinder, class DeviceCache >
dxTBinder & dx::BinderCache<dxTBinder, DeviceCache>::GetFromID( int iID_p, int * pIndexID_p /*= nullptr */ )
{
	for( int it = 0, size = (int)m_cache.size();	it < size;		++it)
	{
		if( m_IDs[it] == iID_p )
		{
			return m_cache[it];
		}
	}

	// cache miss:

	int index = Add( iID_p );

	if( pIndexID_p )
		*pIndexID_p = index;

	return m_cache[index];
}

template< class dxTBinder, class DeviceCache >
dxTBinder & dx::BinderCache<dxTBinder, DeviceCache>::Get( UINT itTBinder_p )
{
	assert( itTBinder_p < m_cache.size());

	return m_cache[itTBinder_p];
}

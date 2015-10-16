#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/02/09
	created:	9:2:2014   20:34
	file:		BinderCache.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	#as the only thing require for a shader binder is the shader d3d interface, a cache
				can be created using the d3d interface ID as key
				#fixed size
				#this can work for any d3d resource, but sometimes its better to have a cache with more
				intuitive keys (like texture name, etc.)

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "Device.h"
#include "pipeline/Binder.h"

namespace dx{

	template< class dxTBinder, class DeviceCache >
	class BinderCache{

	public:

		void Init( int size_p, DeviceCache * pDeviceCache_p );

		//------------------------------------------------------------------------
		// add new shader to the cache
		//------------------------------------------------------------------------
		int Add( int iID_p );

		//------------------------------------------------------------------------
		// Get the shader by ID, if ID is not found, it adds a new shader
		// to the cache
		//------------------------------------------------------------------------
		dxTBinder & GetFromID( int iID_p, int * pIndexID_p = nullptr );

		//------------------------------------------------------------------------
		// get texture by index
		//------------------------------------------------------------------------
		dxTBinder & Get( UINT itTBinder_p );

	private:

		unsigned int m_size;
		DeviceCache * m_pDeviceCache;
		std::vector<dxTBinder>		m_cache;
		std::vector<int>			m_IDs;
	};


}
#include "BinderCache.inl"
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/12
	created:	12:2:2013   15:15
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\ResourceCache.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render
	file base:	ResourceCache
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	DX Resource central point of creation and sharing..
				- it will release all resources on destruction
				- it will look for a resource with the given description
				- it can return by ID
				TODO: ..if the game changes of level, things like textures may not be used anymore ( some of them ),
				so it would be good to have a cleaning


	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#pragma once

// libraries needed

#pragma comment( lib, "D3D11.lib")
#pragma comment( lib, "D3DX11.lib")

// system/standard headers

#include <assert.h>
#include <D3D11.h>

#include "../../namespace gen/gen_macros.h"

#include "type wrappers/BlendState.h"
#include "type wrappers/BufferResource.h"
#include "type wrappers/DepthStencilState.h"
#include "type wrappers/InputLayout.h"
#include "type wrappers/PixelShader.h"
#include "type wrappers/VertexShader.h"
#include "type wrappers/RasterizerState.h"
#include "type wrappers/RenderTargetView.h"
#include "type wrappers/SamplerState.h"
#include "type wrappers/ShaderResourceView.h"
#include "type wrappers/TextureResource.h"

namespace dx{

	//========================================================================
	// A fixed size resource cache class.
	// ***
	// T must implement a CreationParams structure and a Create method that uses the CreationParams
	// as param and returns the DX pointer. Create receives device context and params as parameter.
	// T must implement also a Description structure with a comparison operator for cache read tests.
	// CreationParams must contain Description desc object.
	// T must typedef the type its wrapping and name it res and pRes.
	// ***
	//========================================================================
	template< class T, int fixedSize >
	class ResourceCache{

	private:

		ID3D11Device *& m_pDeviceRef;	// the one that actually can create stuff

		UINT m_iCurrentIndex;			// next slot to be allocated, also the ID of the resources
		//UINT m_nFreedSlots;				// how many freed slot there are?

		typename T::pRes m_cache[fixedSize];			// the container per se

		struct SlotInfo{

			DBG(UINT nUseCount;)
			typename T::Description desc;

		} m_slotsInfo[fixedSize];//use a hashtable<desc,index on m_cache>;


		ResourceCache();	//denied default ctor
		ResourceCache & operator = (const ResourceCache & other_p); //denied assign operator

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ResourceCache( ID3D11Device *& pDevice_p ): m_pDeviceRef(pDevice_p), m_iCurrentIndex(0){

			memset(m_cache, NULL, fixedSize*sizeof(T*));
			//memset( m_slotsInfo, 0, fixedSize*sizeof(SlotInfo));	// (leaks due std stuff on the descs)
		}

		//------------------------------------------------------------------------
		// Releases all non null pointers in the cache.
		//------------------------------------------------------------------------
		virtual ~ResourceCache()
		{
			for( UINT it = 0; it < fixedSize; it++ )
			{
				if( m_cache[it] != NULL )
					m_cache[it]->Release();
			}
		}

		//------------------------------------------------------------------------
		// Acquire by desc, if not found (cache miss), creates one
		//------------------------------------------------------------------------
		UINT Acquire( const typename T::CreationParams & params_p,  typename T::pRes & pResource_p ){

			for( UINT it = 0; it < fixedSize; it++ )
			{
				if( m_slotsInfo[it].desc == params_p.desc)
				{
					pResource_p = m_cache[it];
					DBG(++m_slotsInfo[it].nUseCount;);
					return it;
				}
			}

			// cache miss:
			return CreateResource( params_p, pResource_p );
		}

		//------------------------------------------------------------------------
		// Main function to return stored resources.
		//------------------------------------------------------------------------
		typename T::pRes Acquire( const UINT id_p )
		{
			assert(id_p < fixedSize);
			assert(id_p <= m_iCurrentIndex );
			DBG(assert(m_slotsInfo[id_p].nUseCount != 0));

			DBG(++m_slotsInfo[id_p].nUseCount);

			return m_cache[id_p];
		}

		//------------------------------------------------------------------------
		// Acquire by desc, if not found (cache miss), creates one
		//------------------------------------------------------------------------
		UINT Acquire( const typename T::CreationParams & params_p ){

			for( UINT it = 0; it < fixedSize; it++ )
			{
				if( m_slotsInfo[it].desc == params_p.desc)
				{
					DBG(++m_slotsInfo[it].nUseCount);
					return it;
				}
			}

			// cache miss:
			return CreateResource( params_p );
		}

		UINT Clone(const typename T::CreationParams & params_p ){

			// cache miss:

		}

		//

		UINT GetAmount()const {
			return m_iCurrentIndex;
		}

		const SlotInfo & GetSlotInfo(unsigned int index)const {

			assert(index < (m_iCurrentIndex));
			return m_slotsInfo[index];
		}

	private:

		//------------------------------------------------------------------------
		// Receives a CreationParams for the current template type.
		// Returns ID and resource data
		//------------------------------------------------------------------------
		UINT  CreateResource( const typename T::CreationParams & params_p, typename T::pRes & pResource_p  )
		{

			if( m_iCurrentIndex == fixedSize)
			{
				throw std::exception("render:dx resource cache limit achieved");
			}

			pResource_p = m_cache[m_iCurrentIndex] = T::Create(m_pDeviceRef, params_p);


			// update slot info:
			DBG(m_slotsInfo[m_iCurrentIndex].nUseCount = 1;);
			m_slotsInfo[m_iCurrentIndex].desc = params_p.desc;

			// points to next element:
			++m_iCurrentIndex;

			// return new element index/ID:
			return (m_iCurrentIndex-1);
		}

		//------------------------------------------------------------------------
		// Receives a CreationParams for the current template type.
		// Returns ID only.
		//------------------------------------------------------------------------
		UINT  CreateResource( const typename T::CreationParams & params_p )
		{

			if( m_iCurrentIndex == fixedSize)
			{
				throw std::exception("render:dx resource cache limit achieved");
			}

			m_cache[m_iCurrentIndex] = T::Create(m_pDeviceRef, params_p);

			// update slot info:
			DBG(m_slotsInfo[m_iCurrentIndex].nUseCount = 1);
			m_slotsInfo[m_iCurrentIndex].desc = params_p.desc;

			// points to next element:
			++m_iCurrentIndex;

			// return new element index/ID:
			return (m_iCurrentIndex-1);
		}
	};

}
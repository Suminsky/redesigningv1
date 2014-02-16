#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/02/15
	created:	15:2:2014   15:29
	file:		InstancedSprites.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <vector>

// private includes
#include "../namespace render/dx/Device.h"


namespace game{
	class SpriteComponent_;
}

namespace sprite{

	class SpriteRenderer;

	struct spriteInstance{

		float res[2];
		float padding[2];
		float uvRect[4];
		float mWorld[16];
		float color[4];
	};

	//========================================================================
	// works as a circular buffer
	// buffers data so it can map all at once
	// makes no guarantees that data being overlapped on warping are not in use
	// by driver/gpu, so make sure the size is big enough vs usage temporary enough
	//========================================================================
	class InstancesVertexBuffer{

	public:

		//------------------------------------------------------------------------
		// ctors
		//------------------------------------------------------------------------
		InstancesVertexBuffer(	UINT vbMaxInstances_p, UINT vbSlot_p, ID3D11Buffer * pVB_p, UINT vbOffset_p = 0 );
		InstancesVertexBuffer(){}
		void Initialize(		UINT vbMaxInstances_p, UINT vbSlot_p, ID3D11Buffer * pVB_p, UINT vbOffset_p = 0 );

		//------------------------------------------------------------------------
		// resets index on the data buffers
		//------------------------------------------------------------------------
		void StartBuffering();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void BufferNewInstance( const spriteInstance & spriteData_p, UINT64 sortKey_p );
		void BufferNewInstance( const spriteInstance & spriteData_p );

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void MapInstancesToVB( dx::Device * pDevice_p );
		void MapInstancesToVB_NoSort( dx::Device * pDevice_p );

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		UINT GetCurrentVBOffset() const { return m_firstAvailable; }
		UINT GetVBSize() const { return m_size; }
		dx::BindIAVertexBuffer * GetBinder() { return & m_bindVB; }

	private:

		struct Entry{

			UINT64 drawableKey; // sort key
			int index;			// index on the drawables array

			bool operator ()( const Entry & a, const Entry & b)const{ // used by stl sort algos

				return a.drawableKey < b.drawableKey;
			}
		};

		dx::BindIAVertexBuffer m_bindVB;

		UINT m_size;
		UINT m_firstAvailable;

		UINT m_iBufferingAt;
		bool m_bLocked; // assures only one object buffers data per time, TODO: dbg only

		std::vector<spriteInstance> m_vSpriteInstances;
		std::vector<Entry>			m_vSortable;
		std::vector<spriteInstance> m_vSpriteInstancesTmp;	
	};

	//========================================================================
	// 
	//========================================================================
	class InstancedSprites{

		friend class SpriteRenderer;

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		InstancedSprites();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Initialize(	dx::BindPSShaderResourceView	& pTextureBinder_p,
							dx::BindOMBlendState			& pBlendBinder,
							dx::BindPSSampler				& pSampleState,
							InstancesVertexBuffer			& pIVB_p );

		//------------------------------------------------------------------------
		// reset draw calls offset and length
		//------------------------------------------------------------------------
		void StartBufferingInstances();

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void AddInstance( const spriteInstance & spriteData_p, UINT64 sortKey_p );
		void AddInstance( const spriteInstance & spriteData_p );

		void AddInstance( game::SpriteComponent_ *pSprite_p );
		void AddInstance_NoSort( game::SpriteComponent_ *pSprite_p );

		//------------------------------------------------------------------------
		// instance data is mapped to vb, and draw calls are update
		//------------------------------------------------------------------------
		void ConcludeInstancesBuffering( dx::Device * pDevice_p, UINT64 sortKey_p );
		void ConcludeInstancesBuffering_NoSort( dx::Device * pDevice_p, UINT64 sortKey_p );

	private:

		UINT m_nWarpedAt;

		dx::PipeState			 m_pipeState;	// holds state commom to all instance, like texture, blend and sampler
		dx::DrawIndexedInstanced m_drawCall;	// holds offsets into the instances VB
		dx::DrawIndexedInstanced m_drawCall_warpException;

		render::Drawable		 m_drawable; // pointers to above structures, plus pipestates from spriteRenderer (shader, IA, camera..)
		render::Drawable		 m_drawable_warpException;

		InstancesVertexBuffer	* m_pIVBref;
	};
}
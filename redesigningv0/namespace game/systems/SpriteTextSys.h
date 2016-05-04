#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2015/07/11
	created:	11:7:2015   13:36
	file:		SpriteTextSys.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../System.h"
//#include "SpriteTextCompo.h"
#include "../../namespace sprite/InstancedSprites.h"

namespace game{

	class SpriteTextCompo;
	class SpriteTextCompoFactory;
}


namespace sprite{

	class SpriteRenderer;
	class Camera;
}
namespace dx{

	class Device;
}

namespace game{

	class SpriteTextSys: public game::System{

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		SpriteTextSys( sprite::SpriteRenderer * pSpriteRenderer_p, sprite::Camera * pCamera_p, dx::Device * pDevice_p )
			:
		m_poolAccess(nullptr, nullptr),
		m_pSpriteRendererRef(pSpriteRenderer_p),
		m_pCameraRef(pCamera_p),
		m_pDevice(pDevice_p)
		{}

	private:

		//------------------------------------------------------------------------
		// overrides
		//------------------------------------------------------------------------
		void VOnInit();
		void VOnDraw( double interp_p );

		//------------------------------------------------------------------------
		// aux
		//------------------------------------------------------------------------
		void SortTexts();
		void MapAllInstMemBlockToVB();
		D3D11_MAPPED_SUBRESOURCE StartMappingToVB();
		void ConcludeMapping();


		void PrepareSortedTexts_OptimizedForStaticLinearlyAllocated();
		void PrepareSortedTexts_MappedPerText();

		gen::Pool<game::SpriteTextCompo>::PoolAccessor m_poolAccess;
		sprite::Camera * m_pCameraRef;
		sprite::SpriteRenderer * m_pSpriteRendererRef;
		game::SpriteTextCompoFactory * m_pTextCompoFactoryRef; // get at OnInit
		dx::Device * m_pDevice;

		render::Drawable m_drawableAux;
		dx::PipeState m_pipeState_VB;

		struct Entry{

			UINT64 drawableKey; // sort key
			int index;	
		};
		std::vector<Entry> m_sortArray;
	};

	typedef std::shared_ptr<SpriteTextSys> shared_SpriteTextSys_ptr;
	typedef std::weak_ptr<SpriteTextSys> weak_SpriteTextSys_ptr;

}
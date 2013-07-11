#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/12
	created:	12:2:2013   17:12
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\type wrappers\InputLayout.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\type wrappers
	file base:	InputLayout
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	generic tests
				TODO: everything, this is a mess of tests that I should redo when I get stuff straight

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// libraries needed

// system/standard headers

//#include <boost/smart_ptr.hpp>

// private headers
#include "../namespace gen/gen_data.h"
#include "../namespace win/win_macros.h"

#include "../namespace render/dx/Device.h"
#include "../namespace render/DrawablesQueue.h"
#include "binders cache/BlendStateBinders.h"
#include "binders cache/binderscache.h"
#include "Camera.h"
#include "CameraCbuffer.h"
#include "Sprite.h"

namespace game{
	class SpriteComponent;
}

namespace sprite{


	class SpriteRenderer{

		struct vpostex{
			float pos[3];
			float uv[2];
		};

	public:

		render::DrawablesQueue m_queue;
		Camera m_camera;
		dx::ShaderResource m_spriteShaderRes;

		dx::State m_defaultVertexInput; //vertex, index, input layout, primitive topology
		dx::DrawIndexed m_drawIndexed;

		BlendStateBinders m_blends;
		TextureBinders m_texs;
		SamplerStateBinders m_samplers;

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		SpriteRenderer( dx::Device * pDevice_p )
			:
		m_spriteShaderRes("namespace sprite/SpriteV9.hlsl", 1),
		m_blends(pDevice_p),
		m_texs(pDevice_p),
		m_samplers(pDevice_p),
		m_drawIndexed(6)
		{}

		SpriteRenderer(): m_drawIndexed(6){}
		virtual ~SpriteRenderer(){};

		//------------------------------------------------------------------------
		// delayed "ctor"
		//------------------------------------------------------------------------
		void Init( dx::Device * pDevice_p ){

			pDevice_p = pDevice_p;
			m_spriteShaderRes.Init( "Contents/SpriteV9.hlsl", 1 );
			m_blends.Init(pDevice_p);
			m_texs.Init(pDevice_p);
			m_samplers.Init(pDevice_p);
		}
	
		//------------------------------------------------------------------------
		// queue the states of a sprite
		//------------------------------------------------------------------------
		void Render( Sprite * pSprite_p );
		void Render( game::SpriteComponent *pSprite_p);
		void Render( game::SpriteComponent *pSprite_p, Camera *pCamera_p );

		//------------------------------------------------------------------------
		// batch commands and executes
		//------------------------------------------------------------------------
		void Raster( ID3D11DeviceContext * pContext_p ){

			dx::commandbuffer cmds;
			m_queue.CreateCommandBuffer( cmds , false);
			ExecuteRenderCommands(cmds, pContext_p);
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void LoadShader( dx::Device *pDevice_p, const char * szVS_p,const char * szPS_p );


	private:

		//------------------------------------------------------------------------
		// Creates a state that binds input layout, vertex and index buffer for
		// a 1x1 quad, with 0 to 1 uv
		//------------------------------------------------------------------------
		void CreateDefaultVertexInputState( ID3DBlob * pShaderBytes_p, dx::Device * pDevice_p );

		//------------------------------------------------------------------------
		// execute given commands
		//------------------------------------------------------------------------
		void ExecuteRenderCommands( const dx::commandbuffer & cmds_p, ID3D11DeviceContext * pContext_p ){

			for(	dx::commandbuffer::const_iterator it = cmds_p.begin(),
					itEnd = cmds_p.end();
					it != itEnd;
					++it ){

				(*it).get()->Execute(pContext_p);
			}
		}
	};
}
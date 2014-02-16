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
#include "InstancedSprites.h"

namespace game{
	class SpriteComponent;
	class SpriteComponent_;
}

namespace sprite{


	class SpriteRenderer{

		struct vpostex{
			float pos[3];
			float uv[2];
		};

	public:		

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		SpriteRenderer( dx::Device * pDevice_p, int maxInstances_p = 128 );
		SpriteRenderer();
		virtual ~SpriteRenderer(){};

		//------------------------------------------------------------------------
		// delayed "ctor"
		//------------------------------------------------------------------------
		void Init( dx::Device * pDevice_p, int maxInstances_p = 128 );

		//------------------------------------------------------------------------
		// queue the states of a sprite
		//------------------------------------------------------------------------
		void Render( Sprite * pSprite_p );
		void Render( game::SpriteComponent *pSprite_p);
		void Render( game::SpriteComponent *pSprite_p, Camera *pCamera_p );
		void Render( game::SpriteComponent_ *pSprite_p, Camera *pCamera_p );
		void Render( InstancedSprites * pInstSprites, Camera *pCamera_p );

		//------------------------------------------------------------------------
		// batch commands and executes
		//------------------------------------------------------------------------
		void Raster( ID3D11DeviceContext * pContext_p );

		render::DrawablesQueue	m_queue;		

		dx::PipeState	m_defaultVertexInput; //vertex, index, input layout, primitive topology
		dx::DrawIndexed m_drawIndexed;

		dx::PipeState	m_instancedVertexInput;

		BlendStateBinders	m_blends_cache;
		TextureBinders		m_tex2D_cache;
		SamplerStateBinders	m_samplers_cache;
		dx::ShaderResource	m_spriteShaderRes;

		Camera m_camera;

		InstancesVertexBuffer		m_dynamic_tmp_instancesVB;	

	private:

		// TODO: a cache class for each of those binders would be the right thing
		dx::BindIAPrimitiveTopology m_bindPrimitiveTopo;
		dx::BindIAInputLayout		m_bindIAInputLayout; // TODO: should be tied with vertex shaders some how
		dx::BindIAInputLayout		m_bindIAInputLayoutInstanced;
		dx::BindIAVertexBuffer		m_bindVB;
		dx::BindIAIndexBuffer		m_bindIB;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void LoadShader( dx::Device *pDevice_p, UINT maxInstances_p );

		//------------------------------------------------------------------------
		// Creates a state that binds input layout, vertex and index buffer for
		// a 1x1 quad, with 0 to 1 uv
		//------------------------------------------------------------------------
		void CreateDefaultVertexInputState( ID3DBlob * pShaderBytes_p, dx::Device * pDevice_p );
		void CreateInstacedVertexInputState( ID3DBlob * pShaderBytes_p, dx::Device * pDevice_p, UINT maxInstances_p );

		//------------------------------------------------------------------------
		// execute given commands
		//------------------------------------------------------------------------
		void ExecuteRenderCommands( const render::RenderCommands & cmds_p, ID3D11DeviceContext * pContext_p );
	};
}
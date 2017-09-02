#include "PCHF.h"

#include "SpriteRenderer.h"

#include <D3D11Shader.h>

#include "../namespace render/dx/HLSLResource.h"
#include "../namespace game/components/SpriteComponent_.h"

#include <algorithm>

sprite::SpriteRenderer::SpriteRenderer( dx::Device * pDevice_p, int maxInstances_p )
	:
	m_spriteShaderRes("namespace sprite/InstancedSpriteV10.hlsl", 1, 2, pDevice_p ),
	m_blends_cache(pDevice_p),
	m_tex2D_cache(pDevice_p),
	m_samplers_cache(pDevice_p),
	m_drawIndexed(6),
	m_bindPrimitiveTopo(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	LoadShader(pDevice_p, maxInstances_p );

	m_drawableAux.Initialize(m_drawablePipeMemBuff.StackAlloc(6u), 6u);
}
sprite::SpriteRenderer::SpriteRenderer()
	:
	m_drawIndexed(6),
	m_bindPrimitiveTopo(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

void sprite::SpriteRenderer::Init( dx::Device * pDevice_p, int maxInstances_p )
{
	m_spriteShaderRes.Init( "Contents/InstancedSpriteV10.hlsl", 2, 2, pDevice_p );
	m_blends_cache.Init(pDevice_p);
	m_tex2D_cache.Init(pDevice_p);
	m_samplers_cache.Init(pDevice_p);

	LoadShader( pDevice_p, maxInstances_p );

	m_drawableAux.Initialize(m_drawablePipeMemBuff.StackAlloc(6u), 6u);
}

void sprite::SpriteRenderer::Render( game::SpriteComponent_ *pSprite_p, Camera *pCamera_p )
{
	m_drawableAux.Clear();

	pSprite_p->m_sortKey.bitfield.shaderID = 0;
	m_drawableAux.SetSortKey( pSprite_p->m_sortKey.intRepresentation );
	// sprite
	m_drawableAux.AddPipelineState( &pSprite_p->m_pipeState_cb_tex_ss_bs );
	// shader
	m_drawableAux.AddPipelineState( &m_spriteShaderRes.m_permutations[pSprite_p->m_iShaderPermutation].m_pipeState );
	// vb
	m_drawableAux.AddPipelineState( &m_defaultVertexInput_to_il_vb_ib );
	// camera
	m_drawableAux.AddPipelineState( &pCamera_p->m_pipeState_vp_rt_cb );
	

	// draw call
	m_drawableAux.SetDrawCall( &m_drawIndexed );

	m_queue.Submit( m_drawableAux );
}

//void sprite::SpriteRenderer::Render( InstancedSprites * pInstSprites, Camera *pCamera_p )
//{
//	SortMask sortKey;
//	sortKey.intRepresentation = pInstSprites->m_drawable.GetSortKey();
//	sortKey.bitfield.shaderID = 1;
//
//	if( pInstSprites->m_drawCall_warpException.GetNInstances() ){
//
//		m_drawableAux = pInstSprites->m_drawable_warpException;
//
//		m_drawableAux.AddPipelineState( &m_spriteShaderRes.m_permutations/*[1]*/.m_pipeState );
//		m_drawableAux.AddPipelineState( &m_instancedVertexInput );
//		m_drawableAux.AddPipelineState( &pCamera_p->m_pipeState_vp_rt_cb );
//
//		m_drawableAux.SetSortKey( sortKey.intRepresentation );
//
//		m_queue.Submit( m_drawableAux );
//	}
//	if( pInstSprites->m_drawCall.GetNInstances() ){
//
//		m_drawableAux = pInstSprites->m_drawable;
//
//		m_drawableAux.AddPipelineState( &m_spriteShaderRes.m_permutations/*[1]*/.m_pipeState );
//		m_drawableAux.AddPipelineState( &m_instancedVertexInput );
//		m_drawableAux.AddPipelineState( &pCamera_p->m_pipeState_vp_rt_cb );
//
//		m_drawableAux.SetSortKey( sortKey.intRepresentation );
//
//		m_queue.Submit( m_drawableAux );
//	}
//}

void sprite::SpriteRenderer::Render( render::Drawable * pInstDrawable, Camera *pCamera_p )
{	
	m_drawableAux = *pInstDrawable;

	SortMask sortKey;
	sortKey.intRepresentation = pInstDrawable->GetSortKey();
	sortKey.bitfield.shaderID = 1;
	m_drawableAux.SetSortKey(sortKey.intRepresentation);

	m_drawableAux.AddPipelineState( &m_spriteShaderRes.m_permutations[1].m_pipeState );
	m_drawableAux.AddPipelineState( &m_instancedVertexInput_to_il_vb_ib_ivb );
	m_drawableAux.AddPipelineState( &pCamera_p->m_pipeState_vp_rt_cb );

	m_queue.Submit(m_drawableAux);
}

void sprite::SpriteRenderer::LoadShader( dx::Device *pDevice_p, UINT maxInstances_p )
{
	ID3DBlob *pShaderBytes;

	// VS
	
	m_spriteShaderRes.LoadShaderProgram( dx::E_VertexShader, pDevice_p, "vs_Sprite", "vs_4_0", true, &pShaderBytes );
	CreateDefaultVertexInputState( pShaderBytes, pDevice_p );
	pShaderBytes->Release();


	// PS

	m_spriteShaderRes.LoadShaderProgram( dx::E_PixelShader, pDevice_p, "ps_Sprite", "ps_4_0", true, &pShaderBytes );
	pShaderBytes->Release();

	// VS

	// compile shader

	m_spriteShaderRes.LoadShaderProgram( dx::E_VertexShader, pDevice_p, "vs_instancedSprite", "vs_4_0", true, &pShaderBytes, 0, 1 );
	CreateInstacedVertexInputState( pShaderBytes, pDevice_p, maxInstances_p );
	pShaderBytes->Release();

	// PS

	m_spriteShaderRes.LoadShaderProgram( dx::E_PixelShader, pDevice_p, "ps_instancedSprite", "ps_4_0", true, &pShaderBytes, 0, 1 );
	pShaderBytes->Release();
}

void sprite::SpriteRenderer::CreateDefaultVertexInputState( ID3DBlob * pShaderBytes_p, dx::Device * pDevice_p )
{
	// TODO: why not put the shaders binders here?
	m_defaultVertexInput_to_il_vb_ib.Initialize(pDevice_p->m_pPipeBindsMem->StackAlloc(4u), 4u);

	// primitive topology
	m_defaultVertexInput_to_il_vb_ib.AddBinderCommand(  &m_bindPrimitiveTopo );

	// input layout

	dx::InputLayout::CreationParams params = {0};
	params.pShaderSig = pShaderBytes_p->GetBufferPointer();
	params.iSizeShaderSig = pShaderBytes_p->GetBufferSize();
	params.desc.nElements = 2;
	params.desc.pInputElementsDescs.reset(new D3D11_INPUT_ELEMENT_DESC[params.desc.nElements](), &gen::ArrayDeleter<D3D11_INPUT_ELEMENT_DESC>);

	params.desc.pInputElementsDescs.get()[0].SemanticName =  "POSITION";
	//params.desc.pInputElementsDescs[0].SemanticIndex = 0;
	params.desc.pInputElementsDescs.get()[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	params.desc.pInputElementsDescs.get()[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	params.desc.pInputElementsDescs.get()[1].SemanticName =  "TEXCOORD";
	//params.desc.pInputElementsDescs[0].SemanticIndex = 0;
	params.desc.pInputElementsDescs.get()[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	params.desc.pInputElementsDescs.get()[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	params.desc.pInputElementsDescs.get()[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	ID3D11InputLayout * pInputLayout = NULL;
	pDevice_p->m_pCacheInputLayout->Acquire(params, pInputLayout);

	m_bindIAInputLayout.Initialize( pInputLayout );

	m_defaultVertexInput_to_il_vb_ib.AddBinderCommand( &m_bindIAInputLayout );

	// vertex buffer

	vpostex vData[] = {

		{{-0.5f, 0.5f, 0.5f},	{0.0f, 0.0f}},			//0
		{{ 0.5f, 0.5f, 0.5f},	{1.0f, 0.0f}},			//1
		{{-0.5f,-0.5f, 0.5f},	{0.0f, 1.0f}},			//2
		{{ 0.5f, -0.5f, 0.5f},	{1.0f, 1.0f}}			//3
	};

	dx::BufferResource::CreationParams vbParams = {0};
	vbParams.desc.bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbParams.desc.bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbParams.desc.bufferDesc.ByteWidth = sizeof(vpostex)*4;
	D3D11_SUBRESOURCE_DATA bufferData = {0};
	bufferData.pSysMem = (void*)vData;
	vbParams.pInitialData = &bufferData;

	ID3D11Buffer * pVB = NULL;
	pDevice_p->m_pCacheBuffer->Acquire(vbParams, pVB);

	m_bindVB.Initialize( 0, pVB, (UINT)(sizeof(float)*5) );

	m_defaultVertexInput_to_il_vb_ib.AddBinderCommand( &m_bindVB );

	// index buffer

	WORD iData[] = {
		0,1,2,
		2,1,3
	};

	vbParams.desc.bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vbParams.desc.bufferDesc.ByteWidth = sizeof(WORD)*6;
	bufferData.pSysMem = (void*)iData;

	//pVB = NULL;
	pDevice_p->m_pCacheBuffer->Acquire( vbParams, pVB );

	m_bindIB.Initialize(pVB);
	m_defaultVertexInput_to_il_vb_ib.AddBinderCommand( &m_bindIB );
}
void sprite::SpriteRenderer::CreateInstacedVertexInputState( ID3DBlob * pShaderBytes_p, dx::Device * pDevice_p, UINT maxInstances_p )
{
	// TODO: why not put the shaders binders here?

	m_instancedVertexInput_to_il_vb_ib_ivb.Initialize(pDevice_p->m_pPipeBindsMem->StackAlloc(5u), 5u);

	// primitive topology
	m_instancedVertexInput_to_il_vb_ib_ivb.AddBinderCommand(  &m_bindPrimitiveTopo );

	//------------------------------------------------------------------------
	// input layout 
	// NOTE: wheres my code using reflection? e_e", I probably gave up as it was
	// taking too long or something..lost in some commit
	//------------------------------------------------------------------------

	dx::InputLayout::CreationParams params = {0};
	params.pShaderSig = pShaderBytes_p->GetBufferPointer();
	params.iSizeShaderSig = pShaderBytes_p->GetBufferSize();

	params.desc.nElements = 10;
	params.desc.pInputElementsDescs.reset(	new D3D11_INPUT_ELEMENT_DESC[params.desc.nElements](),
											&gen::ArrayDeleter<D3D11_INPUT_ELEMENT_DESC>);

	//#1
	params.desc.pInputElementsDescs.get()[0].SemanticName =  "POSITION";
	//params.desc.pInputElementsDescs[0].SemanticIndex = 0;
	params.desc.pInputElementsDescs.get()[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	params.desc.pInputElementsDescs.get()[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//#2
	params.desc.pInputElementsDescs.get()[1].SemanticName =  "TEXCOORD";
	//params.desc.pInputElementsDescs[0].SemanticIndex = 0;
	params.desc.pInputElementsDescs.get()[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	params.desc.pInputElementsDescs.get()[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	params.desc.pInputElementsDescs.get()[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	//#3
	params.desc.pInputElementsDescs.get()[2].SemanticName =  "INST_RES";
	//params.desc.pInputElementsDescs[0].SemanticIndex = 0;
	params.desc.pInputElementsDescs.get()[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	params.desc.pInputElementsDescs.get()[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	params.desc.pInputElementsDescs.get()[2].AlignedByteOffset = 0;
	params.desc.pInputElementsDescs.get()[2].InputSlot = 1;
	params.desc.pInputElementsDescs.get()[2].InstanceDataStepRate = 1;

	//#4
	params.desc.pInputElementsDescs.get()[3].SemanticName =  "INST_PADD";
	//params.desc.pInputElementsDescs[0].SemanticIndex = 0;
	params.desc.pInputElementsDescs.get()[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	params.desc.pInputElementsDescs.get()[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	params.desc.pInputElementsDescs.get()[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	params.desc.pInputElementsDescs.get()[3].InputSlot = 1;
	params.desc.pInputElementsDescs.get()[3].InstanceDataStepRate = 1;

	//#5
	params.desc.pInputElementsDescs.get()[4].SemanticName =  "INST_TEXTCOORD";
	//params.desc.pInputElementsDescs[0].SemanticIndex = 0;
	params.desc.pInputElementsDescs.get()[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	params.desc.pInputElementsDescs.get()[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	params.desc.pInputElementsDescs.get()[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	params.desc.pInputElementsDescs.get()[4].InputSlot = 1;
	params.desc.pInputElementsDescs.get()[4].InstanceDataStepRate = 1;

	//#6 - matrix
	params.desc.pInputElementsDescs.get()[5].SemanticName =  "INST_WORLD";
	params.desc.pInputElementsDescs.get()[5].SemanticIndex = 0;
	params.desc.pInputElementsDescs.get()[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	params.desc.pInputElementsDescs.get()[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	params.desc.pInputElementsDescs.get()[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	params.desc.pInputElementsDescs.get()[5].InputSlot = 1;
	params.desc.pInputElementsDescs.get()[5].InstanceDataStepRate = 1;
	//#7 - matrix
	params.desc.pInputElementsDescs.get()[6].SemanticName =  "INST_WORLD";
	params.desc.pInputElementsDescs.get()[6].SemanticIndex = 1;
	params.desc.pInputElementsDescs.get()[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	params.desc.pInputElementsDescs.get()[6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	params.desc.pInputElementsDescs.get()[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	params.desc.pInputElementsDescs.get()[6].InputSlot = 1;
	params.desc.pInputElementsDescs.get()[6].InstanceDataStepRate = 1;
	//#8 - matrix
	params.desc.pInputElementsDescs.get()[7].SemanticName =  "INST_WORLD";
	params.desc.pInputElementsDescs.get()[7].SemanticIndex = 2;
	params.desc.pInputElementsDescs.get()[7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	params.desc.pInputElementsDescs.get()[7].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	params.desc.pInputElementsDescs.get()[7].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	params.desc.pInputElementsDescs.get()[7].InputSlot = 1;
	params.desc.pInputElementsDescs.get()[7].InstanceDataStepRate = 1;
	//#9 - matrix
	params.desc.pInputElementsDescs.get()[8].SemanticName =  "INST_WORLD";
	params.desc.pInputElementsDescs.get()[8].SemanticIndex = 3;
	params.desc.pInputElementsDescs.get()[8].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	params.desc.pInputElementsDescs.get()[8].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	params.desc.pInputElementsDescs.get()[8].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	params.desc.pInputElementsDescs.get()[8].InputSlot = 1;
	params.desc.pInputElementsDescs.get()[8].InstanceDataStepRate = 1;

	//#10
	params.desc.pInputElementsDescs.get()[9].SemanticName =  "INST_COLOR";
	params.desc.pInputElementsDescs.get()[9].SemanticIndex = 0;
	params.desc.pInputElementsDescs.get()[9].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	params.desc.pInputElementsDescs.get()[9].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	params.desc.pInputElementsDescs.get()[9].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	params.desc.pInputElementsDescs.get()[9].InputSlot = 1;
	params.desc.pInputElementsDescs.get()[9].InstanceDataStepRate = 1;

	ID3D11InputLayout * pInputLayout = NULL;
	pDevice_p->m_pCacheInputLayout->Acquire(params, pInputLayout);
	m_bindIAInputLayoutInstanced.Initialize( pInputLayout );

	m_instancedVertexInput_to_il_vb_ib_ivb.AddBinderCommand( &m_bindIAInputLayoutInstanced );
	//------------------------------------------------------------------------


	//------------------------------------------------------------------------
	// vertex buffer
	//------------------------------------------------------------------------

	vpostex vData[] = {

		{{-0.5f, 0.5f, 0.5f},	{0.0f, 0.0f}},			//0
		{{ 0.5f, 0.5f, 0.5f},	{1.0f, 0.0f}},			//1
		{{-0.5f,-0.5f, 0.5f},	{0.0f, 1.0f}},			//2
		{{ 0.5f, -0.5f, 0.5f},	{1.0f, 1.0f}}			//3
	};

	dx::BufferResource::CreationParams vbParams = {0};
	vbParams.desc.bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbParams.desc.bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbParams.desc.bufferDesc.ByteWidth = sizeof(vpostex)*4;
	D3D11_SUBRESOURCE_DATA bufferData = {0};
	bufferData.pSysMem = (void*)vData;
	vbParams.pInitialData = &bufferData;

	ID3D11Buffer * pVB = NULL;
	pDevice_p->m_pCacheBuffer->Acquire(vbParams, pVB);

	m_bindVB.Initialize( 0, pVB, (UINT)(sizeof(float)*5) );

	m_instancedVertexInput_to_il_vb_ib_ivb.AddBinderCommand( &m_bindVB );
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// index buffer
	//------------------------------------------------------------------------
	
	WORD iData[] = {
		0,1,2,
		2,1,3
	};

	vbParams.desc.bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vbParams.desc.bufferDesc.ByteWidth = sizeof(WORD)*6;
	bufferData.pSysMem = (void*)iData;

	//pVB = NULL;
	pDevice_p->m_pCacheBuffer->Acquire( vbParams, pVB );

	m_bindIB.Initialize(pVB);
	m_instancedVertexInput_to_il_vb_ib_ivb.AddBinderCommand( &m_bindIB );
	//------------------------------------------------------------------------ 

	// the only thing missing is the instance data

	////------------------------------------------------------------------------
	//// instance buffer
	////------------------------------------------------------------------------
	//vbParams.desc.bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbParams.desc.bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//vbParams.desc.bufferDesc.ByteWidth = sizeof(spriteInstance)*maxInstances_p;
	//vbParams.desc.bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	////bufferData.pSysMem = nullptr;
	//vbParams.pInitialData = nullptr;//&bufferData;

	//pDevice_p->m_pCacheBuffer->Acquire(vbParams, pVB);

	//m_dynamic_tmp_instancesVB.Initialize( maxInstances_p, 1, pVB );

	//m_instancedVertexInput_to_il_vb_ib_ivb.AddBinderCommand( m_dynamic_tmp_instancesVB.GetBinder());
	maxInstances_p = maxInstances_p;
	////------------------------------------------------------------------------ 
}

void sprite::SpriteRenderer::Raster( ID3D11DeviceContext * pContext_p )
{
	m_cmds.clear();
	//render::RenderCommands cmds;

	m_queue.CreateCommandBuffer(m_cmds, false);

	ExecuteRenderCommands(m_cmds, pContext_p);
}

void sprite::SpriteRenderer::ExecuteRenderCommands( const render::RenderCommands & cmds_p, ID3D11DeviceContext * pContext_p )
{
	for (int it = 0; it < cmds_p.size(); ++it ) {

		cmds_p[it]->Execute(pContext_p);
	}
}
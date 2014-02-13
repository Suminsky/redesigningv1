#include "SpriteRenderer.h"

#include <D3D11Shader.h>

#include "../namespace render/dx/HLSLResource.h"
#include "../namespace game/components/SpriteComponent.h"
#include "../namespace game/components/SpriteComponent_.h"
#include <algorithm>



sprite::SpriteRenderer::SpriteRenderer( dx::Device * pDevice_p, int maxInstances_p )
	:
	m_spriteShaderRes("namespace sprite/InstancedSpriteV10.hlsl", 1, 2, pDevice_p ),
	m_blends(pDevice_p),
	m_texs(pDevice_p),
	m_samplers(pDevice_p),
	m_drawIndexed(6), m_drawIndexedInstanced(6),
	m_maxInstances(maxInstances_p), m_currentInstancesUsed(0),
	m_bindPrimitiveTopo(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	LoadShader(pDevice_p);

	m_vSpriteInstances.reserve(maxInstances_p);
	m_vInstancesSort.reserve(maxInstances_p);
	m_vSpriteInstancesTmp.reserve(maxInstances_p);
}

sprite::SpriteRenderer::SpriteRenderer()
	:
	m_drawIndexed(6), m_drawIndexedInstanced(6), m_currentInstancesUsed(0),
	m_bindPrimitiveTopo(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{

}

void sprite::SpriteRenderer::Init( dx::Device * pDevice_p, int maxInstances_p )
{
	pDevice_p = pDevice_p;
	m_spriteShaderRes.Init( "Contents/InstancedSpriteV10.hlsl", 2, 2, pDevice_p );
	m_blends.Init(pDevice_p);
	m_texs.Init(pDevice_p);
	m_samplers.Init(pDevice_p);
	m_vSpriteInstances.reserve(maxInstances_p);
	m_vInstancesSort.reserve(maxInstances_p);
	m_vSpriteInstancesTmp.reserve(maxInstances_p);
	m_maxInstances = maxInstances_p;

	LoadShader(pDevice_p);
}

void sprite::SpriteRenderer::Raster( ID3D11DeviceContext * pContext_p )
{
	render::RenderCommands cmds;
	m_queue.CreateCommandBuffer( cmds , false);
	ExecuteRenderCommands(cmds, pContext_p);
}


void sprite::SpriteRenderer::Render( Sprite * pSprite_p )
{
	static render::Drawable drawInst;
	drawInst.Clear();

	drawInst.SetSortKey( pSprite_p->m_renderSortKey.intRepresentation );
	// shader
	drawInst.AddPipelineState( &m_spriteShaderRes.m_permutations[pSprite_p->m_iCurrentPermutationIndex].m_pipeState );
	// vb
	drawInst.AddPipelineState( &m_defaultVertexInput );
	// camera
	drawInst.AddPipelineState( &m_camera.m_pipeState );
	// sprite
	drawInst.AddPipelineState( &pSprite_p->m_pipeState );

	// draw call
	drawInst.SetDrawCall( &m_drawIndexed );

	m_queue.Submit( drawInst );
}
void sprite::SpriteRenderer::Render( game::SpriteComponent * pSprite_p )
{
	static render::Drawable drawInst;
	drawInst.Clear();

	drawInst.SetSortKey( pSprite_p->m_sortKey.intRepresentation );
	// shader
	drawInst.AddPipelineState( &m_spriteShaderRes.m_permutations[pSprite_p->m_iShaderPermutation].m_pipeState );
	// vb
	drawInst.AddPipelineState( &m_defaultVertexInput );
	// camera
	drawInst.AddPipelineState( &m_camera.m_pipeState );
	// sprite
	drawInst.AddPipelineState( &pSprite_p->m_pipeState );

	// draw call
	drawInst.SetDrawCall( &m_drawIndexed );

	m_queue.Submit( drawInst );
}
void sprite::SpriteRenderer::Render( game::SpriteComponent *pSprite_p, Camera *pCamera_p )
{

	static render::Drawable s_drawInst;
	s_drawInst.Clear();

	s_drawInst.SetSortKey( pSprite_p->m_sortKey.intRepresentation );
	// shader
	s_drawInst.AddPipelineState( &m_spriteShaderRes.m_permutations[pSprite_p->m_iShaderPermutation].m_pipeState );
	// vb
	s_drawInst.AddPipelineState( &m_defaultVertexInput );
	// camera
	s_drawInst.AddPipelineState( &pCamera_p->m_pipeState );
	// sprite
	s_drawInst.AddPipelineState( &pSprite_p->m_pipeState );

	// draw call
	s_drawInst.SetDrawCall( &m_drawIndexed );

	m_queue.Submit( s_drawInst );
}
void sprite::SpriteRenderer::Render( game::SpriteComponent_ *pSprite_p, Camera *pCamera_p )
{

	static render::Drawable s_drawInst;
	s_drawInst.Clear();

	s_drawInst.SetSortKey( pSprite_p->m_sortKey.intRepresentation );
	// shader
	s_drawInst.AddPipelineState( &m_spriteShaderRes.m_permutations[pSprite_p->m_iShaderPermutation].m_pipeState );
	// vb
	s_drawInst.AddPipelineState( &m_defaultVertexInput );
	// camera
	s_drawInst.AddPipelineState( &pCamera_p->m_pipeState );
	// sprite
	s_drawInst.AddPipelineState( &pSprite_p->m_pipeState );

	// draw call
	s_drawInst.SetDrawCall( &m_drawIndexed );

	m_queue.Submit( s_drawInst );
}

void sprite::SpriteRenderer::LoadShader( dx::Device *pDevice_p )
{
	// VS

	ID3DBlob *pShaderBytes;
	m_spriteShaderRes.LoadShaderProgram( dx::E_VertexShader, pDevice_p, "vs_Sprite", "vs_4_0", true, &pShaderBytes );

	CreateDefaultVertexInputState( pShaderBytes, pDevice_p );

	// PS

	m_spriteShaderRes.LoadShaderProgram( dx::E_PixelShader, pDevice_p, "ps_Sprite", "ps_4_0", true, &pShaderBytes );

	// VS

	// compile shader

	m_spriteShaderRes.LoadShaderProgram( dx::E_VertexShader, pDevice_p, "vs_instancedSprite", "vs_4_0", true, &pShaderBytes, 0, 1 );

	CreateInstacedVertexInputState( pShaderBytes, pDevice_p );

	// PS

	m_spriteShaderRes.LoadShaderProgram( dx::E_PixelShader, pDevice_p, "ps_instancedSprite", "ps_4_0", true, &pShaderBytes, 0, 1 );
}

void sprite::SpriteRenderer::CreateDefaultVertexInputState( ID3DBlob * pShaderBytes_p, dx::Device * pDevice_p )
{
	// TODO: why not put the shaders binders here?
	
	// primitive topology
	m_defaultVertexInput.AddBinderCommand(  &m_bindPrimitiveTopo );

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

	m_defaultVertexInput.AddBinderCommand( &m_bindIAInputLayout );

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

	m_defaultVertexInput.AddBinderCommand( &m_bindVB );

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
	m_defaultVertexInput.AddBinderCommand( &m_bindIB );
}
void sprite::SpriteRenderer::CreateInstacedVertexInputState( ID3DBlob * pShaderBytes_p, dx::Device * pDevice_p )
{
	// TODO: why not put the shaders binders here?

	// primitive topology
	m_instancedVertexInput.AddBinderCommand(  &m_bindPrimitiveTopo );

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

	m_instancedVertexInput.AddBinderCommand( &m_bindIAInputLayoutInstanced );
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

	m_instancedVertexInput.AddBinderCommand( &m_bindVB );
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
	m_instancedVertexInput.AddBinderCommand( &m_bindIB );
	//------------------------------------------------------------------------ 

	// the only thing missing is the instance data

	//------------------------------------------------------------------------
	// instance buffer
	//------------------------------------------------------------------------
	vbParams.desc.bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbParams.desc.bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbParams.desc.bufferDesc.ByteWidth = sizeof(spriteInstance)*m_maxInstances;
	vbParams.desc.bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//bufferData.pSysMem = nullptr;
	vbParams.pInitialData = nullptr;//&bufferData;

	pDevice_p->m_pCacheBuffer->Acquire(vbParams, pVB);

	m_bindInstVB.Initialize( 1, pVB, (UINT)(sizeof(float)*28) );

	m_instancedVertexInput.AddBinderCommand( &m_bindInstVB );
	//------------------------------------------------------------------------ 
}

void sprite::SpriteRenderer::BeginInstancing( dx::BindPSShaderResourceView * pTextureBinder_p, dx::BindOMBlendState * pBlendBinder, dx::BindPSSampler * pSampleState )
{
	// TODO: this allows for only one instanced draw call per frame, cause they will overwrite...means this is pretty stupid
	// TODO: camera should be a parameter

	m_currentInstancesUsed = 0;
	m_vSpriteInstances.clear();	m_vSpriteInstances.reserve(m_maxInstances);
	m_vInstancesSort.clear();	m_vInstancesSort.reserve(m_maxInstances);

	m_drawableInstances.Clear();

	m_drawableInstances.AddPipelineState( &m_spriteShaderRes.m_permutations[1].m_pipeState );
	m_drawableInstances.AddPipelineState( &m_instancedVertexInput );
	
	//m_drawableInstances.AddPipelineState( &m_camera.m_pipeState );

	static dx::PipeState instancesCommomPipeState;
	instancesCommomPipeState.Reset();
		instancesCommomPipeState.AddBinderCommand( pTextureBinder_p );
		instancesCommomPipeState.AddBinderCommand( pBlendBinder );
		instancesCommomPipeState.AddBinderCommand( pSampleState );
	m_drawableInstances.AddPipelineState( &instancesCommomPipeState );

	m_drawableInstances.SetDrawCall( &m_drawIndexedInstanced );
}

void sprite::SpriteRenderer::RenderInstance( spriteInstance & spriteData_p, UINT64 sortKey_p )
{
	assert( (int)m_vSpriteInstances.size() < m_maxInstances );

	m_vSpriteInstances.push_back(spriteData_p);

	InstanceEntry entry = { sortKey_p, m_currentInstancesUsed };
	m_vInstancesSort.push_back(entry);

	++m_currentInstancesUsed;
}

void sprite::SpriteRenderer::RenderInstance( game::SpriteComponent_ *pSprite_p )
{
	spriteInstance instanceData;
	memcpy( (void*)&instanceData, (void*)&pSprite_p->m_renderData, sizeof(float)*28);

	RenderInstance( instanceData, pSprite_p->m_sortKey.intRepresentation );

}

void sprite::SpriteRenderer::FinishInstancingRender( dx::Device * pDevice_p, UINT64 sortKey_p )
{
	assert( m_currentInstancesUsed );

	m_drawIndexedInstanced.UpdateNumberOfInstances( m_currentInstancesUsed );

	// sort instance data

	std::sort( m_vInstancesSort.begin(),  m_vInstancesSort.end(), InstanceEntry() );

	// now re-order vector
	m_vSpriteInstancesTmp = m_vSpriteInstances;
	for( int it = 0; it < m_currentInstancesUsed; ++it ){

		m_vSpriteInstances[it] = m_vSpriteInstancesTmp[ m_vInstancesSort[it].index ];
	}

	// map data to vertex buffer

	D3D11_MAPPED_SUBRESOURCE mapped;// = {0};
	pDevice_p->GetContext()->Map( m_bindInstVB.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
	memcpy( mapped.pData, (void*)m_vSpriteInstances.data(), sizeof(spriteInstance)*m_currentInstancesUsed );
	pDevice_p->GetContext()->Unmap( m_bindInstVB.GetBuffer(), 0 );

	m_drawableInstances.SetSortKey( sortKey_p );
	m_queue.Submit( m_drawableInstances );
}

void sprite::SpriteRenderer::ExecuteRenderCommands( const render::RenderCommands & cmds_p, ID3D11DeviceContext * pContext_p )
{
	for(	render::RenderCommands::const_iterator it = cmds_p.begin(), itEnd = cmds_p.end();
			it != itEnd;
			++it ){

		(*it)->Execute(pContext_p);
	}
}
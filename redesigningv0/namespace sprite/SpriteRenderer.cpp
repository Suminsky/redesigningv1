#include "SpriteRenderer.h"

#include <D3D11Shader.h>

#include "../namespace render/dx/HLSLResource.h"
#include "../namespace game/components/SpriteComponent.h"

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
void sprite::SpriteRenderer::CreateDefaultVertexInputState( ID3DBlob * pShaderBytes_p, dx::Device * pDevice_p )
{
	// TODO: instead of statics, cache classes
	
	// primitive topology
	static dx::BindIAPrimitiveTopology s_bindPrimitiveTopo( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_defaultVertexInput.AddBinderCommand(  &s_bindPrimitiveTopo );

	// input layout

	dx::InputLayout::CreationParams params = {0};
	params.pShaderSig = pShaderBytes_p->GetBufferPointer();
	params.iSizeShaderSig = pShaderBytes_p->GetBufferSize();
	params.desc.nElements = 2;
	params.desc.pInputElementsDescs.reset(new D3D11_INPUT_ELEMENT_DESC[2](), &gen::ArrayDeleter<D3D11_INPUT_ELEMENT_DESC>);

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

	static dx::BindIAInputLayout s_bindISInputLayout( pInputLayout );

	m_defaultVertexInput.AddBinderCommand( &s_bindISInputLayout );

	// vertex buffer

	vpostex vData[] = {

		{{-0.5f, 0.5f, 0.5f},	{0.0f, 0.0f}},			//0
		{{ 0.5f, 0.5f, 0.5f},	{1.0f, 0.0f}},			//1
		{{-0.5f,-0.5f, 0.5f},	{0.0f, 1.0f}},			//2
		{{ 0.5f, -0.5f, 0.5f},	{1.0f, 1.0f}}			//3
	};
	//vpostex vData[] = {

	//	{{ 0.0f, 1.0f, 0.5f},	{0.0f, 0.0f}},			//0
	//	{{ 1.0f, 1.0f, 0.5f},	{1.0f, 0.0f}},			//1
	//	{{ 0.0f, 0.0f, 0.5f},	{0.0f, 1.0f}},			//2
	//	{{ 1.0f, 0.0f, 0.5f},	{1.0f, 1.0f}}			//3
	//};

	dx::BufferResource::CreationParams vbParams = {0};
	vbParams.desc.bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbParams.desc.bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbParams.desc.bufferDesc.ByteWidth = sizeof(vpostex)*4;
	D3D11_SUBRESOURCE_DATA bufferData = {0};
	bufferData.pSysMem = (void*)vData;
	vbParams.pInitialData = &bufferData;

	ID3D11Buffer * pVB = NULL;
	pDevice_p->m_pCacheBuffer->Acquire(vbParams, pVB);

	static dx::BindIAVertexBuffer s_bindVB( 0, pVB, (UINT)(sizeof(float)*5) );

	m_defaultVertexInput.AddBinderCommand( &s_bindVB );

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

	static dx::BindIAIndexBuffer s_bindIB(pVB);
	m_defaultVertexInput.AddBinderCommand( &s_bindIB );
}

void sprite::SpriteRenderer::LoadShader( dx::Device *pDevice_p, const char * szVS_p,const char * szPS_p )
{
	// VS

	// compile shader

	ID3DBlob *pShaderBytes;
	m_spriteShaderRes.LoadShaderProgram( dx::E_VertexShader, pDevice_p, szVS_p, "vs_4_0", true, &pShaderBytes );

	CreateDefaultVertexInputState( pShaderBytes, pDevice_p );

	/*ID3D11ShaderReflection * pReflection = NULL;

	if(pReflection){

	pReflection->Release();
	pReflection = NULL;
	}

	if(pShaderBytes){
	pShaderBytes->Release();
	pShaderBytes = NULL;
	}*/

	// PS

	m_spriteShaderRes.LoadShaderProgram( dx::E_PixelShader, pDevice_p, szPS_p, "ps_4_0", true, &pShaderBytes );

	/*m_spriteShaderRes.LoadResourceForShader( dx::E_PixelShader, pReflection, pShaderBytes, pDevice_p, "tex2D", D3D11_USAGE_IMMUTABLE, 0 );*/

	//if(pReflection){

	//	pReflection->Release();
	//	pReflection = NULL;
	//}

	//if(pShaderBytes){
	//	pShaderBytes->Release();
	//	pShaderBytes = NULL;
	//}
}

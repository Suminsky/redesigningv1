#include "SpriteRenderer.h"

#include <D3D11Shader.h>

#include "../namespace render/dx/HLSLResource.h"
#include "../namespace game/components/SpriteComponent.h"

void sprite::SpriteRenderer::Render( Sprite * pSprite_p )
{
	static render::Drawable drawInst;
	drawInst.m_PipeStatesGroup.resize(0);

	drawInst.m_sortKey = pSprite_p->m_renderSortKey.intRepresentation;
	// shader
	drawInst.m_PipeStatesGroup.push_back(m_spriteShaderRes.m_permutations[pSprite_p->m_iCurrentPermutationIndex].m_pPipeState);
	// vb
	drawInst.m_PipeStatesGroup.push_back( dx::shared_State_ptr(&m_defaultVertexInput, &gen::NoOp<dx::State>)  );
	// camera
	drawInst.m_PipeStatesGroup.push_back( dx::shared_State_ptr(&m_camera.m_pipeState, &gen::NoOp<dx::State>)  );
	// sprite
	drawInst.m_PipeStatesGroup.push_back(  dx::shared_State_ptr(&pSprite_p->m_pipeState, &gen::NoOp<dx::State>) );

	// draw call
	drawInst.m_pDrawCall.reset(&m_drawIndexed, &gen::NoOp<dx::DrawCall>);

	m_queue.Submit( &drawInst );
}
void sprite::SpriteRenderer::Render( game::SpriteComponent * pSprite_p )
{
	static render::Drawable drawInst;
	drawInst.m_PipeStatesGroup.resize(0);

	drawInst.m_sortKey = pSprite_p->m_sortKey.intRepresentation;
	// shader
	drawInst.m_PipeStatesGroup.push_back(m_spriteShaderRes.m_permutations[pSprite_p->m_iShaderPermutation].m_pPipeState);
	// vb
	drawInst.m_PipeStatesGroup.push_back( dx::shared_State_ptr(&m_defaultVertexInput, &gen::NoOp<dx::State>)  );
	// camera
	drawInst.m_PipeStatesGroup.push_back( dx::shared_State_ptr(&m_camera.m_pipeState, &gen::NoOp<dx::State>)  );
	// sprite
	drawInst.m_PipeStatesGroup.push_back(  dx::shared_State_ptr(&pSprite_p->m_pipeState, &gen::NoOp<dx::State>) );

	// draw call
	drawInst.m_pDrawCall.reset(&m_drawIndexed, &gen::NoOp<dx::DrawCall>);

	m_queue.Submit( &drawInst );
}

void sprite::SpriteRenderer::Render( game::SpriteComponent *pSprite_p, Camera *pCamera_p )
{
	// NOTE/TODO: even the given pointer being on the stack, shared ptr still allocates for the ref ctrl..

	static render::Drawable drawInst;
	drawInst.m_PipeStatesGroup.resize(0);

	drawInst.m_sortKey = pSprite_p->m_sortKey.intRepresentation;
	// shader
	drawInst.m_PipeStatesGroup.push_back(m_spriteShaderRes.m_permutations[pSprite_p->m_iShaderPermutation].m_pPipeState);
	// vb
	drawInst.m_PipeStatesGroup.push_back( dx::shared_State_ptr(&m_defaultVertexInput, &gen::NoOp<dx::State>)  );
	// camera
	drawInst.m_PipeStatesGroup.push_back( dx::shared_State_ptr(&pCamera_p->m_pipeState, &gen::NoOp<dx::State>)  );
	// sprite
	drawInst.m_PipeStatesGroup.push_back(  dx::shared_State_ptr(&pSprite_p->m_pipeState, &gen::NoOp<dx::State>) );

	// draw call
	drawInst.m_pDrawCall.reset(&m_drawIndexed, &gen::NoOp<dx::DrawCall>);

	m_queue.Submit( &drawInst );
}
void sprite::SpriteRenderer::CreateDefaultVertexInputState( ID3DBlob * pShaderBytes_p, dx::Device * pDevice_p )
{
	// primitive topology

	m_defaultVertexInput.AddBinderCommand(  std::make_shared<dx::BindIAPrimitiveTopology>( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ));

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

	m_defaultVertexInput.AddBinderCommand( std::make_shared<dx::BindIAInputLayout>( pInputLayout));

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

	m_defaultVertexInput.AddBinderCommand( std::make_shared< dx::BindIAVertexBuffer>(0, pVB, (UINT)(sizeof(float)*5) ) );

	// index buffer

	WORD iData[] = {
		0,1,2,
		2,1,3
	};

	vbParams.desc.bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vbParams.desc.bufferDesc.ByteWidth = sizeof(WORD)*6;
	bufferData.pSysMem = (void*)iData;

	//pVB = NULL;
	pDevice_p->m_pCacheBuffer->Acquire(vbParams, pVB);
	m_defaultVertexInput.AddBinderCommand( std::make_shared< dx::BindIAIndexBuffer>(pVB));
}

void sprite::SpriteRenderer::LoadShader( dx::Device *pDevice_p, const char * szVS_p,const char * szPS_p )
{
	// VS

	// compile shader

	ID3DBlob *pShaderBytes;
	m_spriteShaderRes.LoadShaderProgram( dx::E_VertexShader, pDevice_p, szVS_p, "vs_4_0", true, &pShaderBytes );

	CreateDefaultVertexInputState(pShaderBytes, pDevice_p);

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

	m_spriteShaderRes.LoadShaderProgram( dx::E_PixelShader, pDevice_p, szPS_p, "ps_4_0", true, &pShaderBytes);

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

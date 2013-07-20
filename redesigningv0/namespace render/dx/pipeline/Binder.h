//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/15
	created:	15:2:2013   18:17
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\namespace render\Binder.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\namespace dx\namespace render
	file base:	Binder
	file ext:	h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	This is a command pattern class.
				Binds stuff to the dx render trough given device context.

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// system/standard headers

#include <vector>
#include <assert.h>
//#include <boost/smart_ptr.hpp>

// private headers
#include "Command.h"


namespace dx{


	enum E_BIND{

		E_IA_InputLayout = 0,

		E_IA_VertexBuffer0,
		E_IA_VertexBuffer1,
		E_IA_VertexBuffer2,
		E_IA_VertexBuffer3,
		E_IA_VertexBuffer4,

		E_IA_IndexBuffer,
		E_IA_PrimitiveTopology,

		E_VertexShader,
		E_PixelShader,

		E_VS_CBuffer0,
		E_VS_CBuffer1,
		E_VS_CBuffer2,
		E_VS_CBuffer3,

		E_GS_CBuffer0,
		E_GS_CBuffer1,
		E_GS_CBuffer2,
		E_GS_CBuffer3,

		E_PS_CBuffer0,
		E_PS_CBuffer1,
		E_PS_CBuffer2,
		E_PS_CBuffer3,

		E_VS_ShaderResourceView0,
		E_VS_ShaderResourceView1,
		E_VS_ShaderResourceView2,
		E_VS_ShaderResourceView3,

		E_GS_ShaderResourceView0,
		E_GS_ShaderResourceView1,
		E_GS_ShaderResourceView2,
		E_GS_ShaderResourceView3,

		E_PS_ShaderResourceView0,
		E_PS_ShaderResourceView1,
		E_PS_ShaderResourceView2,
		E_PS_ShaderResourceView3,

		E_VS_Sampler0,
		E_VS_Sampler1,
		E_VS_Sampler2,
		E_VS_Sampler3,

		E_GS_Sampler0,
		E_GS_Sampler1,
		E_GS_Sampler2,
		E_GS_Sampler3,

		E_PS_Sampler0,
		E_PS_Sampler1,
		E_PS_Sampler2,
		E_PS_Sampler3,

		E_RS_ViewPort,
		E_RS_Scissor,
		E_RS_RasterizerState,

		E_OM_RenderTargetDepthStencil,
		E_OM_DepthStencilState,
		E_OM_BlendState,

		E_MAX_BINDS
	};

#pragma region constants

	static const UINT64 IA_InputLayout = 1LL << 0;

	static const UINT64 IA_VertexBuffer0 = 1LL << 1; // up to 16
	static const UINT64 IA_VertexBuffer1 = 1LL << 2;
	static const UINT64 IA_VertexBuffer2 = 1LL << 3;
	static const UINT64 IA_VertexBuffer3 = 1LL << 4;
	static const UINT64 IA_VertexBuffer4 = 1LL << 5;

	static const UINT64 IA_IndexBuffer = 1LL << 6;
	static const UINT64 IA_PrimitiveTopology = 1LL << 7;

	static const UINT64 VS_VertexShader = 1LL << 8;
	static const UINT64 PS_PixelShader = 1LL << 9;

	static const UINT64 VS_CBuffer0 = 1LL << 10;	// Each shader stage allows up to 15 shader-static constant buffers; each buffer can hold up to 4096 static constants.
	static const UINT64 VS_CBuffer1 = 1LL << 11;
	static const UINT64 VS_CBuffer2 = 1LL << 12;
	static const UINT64 VS_CBuffer3 = 1LL << 13;

	static const UINT64 GS_CBuffer0 = 1LL << 14;
	static const UINT64 GS_CBuffer1 = 1LL << 15;
	static const UINT64 GS_CBuffer2 = 1LL << 16;
	static const UINT64 GS_CBuffer3 = 1LL << 17;

	static const UINT64 PS_CBuffer0 = 1LL << 18;
	static const UINT64 PS_CBuffer1 = 1LL << 19;
	static const UINT64 PS_CBuffer2 = 1LL << 20;
	static const UINT64 PS_CBuffer3 = 1LL << 21;

	static const UINT64 VS_ShaderResourceView0 = 1LL << 22;	// 128 texture buffers!
	static const UINT64 VS_ShaderResourceView1 = 1LL << 23;
	static const UINT64 VS_ShaderResourceView2 = 1LL << 24;
	static const UINT64 VS_ShaderResourceView3 = 1LL << 25;

	static const UINT64 GS_ShaderResourceView0 = 1LL << 26;
	static const UINT64 GS_ShaderResourceView1 = 1LL << 27;
	static const UINT64 GS_ShaderResourceView2 = 1LL << 28;
	static const UINT64 GS_ShaderResourceView3 = 1LL << 29;

	static const UINT64 PS_ShaderResourceView0 = 1LL << 30;
	static const UINT64 PS_ShaderResourceView1 = 1LL << 31;
	static const UINT64 PS_ShaderResourceView2 = 1LL << 32;
	static const UINT64 PS_ShaderResourceView3 = 1LL << 33;

	static const UINT64 VS_Sampler0 = 1LL << 34;	// 128 sampler buffers!
	static const UINT64 VS_Sampler1 = 1LL << 35;
	static const UINT64 VS_Sampler2 = 1LL << 36;
	static const UINT64 VS_Sampler3 = 1LL << 37;

	static const UINT64 GS_Sampler0 = 1LL << 38;
	static const UINT64 GS_Sampler1 = 1LL << 39;
	static const UINT64 GS_Sampler2 = 1LL << 40;
	static const UINT64 GS_Sampler3 = 1LL << 41;

	static const UINT64 PS_Sampler0 = 1LL << 42;
	static const UINT64 PS_Sampler1 = 1LL << 43;
	static const UINT64 PS_Sampler2 = 1LL << 44;
	static const UINT64 PS_Sampler3 = 1LL << 45;

	static const UINT64 RS_ViewPort = 1LL << 46;	// 16 viewports can be bound; but only one can be active
													// To perform a per-primitive selection of the viewport in the geometry shader;
													// specify the ViewportArrayIndex semantic on the appropriate GS output component in 
													// the GS output signature declaration.
	static const UINT64 RS_Scissor = 1LL << 47; 
	static const UINT64 RS_RasterizerState = 1LL << 48;

	static const UINT64 OM_RenderTargetDepthStencil = 1LL << 49; // up yo 8 rts; but at the same time and always overwrite everyone already bound
	static const UINT64 OM_DepthStencilState = 1LL << 50;
	static const UINT64 OM_BlendState = 1LL << 51;	

#pragma endregion

	class Binder: public Command{

	protected:

		UINT64 m_typeBits;
		E_BIND m_typeIndex;

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Binder( UINT64 typeBits_p, E_BIND typeIndex_p): m_typeBits(typeBits_p), m_typeIndex(typeIndex_p){};
		Binder(){}
		virtual ~Binder(){};

		//------------------------------------------------------------------------
		// What this object binds?
		//------------------------------------------------------------------------
		UINT64 TypeBits() const { return m_typeBits;	}
		E_BIND TypeIndex() const { return m_typeIndex;	}
	};

	typedef std::shared_ptr<Binder> shared_Binder_ptr;
}

//========================================================================
// the binder commands definitions
//========================================================================
#include "BinderBinds.h"
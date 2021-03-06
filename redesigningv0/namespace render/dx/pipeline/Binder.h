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

	� Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
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

	enum class E_BIND_MASK : uint64_t {

		 IA_InputLayout = 1LL << 0,

		 IA_VertexBuffer0 = 1LL << 1, // up to 16
		 IA_VertexBuffer1 = 1LL << 2,
		 IA_VertexBuffer2 = 1LL << 3,
		 IA_VertexBuffer3 = 1LL << 4,
		 IA_VertexBuffer4 = 1LL << 5,

		 IA_IndexBuffer = 1LL << 6,
		 IA_PrimitiveTopology = 1LL << 7,

		 VS_VertexShader = 1LL << 8,
		 PS_PixelShader = 1LL << 9,

		 VS_CBuffer0 = 1LL << 10,	// Each shader stage allows up to 15 shader-static constant buffers, each buffer can hold up to 4096 static constants.
		 VS_CBuffer1 = 1LL << 11,
		 VS_CBuffer2 = 1LL << 12,
		 VS_CBuffer3 = 1LL << 13,

		 GS_CBuffer0 = 1LL << 14,
		 GS_CBuffer1 = 1LL << 15,
		 GS_CBuffer2 = 1LL << 16,
		 GS_CBuffer3 = 1LL << 17,

		 PS_CBuffer0 = 1LL << 18,
		 PS_CBuffer1 = 1LL << 19,
		 PS_CBuffer2 = 1LL << 20,
		 PS_CBuffer3 = 1LL << 21,

		 VS_ShaderResourceView0 = 1LL << 22,	// 128 texture buffers!
		 VS_ShaderResourceView1 = 1LL << 23,
		 VS_ShaderResourceView2 = 1LL << 24,
		 VS_ShaderResourceView3 = 1LL << 25,

		 GS_ShaderResourceView0 = 1LL << 26,
		 GS_ShaderResourceView1 = 1LL << 27,
		 GS_ShaderResourceView2 = 1LL << 28,
		 GS_ShaderResourceView3 = 1LL << 29,

		 PS_ShaderResourceView0 = 1LL << 30,
		 PS_ShaderResourceView1 = 1LL << 31,
		 PS_ShaderResourceView2 = 1LL << 32,
		 PS_ShaderResourceView3 = 1LL << 33,

		 VS_Sampler0 = 1LL << 34,	// 128 sampler buffers!
		 VS_Sampler1 = 1LL << 35,
		 VS_Sampler2 = 1LL << 36,
		 VS_Sampler3 = 1LL << 37,

		 GS_Sampler0 = 1LL << 38,
		 GS_Sampler1 = 1LL << 39,
		 GS_Sampler2 = 1LL << 40,
		 GS_Sampler3 = 1LL << 41,

		 PS_Sampler0 = 1LL << 42,
		 PS_Sampler1 = 1LL << 43,
		 PS_Sampler2 = 1LL << 44,
		 PS_Sampler3 = 1LL << 45,

		 RS_ViewPort = 1LL << 46,	// 16 viewports can be bound, but only one can be active
									// To perform a per-primitive selection of the viewport in the geometry shader,
									// specify the ViewportArrayIndex semantic on the appropriate GS output component in 
									// the GS output signature declaration.
		 RS_Scissor = 1LL << 47,
		 RS_RasterizerState = 1LL << 48,

		 OM_RenderTargetDepthStencil = 1LL << 49, // up yo 8 rts, but at the same time and always overwrite everyone already bound
		 OM_DepthStencilState = 1LL << 50,
		 OM_BlendState = 1LL << 51
	};

	class Binder: public Command{

	public:

		DBG(static uint32_t s_count;)

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Binder(E_BIND_MASK typeBits_p, E_BIND typeIndex_p)
			:
		m_typeBits(typeBits_p),
		m_typeIndex(typeIndex_p){

			assert( m_typeIndex >= 0 && m_typeIndex < E_MAX_BINDS );

			DBG(++s_count;)
		};
		Binder(){
		
			DBG(++s_count;)
		}
		virtual ~Binder(){};

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void Set(E_BIND_MASK typeBits_p, E_BIND typeIndex_p ){

			assert( typeIndex_p >= 0 && typeIndex_p < E_MAX_BINDS );

			m_typeIndex = typeIndex_p;
			m_typeBits = typeBits_p;
		}

		//------------------------------------------------------------------------
		// What this object binds?
		//------------------------------------------------------------------------
		E_BIND_MASK TypeBits() const { return m_typeBits;	}
		uint64_t TypeBits_int() const { return (uint64_t)m_typeBits; }
		E_BIND TypeIndex() const { return m_typeIndex;	}

	private:

		E_BIND_MASK m_typeBits;
		E_BIND m_typeIndex;
	};
}

//========================================================================
// the binder commands definitions
//========================================================================
#include "BinderBinds.h"
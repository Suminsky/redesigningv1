//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/02/15
	created:	15:2:2013   19:26
	filename: 	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render\DrawablesQueue.h
	file path:	C:\Users\Gateway\documents\visual studio 2010\Projects\redesigningv0\redesigningv0\namespace render
	file base:	DrawablesQueue
	file ext:	h
	author:		Icebone1000 (Giuliano SUminsky Pieta)
	
	purpose:	A object that can be draw to a render target, in a single draw call, that is.

	© Icebone1000 (Giuliano SUminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

#pragma once

// system/standard headers

//#include <boost/smart_ptr.hpp>

// private headers

//#include "namespace dx/type wrappers/BufferResource.h"

#include "dx/pipeline/Binder.h"
#include "dx/pipeline/DrawCall.h"

namespace render{

#pragma once

	class Drawable{

	public:

		dx::statevec m_PipeStatesGroup;					// binder commands
		std::shared_ptr<dx::DrawCall> m_pDrawCall;	// draw

		UINT64 m_sortKey; // viewport, distance, material..defined on a higer lvl

	public:

		//------------------------------------------------------------------------
		// ctor/dctor
		//------------------------------------------------------------------------
		Drawable(){};
		virtual ~Drawable(){};

		//------------------------------------------------------------------------
		// getters
		//------------------------------------------------------------------------
		UINT64 GetKey()const{
			return m_sortKey;
		}
		dx::statevec* GetPipeStateGroup(){

			return &m_PipeStatesGroup;
		}
		std::shared_ptr<dx::DrawCall> GetDrawCall() const{
			return m_pDrawCall;
		}

	};
}
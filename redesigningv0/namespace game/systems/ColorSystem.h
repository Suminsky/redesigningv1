#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2013/10/29
	created:	29:10:2013   14:58
	file:		ColorSystem.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../System.h"
#include "../components/ColorComponent.h"
#include "../Message.h"

namespace game{

	typedef std::vector<weak_ColorComponent_ptr>  ColorsReferences;
	typedef std::vector<int> indexes;
	typedef std::vector<Component*> Components;

	class ColorMessage: public Message{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		ColorMessage( MessageType type_p, DirectX::XMFLOAT4 color_p, DirectX::XMFLOAT4 prevColor_p )
			:
			Message(type_p){

			color = color_p;
			previousColor = prevColor_p;
		}

		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 previousColor;
	};


	class ColorSystem: public System{

	public:

	
	private:

		ColorsReferences m_vColors;
		indexes m_vLostReferences;

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void VOnUpdate( double /*dAccum_p*/, double /*dDelta_p*/ ){

			for( int itColor = 0, iSize = (int)m_vColors.size();
				 itColor < iSize;
				 ++itColor ){

				if( shared_ColorComponent_ptr pColor = m_vColors[itColor].lock() ){

					pColor->UpdateLocalColor();

					DirectX::XMFLOAT4 parentColor( 1.0f, 1.0f, 1.0f, 1.0f );
					
					pColor->UpdateWorldColor( parentColor );

					// send event to object
					// should send event straight to the components interested, that way theres no
					// need to search for registered components on the object, theyr registered here..
					// a system should have an array for each message it can generate, the array are
					// of registered components..but then every object will get it..fail
				}
				else{

					m_vLostReferences.push_back( itColor );
				}
			}

			CleanLostReferences();
		}

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		void CleanLostReferences(){

			unsigned int nRemoved =	(unsigned int)m_vLostReferences.size(); // cache
			unsigned int nComponents =		(unsigned int)m_vColors.size();

			for( unsigned int itRemoved = 0, itLast = nComponents - 1;
				itRemoved < nRemoved;
				++itRemoved, --itLast ){

				std::swap( m_vColors[m_vLostReferences[itRemoved]], m_vColors[itLast] );
			}

			// "trim"

			m_vColors.resize(nComponents - nRemoved);

			m_vLostReferences.clear();
		}
	};

	typedef std::shared_ptr<ColorSystem> shared_ColorSystem_ptr;
	typedef std::weak_ptr<ColorSystem> weak_ColorSystem_ptr;
}
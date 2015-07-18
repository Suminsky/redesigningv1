#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2015/07/05
	created:	5:7:2015   3:54
	file:		SpriteTextComposer.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes
#include "../../namespace gen/gen_data.h"
#include "FontCache.h"

namespace game{
	struct spriteInstance;
}

namespace sprite{

	class SpriteTextComposer{

	public:

		//------------------------------------------------------------------------
		// ctor
		//------------------------------------------------------------------------
		SpriteTextComposer( TextureBinders * pTexBinders_p )
			:
		m_fonts(pTexBinders_p)
		{}

		//------------------------------------------------------------------------
		// compose sprite instances based on font info
		//------------------------------------------------------------------------
		void Compose( wchar_t * szText_p, game::spriteInstance * pInstances_p, uint32_t maxChars_p, uint32_t fontID_p );
		void Compose( wchar_t * szText_p, game::spriteInstance * pInstances_p, uint32_t maxChars_p, uint32_t fontID_p,
						float & width_p, float & height_p );


		FontCache m_fonts;
	
	private:

		
	};
}
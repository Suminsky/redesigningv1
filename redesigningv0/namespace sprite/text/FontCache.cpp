#include "FontCache.h"

#include "../../namespace text/CreateFontTexture.h"

using namespace sprite;
using namespace text;


BmpFont_ & sprite::FontCache::Get( const char* szFont_p, uint32_t * pIndexID_p /*= nullptr */ )
{
	for( int it = 0, size = (int)m_names.Size();
		it < size;
		++it)
	{
		if( m_names[it] == szFont_p )
		{
			*pIndexID_p = it;
			return m_fonts[it];
		}
	}

	// cache miss:

	BmpFontDesc desc;
	ReadFontDescFromFile( szFont_p, desc);

	BmpFont_ newFont( desc, m_fontGlyphs.StackAlloc( desc.nCharacteres ) , *m_pTextureCacheRef);

	delete [] desc.characteresIDs;
	delete [] desc.charUVRects;
	delete [] desc.szTextureFilename;

	int index = Add( newFont, szFont_p );

	if( pIndexID_p )
		*pIndexID_p = index;

	return m_fonts[index];
}

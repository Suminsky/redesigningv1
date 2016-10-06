#include "PCHF.h"
#include "SpriteAnimCache.h"
#include "../../namespace text/TextParser.h"
#include "../../namespace sprite/binders cache/TextureBinders.h"

using namespace game;
using namespace text;
using namespace gen;
using namespace DirectX;

game::SpriteAnimCache::SpriteAnimCache(sprite::TextureBinders * pTex2DCache)
	:
m_pTex2DCacheRef(pTex2DCache)
{
	m_dataStream.m_currentByteIndex = 0;
	m_dataStream.m_data = m_data;
	m_dataStream.m_size = E_MAXMEMBYTES;
}

uint32_t game::SpriteAnimCache::AddNew( const char * szFileName )
{
	if( GetFileType(szFileName) == E_FILETYPE_GFIG ){
		return LoadDataFromGfig(szFileName);
	}
	else
		return LoadDataFromSanim(szFileName);
}

uint32_t game::SpriteAnimCache::LoadDataFromGfig( const char * szFileName )
{
	// parse gfig
	gen::DataStream buffer;
	buffer.m_size = 0;
	buffer.m_currentByteIndex = 0;
	if( win::ReadEntireFile( szFileName, buffer ) == false ){
		char errorMsg[2048] = "Error trying to read ";
		int nHeader = sizeof("Error trying to read ")-1;
		int szNameCount = gen::stringUtil::CountString(szFileName);

		memcpy(&errorMsg[nHeader], szFileName, szNameCount);
		errorMsg[nHeader + szNameCount] = ' ';
		errorMsg[nHeader + szNameCount + 1] = '.';
		errorMsg[nHeader + szNameCount + 2] = '\n';
		FormatMessage(	FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), NULL,
			&errorMsg[nHeader + szNameCount + 3], 2048 - nHeader + szNameCount + 3,
			NULL );
		MB("File Error", errorMsg, MB_E);
		_exit(1);
	}

	GfigElementA gfig;
	GfigElementA::ParseGfigFile( buffer, gfig );
	delete [] buffer.m_data;

	// add new entry to cache, point data to current DataStream offset
	Entry * pNewAnimData = m_cache.StackAlloc();
	pNewAnimData->pAnimData = m_dataStream.GetDataAtCurrent();
	memcpy( pNewAnimData->szFilename, szFileName, E_MAXFILENEMAESIZE );

	// start building data
	AnimDataHeader * pHeader = m_dataStream.GetNextChunkPtr<AnimDataHeader>(); //
	//pHeader->ppClipsPtrs = (AnimClipConfig**)m_dataStream.GetDataAtCurrent();

	// clips
	GfigElementA * pClips;
	keepAssert( gfig.GetSubElement( "clips", pClips ) );
	pHeader->nClips = (uint32_t)pClips->m_subElements.size();

	pHeader->ppClipsPtrs = m_dataStream.GetNextChunkPtr<AnimClipConfig*>(pHeader->nClips); //

	for( int clip = 0, n = pHeader->nClips; clip < n; ++clip ){

		pHeader->ppClipsPtrs[clip] =  (AnimClipConfig*)m_dataStream.GetDataAtCurrent();
		LoadClipDataFromGfig(&pClips->m_subElements[clip], &m_dataStream); //
	}

	// frames
	pHeader->pFrames = (SpriteFrame_*)m_dataStream.GetDataAtCurrent();

	GfigElementA * pFrames;
	keepAssert( gfig.GetSubElement( "frames", pFrames ) );
	pHeader->nFrames = (uint16_t)pFrames->m_subElements.size();
	
	for( int frame = 0, n = pHeader->nFrames; frame < n; ++frame ){

		LoadFrameDataFromGfig(&pFrames->m_subElements[frame], &m_dataStream); //
	}

	// sprites
	pHeader->pSprites = (TextureID_Binder_Pair_*)m_dataStream.GetDataAtCurrent();

	GfigElementA * pSrites;
	keepAssert( gfig.GetSubElement( "sprites", pSrites ) );
	pHeader->nSprites = (uint16_t)pSrites->m_subElements.size();
	//uint32_t spriteNamesLenght = 0;
	for( int sprite = 0, n = pHeader->nSprites; sprite < n; ++sprite ){

		LoadSpriteDataFromGfig(&pSrites->m_subElements[sprite], m_pTex2DCacheRef, &m_dataStream); //
		
		// info for writing to file
		//spriteNamesLenght += pSrites->m_subElements[sprite].m_name.length();
	}

	// serialize to bin file
	// 
	// for the sprites...needs to replace pointers to sprite filenames
	DataStream dataBuff;
	dataBuff.m_currentByteIndex = 0;
	uint32_t dataSize = (unsigned int)(m_dataStream.GetDataAtCurrent() - pNewAnimData->pAnimData);
	uint32_t copySize = dataSize - sizeof(TextureID_Binder_Pair_)*pHeader->nSprites;
	uint32_t fileSize = copySize + pHeader->nSprites * E_MAXFILENEMAESIZE;
	dataBuff.m_data = new unsigned char[fileSize];
	dataBuff.m_size = fileSize;
	dataBuff.Stack(pNewAnimData->pAnimData, copySize);
	for( int sprite = 0, n = pHeader->nSprites; sprite < n; ++sprite ){

		dataBuff.Stack((unsigned char*)pSrites->m_subElements[sprite].m_name.c_str(), E_MAXFILENEMAESIZE);
	}

	int nSz = gen::stringUtil::CountString(szFileName, E_MAXFILENEMAESIZE);
	char newSz[E_MAXFILENEMAESIZE];
	memcpy(newSz, szFileName, nSz);
	const char newExt[] = "anim";
	memcpy(&newSz[nSz] - 4, newExt, sizeof(newExt));
	win::SaveEntireFile( newSz, dataBuff);
	
	delete [] dataBuff.m_data;
	//

	return m_cache.Size()-1;
}

uint32_t game::SpriteAnimCache::LoadDataFromSanim( const char * szFileName )
{
	// parse gfig
	gen::DataStream buffer;
	buffer.m_size = 0;
	buffer.m_currentByteIndex = 0;
	if( win::ReadEntireFile( szFileName, buffer ) == false ){
		char errorMsg[2048] = "Error trying to read ";
		int nHeader = sizeof("Error trying to read ")-1;
		int szNameCount = gen::stringUtil::CountString(szFileName);

		memcpy(&errorMsg[nHeader], szFileName, szNameCount);
		errorMsg[nHeader + szNameCount] = ' ';
		errorMsg[nHeader + szNameCount + 1] = '.';
		errorMsg[nHeader + szNameCount + 2] = '\n';
		FormatMessage(	FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), NULL,
			&errorMsg[nHeader + szNameCount + 3], 2048 - nHeader + szNameCount + 3,
			NULL );
		MB("File Error", errorMsg, MB_E);
		_exit(1);
	}

	// add new entry to cache, point data to current DataStream offset
	Entry * pNewAnimData = m_cache.StackAlloc();
	pNewAnimData->pAnimData = m_dataStream.GetDataAtCurrent();
	memcpy( pNewAnimData->szFilename, szFileName, E_MAXFILENEMAESIZE );

	AnimDataHeader * pHeader = buffer.GetNextChunkPtr<AnimDataHeader>();

	// take address of clips offs, which is always after const header size,
	// subtract from where the clips offs will be on the mem
	uintptr_t addressDiff = (uintptr_t)&pNewAnimData->pAnimData[sizeof(AnimDataHeader)] - (uintptr_t)pHeader->ppClipsPtrs;

	// needs to account for the sprite data change
	uint32_t fileSize = buffer.m_size;
	uint32_t copySize = fileSize - pHeader->nSprites * E_MAXFILENEMAESIZE;
	//uint32_t dataSize = copySize + pHeader->nSprites * sizeof(TextureID_Binder_Pair_);

	// copy all loaded data
	m_dataStream.Stack( buffer.m_data, copySize);
	
	// patch pointers offsets
	
	DataStream dataBuff;
	dataBuff.m_data = pNewAnimData->pAnimData;
	dataBuff.m_currentByteIndex = 0;

	pHeader = dataBuff.GetNextChunkPtr<AnimDataHeader>();
	pHeader->ppClipsPtrs = (AnimClipConfig**)((uintptr_t)pHeader->ppClipsPtrs + addressDiff);
	pHeader->pFrames = (SpriteFrame_*)(uintptr_t(pHeader->pFrames) + addressDiff);
	pHeader->pSprites = (TextureID_Binder_Pair_ *)(uintptr_t(pHeader->pSprites) + addressDiff);

	for( int clip = 0, n = pHeader->nClips; clip < n; ++clip ){

		pHeader->ppClipsPtrs[clip] =  (AnimClipConfig*)((uintptr_t)pHeader->ppClipsPtrs[clip] + addressDiff);
		pHeader->ppClipsPtrs[clip]->pFrameSequence = (iframe*)((uintptr_t)pHeader->ppClipsPtrs[clip]->pFrameSequence + addressDiff);
	}

	// for the sprites...needs to replace pointers to sprite filenames
	keepAssert(m_dataStream.GetNextChunkPtr<TextureID_Binder_Pair_>(pHeader->nSprites) == pHeader->pSprites );
	buffer.m_currentByteIndex = copySize;
	for( int sprite = 0, n = pHeader->nSprites; sprite < n; ++sprite ){

		const char * szTex = buffer.GetNextChunkPtr<char>(E_MAXFILENEMAESIZE);
		pHeader->pSprites[sprite].pBindPSSRV = &m_pTex2DCacheRef->Get( szTex, &pHeader->pSprites[sprite].iID );
	}
	
	delete [] buffer.m_data;

	return m_cache.Size()-1;
}

void game::SpriteAnimCache::LoadClipDataFromGfig( text::GfigElementA * pClipGfig, gen::DataStream * pBuff )
{
	AnimClipConfig * pConfig = pBuff->GetNextChunkPtr<AnimClipConfig>();
	

	pConfig->eWrapMode = E_ANIMWRAPMODE__LOOP;
	pConfig->SPF = 0.1f;

	// wrap

	GfigElementA * pWrap;
	if( pClipGfig->GetSubElement("wrap",pWrap ) )
		pConfig->eWrapMode = GetWrapModeFromGFig( pWrap );		

	// spf

	GfigElementA * pSPF;
	if( pClipGfig->GetSubElement("spf", pSPF ) )
		pConfig->SPF = (animTimeUnit)atof( pSPF->m_value.c_str() );
		
	// frames

	GfigElementA * pClipSeq;
	keepAssert( pClipGfig->GetSubElement("frames", pClipSeq) );
	pConfig->pFrameSequence =  pBuff->GetNextChunkPtr<iframe>((int)pClipSeq->m_subElements.size());
	for( int it = 0, n = (int)pClipSeq->m_subElements.size(); it < n; ++it ){

		pConfig->pFrameSequence[it] = (iframe)atoi(pClipSeq->m_subElements[it].m_name.c_str());
	}

	// nframes

	pConfig->nFrames = (iframe)pClipSeq->m_subElements.size();
	
	// name

	memcpy( pConfig->szName, pClipGfig->m_name.c_str(), AnimClipConfig::E_MAXNAMELEN);
}

game::E_ANIMWRAPMODE_ game::SpriteAnimCache::GetWrapModeFromGFig( text::GfigElementA * pGFig_p )
{
	E_ANIMWRAPMODE_ wrapmodes[] = {
		E_ANIMWRAPMODE__LOOP,
		E_ANIMWRAPMODE__CLAMPLAST,
		E_ANIMWRAPMODE__CLAMFIRST,
		E_ANIMWRAPMODE__ONCE
	};
	const char* szAnimWrapModes[] = {
		"loop",
		"clamplast",
		"clampfirst",
		"once"
	};

	for( int it = 0; it < 4; ++it ){

		if( pGFig_p->m_value == szAnimWrapModes[it] ){

			return wrapmodes[it];
		}
	}

	return E_ANIMWRAPMODE__LOOP;
}

void game::SpriteAnimCache::LoadFrameDataFromGfig( text::GfigElementA * pFrameGfig, gen::DataStream * pBuff )
{
	SpriteFrame_ * pFrame = pBuff->GetNextChunkPtr<SpriteFrame_>();

	GfigElementA * pGFigParam;

	pFrame->iSpriteUsedIndex = 0;
	pFrame->fW = 100.0f;
	pFrame->fH = 100.0f;
	pFrame->xOffset = 0.0f;
	pFrame->yOffset = 0.0f;
	pFrame->uvRect = XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );

	if( pFrameGfig->GetSubElement( "sprite", pGFigParam ) )
		pFrame->iSpriteUsedIndex = atoi( pGFigParam->m_value.c_str() );
		
	//

	if( pFrameGfig->GetSubElement( "w", pGFigParam ) )
		pFrame->fW = (float)atof( pGFigParam->m_value.c_str() );		

	if( pFrameGfig->GetSubElement( "h", pGFigParam ) )
		pFrame->fH = (float)atof( pGFigParam->m_value.c_str() );
		
	//

	if( pFrameGfig->GetSubElement( "xoff", pGFigParam ) )
		pFrame->xOffset = (float)atof( pGFigParam->m_value.c_str() );	

	if( pFrameGfig->GetSubElement( "yoff", pGFigParam ) )
		pFrame->yOffset = (float)atof( pGFigParam->m_value.c_str() );

	//
	
	if( pFrameGfig->GetSubElement( "uvRect", pGFigParam ) )
		LoadXYWHFromGfig( pGFigParam, pFrame->uvRect );
}

void game::SpriteAnimCache::LoadXYWHFromGfig( text::GfigElementA * pXYWHGfig, DirectX::XMFLOAT4 & uv )
{
	GfigElementA * pElement;

	if( pXYWHGfig->GetSubElement( "x", pElement) ){
		uv.x = (float)atof(pElement->m_value.c_str());
	}
	if( pXYWHGfig->GetSubElement( "y", pElement) ){
		uv.y = (float)atof(pElement->m_value.c_str());
	}
	if( pXYWHGfig->GetSubElement( "w", pElement) ){
		uv.z = (float)atof(pElement->m_value.c_str());
	}
	if( pXYWHGfig->GetSubElement( "h", pElement) ){
		uv.w = (float)atof(pElement->m_value.c_str());
	}
}

void game::SpriteAnimCache::LoadSpriteDataFromGfig( text::GfigElementA * pSpriteGfig, sprite::TextureBinders *pTex2DCache, gen::DataStream * pBuff )
{
	TextureID_Binder_Pair_ * pSprite = pBuff->GetNextChunkPtr<TextureID_Binder_Pair_>();
	pSprite->pBindPSSRV = &pTex2DCache->Get( pSpriteGfig->m_name.c_str(), &pSprite->iID );
}
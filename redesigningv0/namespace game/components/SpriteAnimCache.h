#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2016/09/28
	created:	28:9:2016   18:38
	file:		SpriteAnimCache.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <memory>

// private includes

namespace dx{

	class BindPSShaderResourceView;
}
namespace sprite{

	class TextureBinders;
}
namespace text{
	struct GfigElementA;
}

namespace game{

	typedef float		animTimeUnit;
	typedef uint8_t		iframe;

	enum E_ANIMWRAPMODE_{
		E_ANIMWRAPMODE__LOOP = 0,
		E_ANIMWRAPMODE__CLAMPLAST,		// after finish, stop in the last frame
		E_ANIMWRAPMODE__CLAMFIRST,		// after finish, it resets to first frame
		E_ANIMWRAPMODE__ONCE,			// after finish, play the previous clip that was playing
	};
	enum E_ANIMSTATE_{
		E_ANIMSTATE__STOPPED = 0,
		E_ANIMSTATE__PLAYING,
		E_ANIMSTATE__PAUSED,
	};

	struct AnimClipConfig{

		enum{E_MAXNAMELEN = 31};					//size		//start offset

		E_ANIMWRAPMODE_		eWrapMode;				// 4		// 0
		animTimeUnit		SPF;					// 4		// 4
		iframe *			pFrameSequence;			// 8		// 8
		iframe				nFrames;				// 1		// 16
		char				szName[E_MAXNAMELEN];	// 31		// 17
																// 48	(multiple of 8)
	};

	struct SpriteFrame_{

		uint32_t iSpriteUsedIndex;
		float fW, fH;
		float xOffset, yOffset;
		DirectX::XMFLOAT4 uvRect;

		bool UVRectCompareDifferent ( const DirectX::XMFLOAT4 & a, const DirectX::XMFLOAT4 & b ) const {

			if( a.x != b.x 
				||
				a.y != b.y
				||
				a.z != b.z
				||
				a.w != b.w	)

				return true;

			return false;
		}
		bool operator == ( const SpriteFrame_ & other_p ) const{

			if( iSpriteUsedIndex != other_p.iSpriteUsedIndex 
				||
				fW != other_p.fW
				||
				fH != other_p.fH
				||
				xOffset != other_p.xOffset
				||
				yOffset != other_p.yOffset
				||
				UVRectCompareDifferent(uvRect, other_p.uvRect) )

				return false;

			return true;
		}
	};
	struct TextureID_Binder_Pair_{

		dx::BindPSShaderResourceView *	pBindPSSRV;	// 0
		uint32_t						iID;		// 8
		uint8_t							pad[4];		// 12
													// 16

		bool operator == ( const TextureID_Binder_Pair_ & other_p ) const{

			return (iID == other_p.iID);
		}
	};

	struct AnimDataHeader{

		AnimClipConfig **ppClipsPtrs;		// 0
		SpriteFrame_ * pFrames;				// 8
		TextureID_Binder_Pair_ * pSprites;	// 16
		uint32_t nClips;					// 24
		uint16_t nFrames;					// 28
		uint16_t nSprites;					// 30
											// 32
	};
	//========================================================================
	// 
	//========================================================================
	class SpriteAnimCache{

		enum{
			E_MAXFILENEMAESIZE = 2048,

			E_MAXANIMS = 12,
			E_MAXCLIPSPERANIM = 12,
			E_MAXFRAMESPERCLIP = 24,
			E_MAXFRAMESPERANIM = 32,
			E_MAXTEXTURESPERANIM = 2,

			E_MAXMEMBYTES =
				E_MAXANIMS
				*
				(
					E_MAXCLIPSPERANIM * (sizeof(AnimClipConfig) +  E_MAXFRAMESPERCLIP * sizeof(iframe))
					+
					E_MAXFRAMESPERANIM * sizeof(SpriteFrame_)
					+
					E_MAXTEXTURESPERANIM * sizeof(TextureID_Binder_Pair_)
				)
		};

	public:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		SpriteAnimCache(sprite::TextureBinders * pTex2DCache);

		//------------------------------------------------------------------------
		// consts
		//------------------------------------------------------------------------
		const uint8_t * GetAnimData( uint32_t ID ) const {

			assert( ID < m_cache.Size());
			return m_cache[ID].pAnimData;
		}

		const char * GetFilename( uint32_t index_p )const{

			assert(index_p < m_cache.Size());

			return m_cache[index_p].szFilename;
		}

		//------------------------------------------------------------------------
		// adds
		//------------------------------------------------------------------------
		const uint8_t * GetAnimData( const char * szFileName, uint32_t * outID ){

			for( int it = 0, n = m_cache.Size(); it < n; ++it){

				if( gen::stringUtil::IsEqual( szFileName, m_cache[it].szFilename) ){
					*outID = it;
					return m_cache[it].pAnimData;
				}
			}

			*outID = AddNew( szFileName );
			return m_cache[*outID].pAnimData;
		}


		enum E_FILETYPE{
			E_FILETYPE_GFIG,
			E_FILETYPE_SANIM
		};

		static E_FILETYPE GetFileType( const char * szFilename ){

			int count = gen::stringUtil::CountString( szFilename, E_MAXFILENEMAESIZE );

			if( szFilename[count-1] == 'g') return E_FILETYPE_GFIG;
			else return E_FILETYPE_SANIM;
		}

	private:

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		uint32_t AddNew( const char * szFileName );
		uint32_t LoadDataFromGfig( const char * szFileName );
		uint32_t LoadDataFromSanim( const char * szFileName );
		
		static void LoadClipDataFromGfig( text::GfigElementA * pClipGfig, gen::DataStream * pBuff);
		static E_ANIMWRAPMODE_ GetWrapModeFromGFig( text::GfigElementA * pGFig_p );
		static void LoadFrameDataFromGfig( text::GfigElementA * pFrameGfig, gen::DataStream * pBuff);
		static void LoadXYWHFromGfig( text::GfigElementA * pXYWHGfig, DirectX::XMFLOAT4 & uv);
		static void LoadSpriteDataFromGfig( text::GfigElementA * pXYWHGfig, sprite::TextureBinders *pTex2DCache, gen::DataStream * pBuff);

		sprite::TextureBinders * m_pTex2DCacheRef;

		struct Entry{
			uint8_t * pAnimData;
			char szFilename[E_MAXFILENEMAESIZE];
		};

		gen::Stack<Entry, E_MAXANIMS> m_cache;

		gen::DataStream	m_dataStream;
		uint8_t m_data[E_MAXMEMBYTES];
	};
}
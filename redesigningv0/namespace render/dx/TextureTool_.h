#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
created:	2017/04/08
file:		TextureTool.h
author:		Icebone1000 (Giuliano Suminsky Pieta)

purpose:

© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
namespace dx {

	class Device;
}

class TextureTool_{

public:

	//------------------------------------------------------------------------
	// loads a texture as staging resource
	//------------------------------------------------------------------------
	static HRESULT LoadTextureForEditingW(ID3D11Device * pDevice_p,
		ID3D11Texture2D *& pTex2D_p,
		D3DX11_IMAGE_LOAD_INFO & outLoadInfo_p, D3DX11_IMAGE_INFO & outImgInfo_p,
		const wchar_t * szInName_p);
	static HRESULT LoadTextureForEditing(ID3D11Device * pDevice_p,
		ID3D11Texture2D *& pTex2D_p,
		D3DX11_IMAGE_LOAD_INFO & outLoadInfo_p, D3DX11_IMAGE_INFO & outImgInfo_p,
		const char * szInName_p);
	//------------------------------------------------------------------------
	// creates empty texture
	//------------------------------------------------------------------------
	static HRESULT CreateTextureForEditing(ID3D11Device * pDevice_p,
		ID3D11Texture2D *& pTex2D_p,
		UINT w_p, UINT h_p);

	//------------------------------------------------------------------------
	// copy a texture, same description and usage
	//------------------------------------------------------------------------
	static HRESULT CloneTexture(dx::Device * pDevice_p, ID3D11Texture2D * pTex_p, ID3D11Texture2D *& pClone_p);

	//------------------------------------------------------------------------
	// convert full transparent color to full BLACK transparent, to avoid linear filter shit
	//------------------------------------------------------------------------ 
	static HRESULT ConvertTextureTransparentPixelsToGivenColor(
		ID3D11DeviceContext * pContext_p,
		ID3D11Texture2D * pTex_p, UINT w_p, UINT h_p,
		const unsigned char color4_p[],
		UINT pixelAlphaTolerance_p);

	static HRESULT ConvertTextureTransparentPixelsToGivenColor(
		dx::Device * pDevice_p,
		const unsigned char color4_p[],
		UINT pixelAlphaTolerance_p,
		const wchar_t * szInName_p, const wchar_t * szOutName_p);

	//------------------------------------------------------------------------
	// 
	//------------------------------------------------------------------------
	struct Rect {
		int x, y, w, h;

		// returns the last pixel, not the last after
		int Right() const { return x + (w - 1); }
		int Down() const { return y + (h - 1); }

		// negative means penetration
		int XDistance(const Rect & other_p) const {
			if (x < other_p.x)

				return (other_p.x - 1) - Right();
			else {

				return (x - 1) - other_p.Right();
			}
		}
		int YDistance(const Rect & other_p) const {
			if (y < other_p.y)

				return (other_p.y - 1) - Down();
			else {

				return (y - 1) - other_p.Down();
			}
		}
		bool Colliding(const Rect & other_p) {

			if (XDistance(other_p) > 0) return false;
			if (YDistance(other_p) > 0) return false;

			return true;
		}

		bool FitInside(const Rect & other_p)const {

			return other_p.w <= w && other_p.h <= h;
		}
		bool FitInside(const Rect & other_p, int & wGap_p, int & hGap_p) {

			wGap_p = w - other_p.w;
			hGap_p = h - other_p.h;

			return wGap_p >= 0 && hGap_p >= 0;
		}
	};
	struct Node {

		std::unique_ptr<Node> m_child[2];
		Rect m_rect;
		bool m_bEmpty;

		Node() { m_bEmpty = true; }

		bool IsLeaf() { return !m_child[0] && !m_child[1]; }
		bool Empty() { return m_bEmpty; }

		//------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------
		Node* Insert(Rect & newImgRect_p);
	};
	static int PackTextures(Rect rects_p[], int nRects_p, int w_p, int h_p);


	//------------------------------------------------------------------------
	// trims texture outside in
	//------------------------------------------------------------------------
	struct TexelRectFromOriginal {
		UINT left, right, up, down;
	};
	static HRESULT TrimTexture(
		dx::Device * pDevice_p,
		ID3D11Texture2D * pTexIn_p, UINT w_p, UINT h_p,
		UINT pixelGap_p, UINT pixelAlphaTolerance_p,
		ID3D11Texture2D *& pTexOut_p, UINT & newW_p, UINT & newH_p, TexelRectFromOriginal & offsets_p);

	static HRESULT TrimTexture(
		dx::Device * pDevice_p,
		DirectX::XMFLOAT2 & offsetFromOriginal_p,
		UINT pixelGap_p, UINT pixelAlphaTolerance_p,
		const wchar_t * szInName_p, std::wstring & szOutName_p);

private:

	struct Texture {
		std::wstring szFileName;
		ID3D11Texture2D * pI;
		D3DX11_IMAGE_INFO info;
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		float xOff, yOff;

		Texture() :pI(nullptr) {}
		~Texture() {
			if (pI)pI->Release();
		}
	};
};

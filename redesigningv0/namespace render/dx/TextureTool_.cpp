#include "PCHF.h"

#include "TextureTool_.h"
#include "Device.h"

#include "../../namespace win/OpenDialog.h"

using namespace std;
using namespace DirectX;

HRESULT TextureTool_::LoadTextureForEditingW(ID3D11Device * pDevice_p, ID3D11Texture2D *& pTex2D_p,
	D3DX11_IMAGE_LOAD_INFO & outLoadInfo_p, D3DX11_IMAGE_INFO & outImgInfo_p,
	const wchar_t * szInName_p)
{
	memset(&outLoadInfo_p, 0, sizeof(D3DX11_IMAGE_LOAD_INFO));
	outLoadInfo_p.BindFlags = 0;
	outLoadInfo_p.CpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	outLoadInfo_p.Usage = D3D11_USAGE_STAGING;
	outLoadInfo_p.MipLevels = 1;
	outLoadInfo_p.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	outLoadInfo_p.pSrcInfo = &outImgInfo_p;

	return D3DX11CreateTextureFromFileW(pDevice_p, szInName_p, &outLoadInfo_p, NULL, (ID3D11Resource**)&pTex2D_p, NULL);
}

HRESULT TextureTool_::LoadTextureForEditing(ID3D11Device * pDevice_p, ID3D11Texture2D *& pTex2D_p, D3DX11_IMAGE_LOAD_INFO & outLoadInfo_p, D3DX11_IMAGE_INFO & outImgInfo_p, const char * szInName_p)
{
	memset(&outLoadInfo_p, 0, sizeof(D3DX11_IMAGE_LOAD_INFO));
	outLoadInfo_p.BindFlags = 0;
	outLoadInfo_p.CpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	outLoadInfo_p.Usage = D3D11_USAGE_STAGING;
	outLoadInfo_p.MipLevels = 1;
	outLoadInfo_p.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	outLoadInfo_p.pSrcInfo = &outImgInfo_p;

	return D3DX11CreateTextureFromFileA(pDevice_p, szInName_p, &outLoadInfo_p, NULL, (ID3D11Resource**)&pTex2D_p, NULL);
}

HRESULT TextureTool_::CreateTextureForEditing(ID3D11Device * pDevice_p, ID3D11Texture2D *& pTex2D_p, UINT w_p, UINT h_p)
{
	D3D11_TEXTURE2D_DESC desc;

	memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	desc.Width = w_p;
	desc.Height = h_p;
	desc.SampleDesc.Count = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;

	UINT size = w_p*h_p;

	struct pixel {

		BYTE r, g, b, a;
	} *pData = new pixel[size];

	memset((void*)pData, 0, sizeof(pixel)*size);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = pData;
	data.SysMemPitch = w_p;
	data.SysMemSlicePitch = 0;

	HRESULT hr = pDevice_p->CreateTexture2D(&desc, &data, &pTex2D_p);

	delete data.pSysMem;

	return hr;
}

HRESULT TextureTool_::CloneTexture(dx::Device * pDevice_p, ID3D11Texture2D * pTex_p, ID3D11Texture2D *& pClone_p)
{
	// I was copying to a staging texture..why? you can just open it as staging...

	D3D11_TEXTURE2D_DESC desc;
	pTex_p->GetDesc(&desc);

	HRESULT hr = pDevice_p->GetDevice()->CreateTexture2D(&desc, NULL, &pClone_p);
	if (FAILED(hr)) return hr;

	pDevice_p->GetContext()->CopyResource(pClone_p, pTex_p);

	return hr;
}

//========================================================================
// 
//========================================================================

HRESULT TextureTool_::ConvertTextureTransparentPixelsToGivenColor(
	ID3D11DeviceContext * pContext_p,
	ID3D11Texture2D * pTex_p, UINT w_p, UINT h_p,
	const unsigned char color4_p[], UINT pixelAlphaTolerance_p)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr = pContext_p->Map(pTex_p, 0, D3D11_MAP_READ_WRITE, 0, &mapped);
	if (FAILED(hr)) return hr;

	//traverse
	struct pixel {

		BYTE r, g, b, a;
	} *pData = (pixel*)mapped.pData;

	UINT realDataWidth = mapped.RowPitch / sizeof(pixel);

	for (UINT y = 0; y < h_p; ++y) {
		for (UINT x = 0; x < w_p; ++x) {

			UINT pxIndex = gen::array2Dto1D::indexFromXY(x, y, realDataWidth);

			if (pData[pxIndex].a <= pixelAlphaTolerance_p) {

				pData[pxIndex].r = color4_p[0];
				pData[pxIndex].g = color4_p[1];
				pData[pxIndex].b = color4_p[2];
				pData[pxIndex].a = color4_p[3];
			}
		}
	}

	pContext_p->Unmap(pTex_p, 0);

	return hr;
}

HRESULT TextureTool_::ConvertTextureTransparentPixelsToGivenColor(dx::Device * pDevice_p, const unsigned char  color4_p[],
	UINT pixelAlphaTolerance_p,
	const wchar_t * szInName_p, const wchar_t * szOutName_p)
{
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	D3DX11_IMAGE_INFO imgInfo;
	ID3D11Texture2D *pTex2D;

	HRESULT hr = LoadTextureForEditingW(pDevice_p->GetDevice(), pTex2D, loadInfo, imgInfo, szInName_p);

	if (FAILED(hr)) return hr;

	hr = ConvertTextureTransparentPixelsToGivenColor(pDevice_p->GetContext(),
		pTex2D, imgInfo.Width, imgInfo.Height,
		color4_p, pixelAlphaTolerance_p);
	if (FAILED(hr)) return hr;

	hr = D3DX11SaveTextureToFileW(pDevice_p->GetContext(), pTex2D, imgInfo.ImageFileFormat, szOutName_p);

	pTex2D->Release();

	return hr;
}

HRESULT TextureTool_::TrimTexture(dx::Device * pDevice_p,
	ID3D11Texture2D * pTexIn_p, UINT w_p, UINT h_p,
	UINT pixelGap_p, UINT pixelAlphaTolerance_p,
	ID3D11Texture2D *& pTexOut_p, UINT & newW_p, UINT & newH_p, TexelRectFromOriginal & offsets_p)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr = pDevice_p->GetContext()->Map(pTexIn_p, 0, D3D11_MAP_READ_WRITE, 0, &mapped);
	if (FAILED(hr)) return hr;

	struct pixel {
		BYTE r, g, b, a;
	} *pData = (pixel*)mapped.pData;

	UINT realDataWidth = mapped.RowPitch / sizeof(pixel);

	// find left first pixel
	UINT left = 0;
	for (UINT x = 0; x < w_p; ++x) {
		for (UINT y = 0; y < h_p; ++y) {

			int index = gen::array2Dto1D::indexFromXY(x, y, realDataWidth);
			if (pData[index].a > pixelAlphaTolerance_p) {

				left = x;
				x = w_p; // to quit out for
				break;
			}
		}
	}

	// find right first pixel
	UINT right = 0;
	for (int x = (int)w_p - 1; x >= 0; --x) {
		for (UINT y = 0; y < h_p; ++y) {

			int index = gen::array2Dto1D::indexFromXY(x, y, realDataWidth);

			if (pData[index].a > pixelAlphaTolerance_p) {

				right = x;
				x = 0; // to quit out for
				break;
			}
		}
	}

	// find top first pixel
	UINT top = 0;
	for (UINT y = 0; y < h_p; ++y) {
		for (UINT x = 0; x < w_p; ++x) {

			int index = gen::array2Dto1D::indexFromXY(x, y, realDataWidth);
			if (pData[index].a > pixelAlphaTolerance_p) {

				top = y;
				y = h_p;
				break;
			}
		}
	}

	// find top first pixel
	UINT bottom = 0;
	for (int y = (int)h_p - 1; y >= 0; --y) {
		for (UINT x = 0; x < w_p; ++x) {

			int index = gen::array2Dto1D::indexFromXY(x, y, realDataWidth);
			if (pData[index].a > pixelAlphaTolerance_p) {

				bottom = y;
				y = 0;
				break;
			}
		}
	}

	pDevice_p->GetContext()->Unmap(pTexIn_p, 0);

	//
	offsets_p.left = left;
	offsets_p.right = w_p - 1 - right;
	offsets_p.up = top;
	offsets_p.down = h_p - 1 - bottom;
	//

	UINT amountOnRight = w_p - 1 - right; // w = 3 ; rit = 2, 1, 0 ; rit = 2 should result in 0
	UINT amountOnBottom = h_p - 1 - bottom;

	newW_p = w_p - left - amountOnRight + pixelGap_p + pixelGap_p;
	newH_p = h_p - top - amountOnBottom + pixelGap_p + pixelGap_p;

	hr = CreateTextureForEditing(pDevice_p->GetDevice(), pTexOut_p, newW_p, newH_p);
	if (FAILED(hr)) return hr;

	D3D11_BOX srcBox;
	srcBox.left = left;
	srcBox.right = right + 1;
	srcBox.top = top;
	srcBox.bottom = bottom + 1;
	srcBox.back = 1;
	srcBox.front = 0;

	// "The values for right, bottom, and back are each one pixel past the end
	// of the pixels that are included in the box region. That is, the values
	// for left, top, and front are included in the box region while the values
	// for right, bottom, and back are excluded from the box region. For example,
	// for a box that is one pixel wide, (right - left) == 1; the box region
	// includes the left pixel but not the right pixel."

	pDevice_p->GetContext()->CopySubresourceRegion(pTexOut_p, 0, pixelGap_p, pixelGap_p, 0, pTexIn_p, 0, &srcBox);

	return hr;
}


const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
	{ L"PNG",       L"*.png" }
};

HRESULT TextureTool_::TrimTexture(dx::Device * pDevice_p, DirectX::XMFLOAT2 & offsetFromOriginal_p,
	UINT pixelGap_p, UINT pixelAlphaTolerance_p,
	const wchar_t * szInName_p, wstring & szOutName_p)
{
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	D3DX11_IMAGE_INFO imgInfo;
	ID3D11Texture2D *pTex2D;

	HRESULT hr = LoadTextureForEditingW(pDevice_p->GetDevice(), pTex2D, loadInfo, imgInfo, szInName_p);

	if (FAILED(hr)) return hr;

	ID3D11Texture2D * pTrimmedTex; UINT w, h; TexelRectFromOriginal offs;
	hr = TrimTexture(pDevice_p, pTex2D, imgInfo.Width, imgInfo.Height, pixelGap_p, pixelAlphaTolerance_p, pTrimmedTex, w, h, offs);

	if (FAILED(hr)) return hr;

	pTex2D->Release();

	wstring filePathName; BOOL bOpt = true;
	hr = win::SaveDialog(filePathName, c_rgSaveTypes, 1, bOpt);

	if (FAILED(hr)) return hr;

	hr = D3DX11SaveTextureToFileW(pDevice_p->GetContext(), pTrimmedTex, imgInfo.ImageFileFormat, filePathName.c_str()/*szOutName_p*/);

	pTrimmedTex->Release();

	//------------------------------------------------------------------------
	// compute offset
	//------------------------------------------------------------------------

	// center of rect on original texture space
	float rectCenterX = float(offs.right - offs.left)*0.5f + offs.left;
	float rectCenterY = float(offs.down - offs.up)*0.5f + offs.up;

	float origCenterX = float(imgInfo.Width)*0.5f;
	float origCenterY = float(imgInfo.Height)*0.5f;

	offsetFromOriginal_p.x = origCenterX - rectCenterX;
	offsetFromOriginal_p.y = origCenterY - rectCenterY;

	szOutName_p = filePathName;

	return hr;
}

int TextureTool_::PackTextures(Rect rects_[], int nRects_p, int w_p, int h_p)
{

	// NOTE:
	// for some really fucked up reason, sorting by area(dimension) results in
	// smaller texture (58 < 64 ~ KB )
	// 

	struct sortEntry {

		int dimensionSize;
		int index;
	};

	sortEntry * vSorted; vSorted = new sortEntry[nRects_p];

	// initialize sort arrays

	for (int it = 0; it < nRects_p; ++it) {

		vSorted[it].index = it;
		vSorted[it].dimensionSize = rects_[it].w * rects_[it].h;
	}

	// sort arrays

	std::sort(&vSorted[0], vSorted + nRects_p, [](const sortEntry & a, const sortEntry & b) { return a.dimensionSize > b.dimensionSize; });

	Node root;
	root.m_bEmpty = true;
	root.m_rect.x = root.m_rect.y = 0;
	root.m_rect.w = w_p; root.m_rect.h = h_p;

	int nUsed = nRects_p;
	for (int r = 0; r < nRects_p; ++r) {

		Node * pNode = root.Insert(rects_[vSorted[r].index]);
		if (pNode) {

			rects_[vSorted[r].index] = pNode->m_rect;
			pNode->m_bEmpty = false;
		}
		else {
			rects_[vSorted[r].index].x = -1;
			--nUsed;
		}
	}

	delete[] vSorted;

	return nUsed;
}

TextureTool_::Node* TextureTool_::Node::Insert(Rect & newImgRect_p)
{
	if (!IsLeaf()) {

		// not leaf, try insert on first then second child

		Node * newNode = m_child[0]->Insert(newImgRect_p);

		if (newNode)	return newNode; // inserted!
		else 			return m_child[1]->Insert(newImgRect_p); // no room on child 0, try 1 (may insert or not)
	}
	else {

		if (!Empty()) return nullptr;

		int wGap, hGap;
		if (!m_rect.FitInside(newImgRect_p, wGap, hGap)) return nullptr;

		if (wGap == 0 && hGap == 0) return this; // inserted!

												 //split node in 2

		m_child[0].reset(new Node);
		m_child[1].reset(new Node);


		/*if dw > dh then
		child[0]->rect = Rectangle(	rc.left,
		rc.top,
		rc.left+width-1,
		rc.bottom)

		child[1]->rect = Rectangle(	rc.left+width,
		rc.top,
		rc.right,
		rc.bottom)
		else
		child[0]->rect = Rectangle(	rc.left,
		rc.top,
		rc.right,
		rc.top+height-1)

		child[1]->rect = Rectangle(	rc.left,
		rc.top+height,
		rc.right,
		rc.bottom)*/

		if (wGap > hGap) {

			m_child[0]->m_rect.x = m_rect.x;
			m_child[0]->m_rect.y = m_rect.y;
			m_child[0]->m_rect.w = newImgRect_p.w;
			m_child[0]->m_rect.h = m_rect.h;

			m_child[1]->m_rect.x = m_rect.x + newImgRect_p.w;
			m_child[1]->m_rect.y = m_rect.y;
			m_child[1]->m_rect.w = m_rect.w - newImgRect_p.w;
			m_child[1]->m_rect.h = m_rect.h;
		}
		else {

			m_child[0]->m_rect.x = m_rect.x;
			m_child[0]->m_rect.y = m_rect.y;
			m_child[0]->m_rect.w = m_rect.w;
			m_child[0]->m_rect.h = newImgRect_p.h;

			m_child[1]->m_rect.x = m_rect.x;
			m_child[1]->m_rect.y = m_rect.y + newImgRect_p.h;
			m_child[1]->m_rect.w = m_rect.w;
			m_child[1]->m_rect.h = m_rect.h - newImgRect_p.h;
		}

		return m_child[0]->Insert(newImgRect_p); // inserted!
	}
}
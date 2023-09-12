#include "DX11Caps.h"
#include "DX11Device.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11Caps, GraphicsCaps);
void DX11Caps::Initialize()
{
	D3D_FEATURE_LEVEL d3dlevel = GetDX11Device()->GetDevice()->GetFeatureLevel();
	D3DFeatureLevel level = D3DFeatureLevel_9_1;
	switch (d3dlevel) {
	case D3D_FEATURE_LEVEL_9_1: level = D3DFeatureLevel_9_1; break;
	case D3D_FEATURE_LEVEL_9_2: level = D3DFeatureLevel_9_2; break;
	case D3D_FEATURE_LEVEL_9_3: level = D3DFeatureLevel_9_3; break;
	case D3D_FEATURE_LEVEL_10_0: level = D3DFeatureLevel_10_0; break;
	case D3D_FEATURE_LEVEL_10_1: level = D3DFeatureLevel_10_1; break;
	case D3D_FEATURE_LEVEL_11_0: level = D3DFeatureLevel_11_0; break;
	case D3D_FEATURE_LEVEL_11_1: level = D3DFeatureLevel_11_1; break;
	}

	maxVSyncInterval = 4;
	maxLights = 8;

	// 纹理大小
	static const int kTextureSizes[D3DFeatureLevelCount] = { 2048, 2048, 4096, 8192, 8192, 16384, 16384 };
	static const int kCubemapSizes[D3DFeatureLevelCount] = { 512,  512, 4096, 8192, 8192, 16384, 16384 };
	maxTextureSize = kTextureSizes[level];
	maxCubeMapSize = kCubemapSizes[level];
	maxRenderTextureSize = maxTextureSize;
	static const int kMRTCount[D3DFeatureLevelCount] = { 1, 1, 4, 8, 8, 8, 8 };
	maxRenderTargets = Math::Min<int>(kMRTCount[level], kMaxSupportedRenderTargets);

	maxTexUnits = kMaxSupportedTextureUnits;
	maxTexCoords = 8;

	static const int kMaxVertexCount[D3DFeatureLevelCount] = { 65534, 1048575, 1048575, 1048575, 1048575, 1048575, 1048575 };
	maxPrimitiveCount = kMaxVertexCount[level];
	maxVertexCount = kMaxVertexCount[level];
	maxIndexCount = kMaxVertexCount[level];

	hasAnisoFilter = true;
	static const int kMaxAniso[D3DFeatureLevelCount] = { 2, 16, 16, 16, 16, 16, 16 };
	maxAnisoLevel = kMaxAniso[level];

	hasAutoMipMapGen = true;
	hasMipLevelBias = true;

	has3DTexture = true;
	hasMipTexture = true;
	hasCubeTexture = true;
	hasRenderToTexture = true;
	hasRenderToCubemap = (level >= D3D_FEATURE_LEVEL_10_0);
	hasRenderTo3D = (level >= D3D_FEATURE_LEVEL_11_0);

	hasDXT1Texture = true;
	hasDXT3Texture = true;
	hasDXT5Texture = true;

	hasMultiSample = true;
	hasSRGBReadWrite = true;

	hasGeometryShader = (level >= D3D_FEATURE_LEVEL_10_0);
	hasHullShader = (level >= D3D_FEATURE_LEVEL_11_0);
	hasDomainShader = (level >= D3D_FEATURE_LEVEL_11_0);
	hasComputeShader = (level >= D3D_FEATURE_LEVEL_11_0);
	hasInstancing = (level >= D3D_FEATURE_LEVEL_9_3);

	hasStencil = true;

//#if !defined(DC_DEBUG)
//	useProgramBinary = true;
//#endif
}
bool DX11Caps::CheckTextureFormat(uint usage, ColorFormat format)
{
	UINT support = 0;
	GetDX11Device()->GetDevice()->CheckFormatSupport(DX11GetTextureFormat(format, false), &support);
	if (support & D3D11_FORMAT_SUPPORT_TEXTURE2D)
	{
		GetDX11Device()->GetDevice()->CheckFormatSupport(DX11GetTextureViewFormat(format, false), &support);
		if (support & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE)
		{
			return true;
		}
	}
	return false;
}
bool DX11Caps::CheckCubeTextureFormat(uint usage, ColorFormat format)
{
	UINT support = 0;
	GetDX11Device()->GetDevice()->CheckFormatSupport(DX11GetTextureFormat(format, false), &support);
	if (support & D3D11_FORMAT_SUPPORT_TEXTURECUBE)
	{
		GetDX11Device()->GetDevice()->CheckFormatSupport(DX11GetTextureViewFormat(format, false), &support);
		if (support & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE)
		{
			return true;
		}
	}
	return false;
}
bool DX11Caps::IsGenerateMipsSupported(TextureType type, ColorFormat format)
{
	UINT fmtSupport = 0;
	HRESULT hr = GetDX11Device()->GetDevice()->CheckFormatSupport(DX11GetTextureFormat(format, false), &fmtSupport);
	if (SUCCEEDED(hr) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
	{
		return true;
	}
	return false;
}
MSAALevel DX11Caps::GetMaximumMSAAQualityLevel(DXGI_FORMAT format)
{
	int maxCount = 1;
	UINT numQuality = 0;
	for (int i = 2; i <= 8; i *= 2)
	{
		if (SUCCEEDED(GetDX11Device()->GetDevice()->CheckMultisampleQualityLevels(format, i, &numQuality)) && numQuality > 0)
			maxCount = i;
	}
	MSAALevel msaa = { maxCount, numQuality };
	return msaa;
}
MSAALevel DX11Caps::GetMSAAQualityLevel(int antiAlias, DXGI_FORMAT format)
{
	int maxCount = 1;
	UINT numQuality = 0;
	for (int i = antiAlias; i > 0; i -= 2)
	{
		if (SUCCEEDED(GetDX11Device()->GetDevice()->CheckMultisampleQualityLevels(format, i, &numQuality)) && numQuality > 0)
		{
			maxCount = i;
			break;
		}
	}
	MSAALevel msaa = { maxCount, numQuality };
	return msaa;
}
DC_END_NAMESPACE
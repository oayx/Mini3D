#include "DX11Caps.h"
#include "DX11Device.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11Caps, GraphicsCaps);
void DX11Caps::Initialize()
{
	D3D_FEATURE_LEVEL d3dlevel = GetDX11Device()->GetDevice()->GetFeatureLevel();
	maxVSyncInterval = 4;
	maxLights = 8;

	// 纹理大小
	maxTextureSize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
	maxCubeMapSize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
	maxRenderTextureSize = maxTextureSize;
	maxRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

	maxTexUnits = kMaxSupportedTextureUnits;
	maxTexCoords = 8;

	maxVertexCount = 1048575;
	maxIndexCount = 1048575;
	maxPrimitiveCount = maxIndexCount / 3;

	hasAnisoFilter = true;
	maxAnisoLevel = D3D11_REQ_MAXANISOTROPY;

	hasAutoMipMapGen = true;
	hasMipLevelBias = true;

	has3DTexture = true;
	hasMipTexture = true;
	hasCubeTexture = true;
	hasRenderToTexture = true;
	hasRenderToCubemap = (d3dlevel >= D3D_FEATURE_LEVEL_10_0);
	hasRenderTo3D = (d3dlevel >= D3D_FEATURE_LEVEL_11_0);

	hasDXT1Texture = true;
	hasDXT3Texture = true;
	hasDXT5Texture = true;

	hasMultiSample = true;
	hasSRGBReadWrite = true;

	hasGeometryShader = (d3dlevel >= D3D_FEATURE_LEVEL_10_0);
	hasHullShader = (d3dlevel >= D3D_FEATURE_LEVEL_11_0);
	hasDomainShader = (d3dlevel >= D3D_FEATURE_LEVEL_11_0);
	hasComputeShader = (d3dlevel >= D3D_FEATURE_LEVEL_11_0);
	hasInstancing = (d3dlevel >= D3D_FEATURE_LEVEL_9_3);

	hasStencil = true;

#if !defined(DC_DEBUG)
	useProgramBinary = true;
#endif
}
bool DX11Caps::CheckTextureFormat(uint usage, ColorFormat format)
{
	UINT support = 0;
	GetDX11Device()->GetDevice()->CheckFormatSupport(DX10GetTextureFormat(format, false), &support);
	if (support & D3D11_FORMAT_SUPPORT_TEXTURE2D)
	{
		GetDX11Device()->GetDevice()->CheckFormatSupport(DX10GetTextureViewFormat(format, false), &support);
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
	GetDX11Device()->GetDevice()->CheckFormatSupport(DX10GetTextureFormat(format, false), &support);
	if (support & D3D11_FORMAT_SUPPORT_TEXTURECUBE)
	{
		GetDX11Device()->GetDevice()->CheckFormatSupport(DX10GetTextureViewFormat(format, false), &support);
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
	HRESULT hr = GetDX11Device()->GetDevice()->CheckFormatSupport(DX10GetTextureFormat(format, false), &fmtSupport);
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
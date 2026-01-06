#include "DX12Caps.h"
#include "DX12Device.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12Caps, GraphicsCaps);
void DX12Caps::Initialize()
{
	D3D_FEATURE_LEVEL d3dlevel = GetDX12Device()->GetFeatureLevel();
	maxVSyncInterval = 4;
	maxLights = 8;

	// 纹理大小
	maxTextureSize = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
	maxCubeMapSize = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
	maxRenderTextureSize = maxTextureSize;
	maxRenderTargets = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;

	maxTexUnits = kMaxSupportedTextureUnits;
	maxTexCoords = 8;

	maxVertexCount = 1048575;
	maxIndexCount = 1048575;
	maxPrimitiveCount = maxIndexCount / 3;

	hasAnisoFilter = true;
	maxAnisoLevel = D3D12_REQ_MAXANISOTROPY;

	hasAutoMipMapGen = true;
	hasMipLevelBias = true;

	has3DTexture = true;
	hasMipTexture = true;
	hasCubeTexture = true;
	hasRenderToTexture = true;
	hasRenderToCubemap = true;
	hasRenderTo3D = true;
	hasStencil = true;

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

#if !defined(DC_DEBUG)
	useProgramBinary = true;
#endif
}
MSAALevel DX12Caps::GetMaximumMSAAQualityLevel(DXGI_FORMAT format)
{
	int maxCount = 1;
	UINT numQuality = 0;
	for (int i = 2; i <= 8; i *= 2)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = format;
		msQualityLevels.SampleCount = i;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		if (SUCCEEDED(GetDX12Device()->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels))))
		{
			maxCount = i;
			numQuality = msQualityLevels.NumQualityLevels;
		}
	}
	MSAALevel msaa = { maxCount, numQuality };
	return msaa;
}
MSAALevel DX12Caps::GetMSAAQualityLevel(int antiAlias, DXGI_FORMAT format)
{
	int maxCount = 1;
	UINT numQuality = 0;
	for (int i = antiAlias; i > 0; i -= 2)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = format;
		msQualityLevels.SampleCount = i;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		if (SUCCEEDED(GetDX12Device()->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels))))
		{
			maxCount = i;
			numQuality = msQualityLevels.NumQualityLevels;
			break;
		}
	}
	MSAALevel msaa = { maxCount, numQuality };
	return msaa;
}
DC_END_NAMESPACE


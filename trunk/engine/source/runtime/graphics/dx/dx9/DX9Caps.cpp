#include "DX9Caps.h"
#include "DX9Device.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9Caps, GraphicsCaps);
void DX9Caps::Initialize()
{
	DX9HR(GetDX9Device()->GetDirect3D()->GetDeviceCaps(GetDX9Device()->GetAdapterIndex(), D3DDEVTYPE_HAL, &_caps));

	Debuger::Log("RenderTarget:%d", _caps.NumSimultaneousRTs);
	Debuger::Log("MaxTextureWidth:%d", _caps.MaxTextureWidth);
	Debuger::Log("MaxTextureHeight:%d", _caps.MaxTextureHeight);
	Debuger::Log("MaxVolumeExtent:%d", _caps.MaxVolumeExtent);
	Debuger::Log("MaxTextureRepeat:%d", _caps.MaxTextureRepeat);
	Debuger::Log("MaxAnisotropy:%d", _caps.MaxAnisotropy);
	Debuger::Log("MaxPrimitiveCount:%d", _caps.MaxPrimitiveCount);
	Debuger::Log("MaxPointSize:%f", _caps.MaxPointSize);
	Debuger::Log("MaxVertexIndex:%d", _caps.MaxVertexIndex);
	Debuger::Log("MaxVertexBlendMatrices:%d", _caps.MaxVertexBlendMatrices);
	Debuger::Log("MaxVertexBlendMatrixIndex:%d", _caps.MaxVertexBlendMatrixIndex);
	Debuger::Log("MaxVertexShaderConst:%d", _caps.MaxVertexShaderConst);

	maxVSyncInterval = 0;
	if (_caps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE)
	{
		maxVSyncInterval = 1;
		if (_caps.PresentationIntervals & D3DPRESENT_INTERVAL_TWO)
			maxVSyncInterval = 2;
	}

	bool usesSoftwareVP = !(_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT);
	if (usesSoftwareVP)
		maxLights = 8; // software T&L always has 8 lights
	else
		maxLights = Math::Clamp<uint>(_caps.MaxActiveLights, 0, 8);

	maxTextureSize = Math::Min<int>(_caps.MaxTextureWidth, _caps.MaxTextureHeight);
	maxCubeMapSize = maxTextureSize;
	maxRenderTextureSize = maxTextureSize;
	maxRenderTargets = Math::Clamp<int>(_caps.NumSimultaneousRTs, 1, kMaxSupportedRenderTargets);
	if (!(_caps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING))
		maxRenderTargets = 1;

	maxTexUnits = Math::Min<int>(_caps.MaxSimultaneousTextures, kMaxSupportedTextureUnits);
	maxTexCoords = Math::Min<int>(_caps.MaxSimultaneousTextures, kMaxSupportedTextureCoords);
	maxTextureRepeat = _caps.MaxTextureRepeat;

	maxPrimitiveCount = _caps.MaxPrimitiveCount;
	maxVertexCount = _caps.MaxPrimitiveCount;
	maxIndexCount = _caps.MaxVertexIndex;

	maxPointSize = _caps.MaxPointSize;

	hasAnisoFilter = _caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY;
	maxAnisoLevel = hasAnisoFilter ? _caps.MaxAnisotropy : 1;

	hasAutoMipMapGen = _caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP;

	has3DTexture = _caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP;
	hasMipTexture = _caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP;
	hasCubeTexture = _caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP;
	hasCubeMipTexture = _caps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP;
	hasRenderToTexture = CheckRenderTextureFormat(ColorFormat::R8G8B8A8);
	hasRenderToCubemap = hasRenderToTexture;
	hasRenderTo3D = has3DTexture && hasRenderToTexture;
	
	hasDXT1Texture = true;
	hasDXT3Texture = true;
	hasDXT5Texture = true;

	hasStencil = true;
}
bool DX9Caps::CheckTextureFormat(uint usage, ColorFormat format)
{
	uint adapter = GetDX9Device()->GetAdapterIndex();
	D3DDEVTYPE deviceType = GetDX9Device()->GetDevType();
	D3DFORMAT adapter_format = GetDX9Device()->GetDisplayMode().Format;
	HRESULT hr = GetDX9Device()->GetDirect3D()->CheckDeviceFormat(adapter, deviceType, adapter_format, usage, D3DRTYPE_TEXTURE, DX9GetTextureFormat(format));
	return SUCCEEDED(hr);
}
bool DX9Caps::CheckCubeTextureFormat(uint usage, ColorFormat format)
{
	uint adapter = GetDX9Device()->GetAdapterIndex();
	D3DDEVTYPE deviceType = GetDX9Device()->GetDevType();
	D3DFORMAT adapter_format = GetDX9Device()->GetDisplayMode().Format;
	HRESULT hr = GetDX9Device()->GetDirect3D()->CheckDeviceFormat(adapter, deviceType, adapter_format, usage, D3DRTYPE_CUBETEXTURE, DX9GetTextureFormat(format));
	return SUCCEEDED(hr);
}
bool DX9Caps::CheckRenderTextureFormat(ColorFormat format)
{
	uint adapter = GetDX9Device()->GetAdapterIndex();
	D3DDEVTYPE deviceType = GetDX9Device()->GetDevType();
	D3DFORMAT adapter_format = GetDX9Device()->GetDisplayMode().Format;
	HRESULT hr = GetDX9Device()->GetDirect3D()->CheckDeviceFormat(adapter, deviceType, adapter_format, D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, DX9GetTextureFormat(format));
	return SUCCEEDED(hr);
}
bool DX9Caps::CheckSRGBTextureReadFormat(ColorFormat format)
{
	uint adapter = GetDX9Device()->GetAdapterIndex();
	D3DDEVTYPE deviceType = GetDX9Device()->GetDevType();
	D3DFORMAT adapter_format = GetDX9Device()->GetDisplayMode().Format;
	HRESULT hr = GetDX9Device()->GetDirect3D()->CheckDeviceFormat(adapter, deviceType, adapter_format, D3DUSAGE_QUERY_SRGBREAD, D3DRTYPE_TEXTURE, DX9GetTextureFormat(format));
	return SUCCEEDED(hr);
}
bool DX9Caps::CheckSRGBTextureWriteFormat(ColorFormat format)
{
	uint adapter = GetDX9Device()->GetAdapterIndex();
	D3DDEVTYPE deviceType = GetDX9Device()->GetDevType();
	D3DFORMAT adapter_format = GetDX9Device()->GetDisplayMode().Format;
	HRESULT hr = GetDX9Device()->GetDirect3D()->CheckDeviceFormat(adapter, deviceType, adapter_format, D3DUSAGE_QUERY_SRGBWRITE, D3DRTYPE_TEXTURE, DX9GetTextureFormat(format));
	return SUCCEEDED(hr);
}
MSAALevel DX9Caps::GetMSAAQualityLevel(int antiAlias, D3DFORMAT backBufferFormat, bool windowed)
{
	uint adapter = GetDX9Device()->GetAdapterIndex();
	D3DDEVTYPE deviceType = GetDX9Device()->GetDevType();

	MSAALevel msaa;
	D3DMULTISAMPLE_TYPE multiSampleType = D3DMULTISAMPLE_NONE;
	DWORD multiSampleQuality = 0;
	while (antiAlias > 0)
	{
		multiSampleType = (D3DMULTISAMPLE_TYPE)antiAlias;
		HRESULT hr = GetDX9Device()->GetDirect3D()->CheckDeviceMultiSampleType(adapter, deviceType, backBufferFormat, windowed, multiSampleType, &multiSampleQuality);
		if (SUCCEEDED(hr))
		{
			multiSampleQuality = 0;//TODO:非0值CreateDevice会失败
			break;
		}
		if (antiAlias <= 1)
		{
			multiSampleType = D3DMULTISAMPLE_NONE;
			break;
		}
		antiAlias--;
	}
	msaa.Count = (int)multiSampleType;
	msaa.Quality = (int)multiSampleQuality;
	return msaa;
}
DC_END_NAMESPACE
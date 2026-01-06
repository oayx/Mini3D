#include "DX11Device.h"
#include "DX11Caps.h"
#include "DX11HardwareVertexBuffer.h"
#include "DX11HardwareIndexBuffer.h"
#include "DX11PostProcess.h"
#include "DX11RenderContent.h"
#include "DX11Texture.h"
#include "DX11RenderTexture.h"
#include "DX11Program.h"
#include "../DXGPUAdapter.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11Device, GraphicsDevice);
DX11Device* GetDX11Device()
{
	return (DX11Device*)Application::GetGraphics();
}
DX11Device::DX11Device()
{
	_gpuAdapter = DXGPUAdapter::Create();
	_graphicsCaps = DX11Caps::Create();
}
DX11Device::~DX11Device()
{
	SAFE_RELEASE(_dXGIFactory);
	SAFE_RELEASE(_immediateContext);
	//TODO:这里会卡死
	SAFE_RELEASE(_d3Device);
}
bool DX11Device::CreateDevice(RenderWindow* window)
{
	DX_ERROR(CreateDXGIFactory(IID_PPV_ARGS(&_dXGIFactory)));

	//尝试获得显卡
	IDXGIAdapter* selected_adapter = nullptr;
	GPUAdapterInfo* adapter = _gpuAdapter->SelectAdapters(_dXGIFactory);
	if (adapter != nullptr)
	{
		if (_dXGIFactory->EnumAdapters(adapter->GetIndex(), &selected_adapter) == DXGI_ERROR_NOT_FOUND || selected_adapter == nullptr)
		{
			Debuger::Warning("Cannot get the adapter:%s", adapter->GetDescription().c_str());
			selected_adapter = nullptr;
		}
		else
		{
			adapter->SetSelect();
		}
	}

	UINT creation_flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if DC_DEBUG
	//creation_flags |= D3D11_CREATE_DEVICE_DEBUG;//需要安装Graphics Tools
#endif
	D3D_FEATURE_LEVEL feature_evel;
	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	HRESULT result = S_FALSE;
	D3D_DRIVER_TYPE driverType = selected_adapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE;
	result = D3D11CreateDevice(
		selected_adapter,			// 显示适配器, 指定 nullptr 以使用默认适配器。
		driverType,					// 创建使用硬件图形驱动程序的设备。
		NULL,						// 应为 0，除非驱动程序是 D3D_DRIVER_TYPE_SOFTWARE。
		creation_flags,				// 设置调试和 Direct2D 兼容性标志。
		feature_levels,				// 此应用程序可以支持的功能级别的列表。
		ARRAY_SIZE(feature_levels),	// 上面的列表的大小。
		D3D11_SDK_VERSION,			// 对于 Microsoft Store 应用，始终将此值设置为 D3D11_SDK_VERSION。
		&_d3Device,				// 返回创建的 Direct3D 设备。
		&feature_evel,				// 返回所创建设备的功能级别。
		&_immediateContext			// 返回设备的即时上下文。
	);
	SAFE_RELEASE(selected_adapter);
	if (FAILED(result))
	{
		Debuger::Warning("Failed to D3D11CreateDevice");
		return false;
	}

	Debuger::Log("feature level:0x%04X", feature_evel);
	switch (feature_evel)
	{
	case D3D_FEATURE_LEVEL_10_0:
	case D3D_FEATURE_LEVEL_10_1:
		_shaderVersion = String::ToString(D3D10_SHADER_MAJOR_VERSION) + "_" + String::ToString(D3D10_SHADER_MINOR_VERSION);
		//_maxTextureSize = D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		break;
	case D3D_FEATURE_LEVEL_11_0:
	case D3D_FEATURE_LEVEL_11_1:
		_shaderVersion = String::ToString(D3D11_SHADER_MAJOR_VERSION) + "_" + String::ToString(D3D11_SHADER_MINOR_VERSION);
		//_maxTextureSize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		break;
	default:
		AssertEx(false, "error feature level");
		break;
	}

	_graphicsCaps->Initialize();
	return result == S_OK;
}
bool DX11Device::CreateSwapChain(RenderWindow* window)
{
	CHECK_RETURN_PTR_FALSE(window);
	DX11SwapChain* swap_chain = DX11SwapChain::Create();
	swap_chain->CreateSwapChain(window);
	_swapChains[int(window->GetTargetDisplay())] = swap_chain;
	return true;
}
void DX11Device::CreateRenderContent()
{
	RenderContent* content = DX11RenderContent::Create();
	content->Initialize();
	_renderContent = content;
}
void DX11Device::DestroyDevice(RenderWindow* window)
{
	SAFE_RELEASE(_dXGIFactory);
	SAFE_RELEASE(_immediateContext);
	//TODO:这里会卡死
	SAFE_RELEASE(_d3Device);
}
void DX11Device::Resize(const WindowResizeDesc& desc)
{
	base::Resize(desc);
}
HardwareVertexBuffer* DX11Device::CreateVertexBuffer()
{
	return DX11HardwareVertexBuffer::Create();
}
HardwareIndexBuffer* DX11Device::CreateIndexBuffer()
{
	return DX11HardwareIndexBuffer::Create();
}
CGProgram* DX11Device::CreateShader()
{
	return DX11Program::Create();
}
Texture* DX11Device::CreateTexture(const TextureDesc& desc)
{
	return DX11Texture::Create(desc);
}
RenderTexture* DX11Device::CreateRenderTexture(const TextureDesc& desc)
{
	return DX11RenderTexture::Create(desc);
}
FinalProcess* DX11Device::CreateFinalProcess()
{
	return DX11FinalProcess::Create();
}
DC_END_NAMESPACE

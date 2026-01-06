#include "DX9Device.h"
#include "DX9Caps.h"
#include "DX9GPUAdapter.h"
#include "DX9HardwareVertexBuffer.h"
#include "DX9HardwareIndexBuffer.h"
#include "DX9PostProcess.h"
#include "DX9RenderContent.h"
#include "DX9RenderTexture.h"
#include "DX9Program.h"
#include "DX9SwapChain.h"
#include "DX9Texture.h"
#include "runtime/thread/Thread.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9Device, GraphicsDevice);
DX9Device* GetDX9Device()
{
	return (DX9Device*)Application::GetGraphics();
}
DX9Device::DX9Device()
{
	_d3DInstance = Direct3DCreate9(D3D_SDK_VERSION);
	AssertEx(_d3DInstance, "");

	DX9HR(_d3DInstance->GetAdapterDisplayMode(0, &_displayMode));

	_shaderVersion = "3_0";

	_gpuAdapter = DX9GPUAdapter::Create();
	_graphicsCaps = DX9Caps::Create();
	_videoList = VideoModeList::Create(_d3DInstance);
}
DX9Device::~DX9Device()
{
	SAFE_DELETE(_videoList);
	SAFE_RELEASE(_d3DDevice);
	SAFE_RELEASE(_d3DInstance);
}
bool DX9Device::CreateDevice(RenderWindow* window)
{
	//尝试获得显卡
	GPUAdapterInfo* adapter = _gpuAdapter->SelectAdapters(nullptr);
	if (adapter)
	{
		_d3DAdapter = adapter->GetIndex();
	}
	_graphicsCaps->Initialize();

	DX9HR(_d3DInstance->CheckDeviceType(_d3DAdapter, _d3DDevType, _displayMode.Format, _displayMode.Format, true));

	D3DCAPS9 caps;
	DX9HR(_d3DInstance->GetDeviceCaps(_d3DAdapter, D3DDEVTYPE_HAL, &caps));

	//msaa
	MSAALevel msaa = DX9Caps::GetMSAAQualityLevel(window->GetAntiAlias(), _displayMode.Format, !window->IsFullscreen());
	HWND hWnd = (HWND)window->GetHwnd();

	_d3DParameters = {};
	_d3DParameters.BackBufferCount = DX9BackBufferCount;
	_d3DParameters.BackBufferWidth = window->GetWidth();
	_d3DParameters.BackBufferHeight = window->GetHeight();
	_d3DParameters.BackBufferFormat = DX9ConvertToAlphaFormat(_displayMode.Format);
	_d3DParameters.Windowed = !window->IsFullscreen();
	_d3DParameters.hDeviceWindow = hWnd;
	_d3DParameters.MultiSampleType = (D3DMULTISAMPLE_TYPE)msaa.Count;
	_d3DParameters.MultiSampleQuality = (DWORD)msaa.Quality;
	_d3DParameters.SwapEffect = D3DSWAPEFFECT_DISCARD; //只有SwapEffect是D3DWAPEFFECT_DISCARD，多重采样才会被支持的
	_d3DParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;//D3DPRESENT_INTERVAL_IMMEDIATE;
	_d3DParameters.FullScreen_RefreshRateInHz = window->IsFullscreen() ? window->GetFPS() : D3DPRESENT_RATE_DEFAULT;
	_d3DParameters.Flags = 0;

	//flags
	DWORD flags = D3DCREATE_FPU_PRESERVE;//保持FPU精度，否则整个程序使用double的地方都有莫名其妙的问题。
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		flags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		flags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		Debuger::Warning("------Hardware Shader Not Supported! Using Software Vertext Processing------");
	}
	if (caps.DevCaps & D3DDEVCAPS_PUREDEVICE)
	{
		flags |= D3DCREATE_PUREDEVICE;
	}
	else Debuger::Warning("------Pure Device Not Supported!------");

	//创建设备
	DX9HR(_d3DInstance->CreateDevice(_d3DAdapter, _d3DDevType, hWnd, flags, &_d3DParameters, &_d3DDevice));
	
	//设置抗锯齿
	DX9HR(_d3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, msaa.Count > 1 ? TRUE : FALSE));

	return true;
}
bool DX9Device::CreateSwapChain(RenderWindow* window)
{
	CHECK_RETURN_PTR_FALSE(window);
	DX9SwapChain* swap_chain = DX9SwapChain::Create();
	swap_chain->CreateSwapChain(window);
	_swapChains[int(window->GetTargetDisplay())] = swap_chain;
	return true;
}
void DX9Device::CreateRenderContent()
{
	RenderContent* content = DX9RenderContent::Create();
	content->Initialize();
	_renderContent = content;
}
void DX9Device::DestroyDevice(RenderWindow* window)
{
	SAFE_RELEASE(_d3DDevice);
}
void DX9Device::Resize(const WindowResizeDesc& desc)
{
	if (!_d3DDevice)
		return;

	this->HandlePreLostDevice();

	// If we're resetting device mid-frame (e.g. script calls Screen.SetResolution),
	// we need to end scene, reset and begin scene again.
	bool wasInsideFrame = _renderContent->IsInsideFrame();
	if (wasInsideFrame)
	{
		_renderContent->EndFrame();
	}

	_d3DParameters.Windowed = !desc.fullscreen;
	_d3DParameters.BackBufferWidth = desc.width;
	_d3DParameters.BackBufferHeight = desc.height;
	_d3DParameters.FullScreen_RefreshRateInHz = desc.fullscreen ? _d3DParameters.FullScreen_RefreshRateInHz : D3DPRESENT_RATE_DEFAULT;
	HRESULT hr = _d3DDevice->Reset(&_d3DParameters);
	if (FAILED(hr))
	{
		Debuger::Warning("D3D device reset failed [%s]", DX9GetError(hr));
		this->SetDeviceInLostState(true);
	}

	base::Resize(desc);
}
HardwareVertexBuffer* DX9Device::CreateVertexBuffer()
{
	return DX9HardwareVertexBuffer::Create();
}
HardwareIndexBuffer* DX9Device::CreateIndexBuffer()
{
	return DX9HardwareIndexBuffer::Create();
}
CGProgram* DX9Device::CreateShader()
{
	return DX9Program::Create();
}
Texture* DX9Device::CreateTexture(const TextureDesc& desc)
{
	return DX9Texture::Create(desc);
}
RenderTexture* DX9Device::CreateRenderTexture(const TextureDesc& desc)
{
	return DX9RenderTexture::Create(desc);
}
FinalProcess* DX9Device::CreateFinalProcess()
{
	return DX9FinalProcess::Create();
}

bool DX9Device::SetD3DRenderState(D3DRENDERSTATETYPE render_type, DWORD value)
{
	if (!_d3DDevice)
		return false;

	HRESULT hr = S_OK;
	DWORD dwOldValue;
	hr = _d3DDevice->GetRenderState(render_type, &dwOldValue);
	if (FAILED(hr))
	{
		Debuger::Error("DX9RenderContent::SetD3DRenderState - GetRenderState:%u", GetLastError());
		return false;
	}

	if (dwOldValue == value)
		return true;

	hr = _d3DDevice->SetRenderState(render_type, value);
	if (FAILED(hr))
	{
		Debuger::Error("DX9RenderContent::SetD3DRenderState - SetRenderState:%u", GetLastError());
		return false;
	}

	return true;
}
bool DX9Device::SetD3DSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE sampler_type, DWORD value)
{
	if (!_d3DDevice)
		return false;

	HRESULT hr = S_OK;
	DWORD dwOldValue;
	hr = _d3DDevice->GetSamplerState(sampler, sampler_type, &dwOldValue);
	if (FAILED(hr))
		return false;

	if (dwOldValue == value)
		return true;

	hr = _d3DDevice->SetSamplerState(sampler, sampler_type, value);
	if (FAILED(hr))
		return false;

	return true;
}
bool DX9Device::SetD3DTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
{
	if (!_d3DDevice)
		return false;

	HRESULT hr = S_OK;
	DWORD dwOldValue;
	hr = _d3DDevice->GetTextureStageState(stage, type, &dwOldValue);
	if (FAILED(hr))
		return false;

	if (dwOldValue == value)
		return true;

	hr = _d3DDevice->SetTextureStageState(stage, type, value);
	if (FAILED(hr))
		return false;

	return true;
}
bool DX9Device::ClearD3DTexture(int stage)
{
	if (!_d3DDevice)
		return false;

	HRESULT hr = _d3DDevice->SetTexture(stage, 0);
	if (FAILED(hr))
	{
		Debuger::Error("DX9RenderContent::ClearD3DTexture - 清除纹理失败:%u", GetLastError());
	}
	return hr == S_OK ? true : false;
}
bool DX9Device::SetD3DTexture(int stage, Texture *texture)
{
	if (!_d3DDevice)
		return false;

	CHECK_RETURN_PTR_FALSE(texture);
	return SetD3DTexture(stage, (IDirect3DTexture9*)texture->GetTexture());
}
bool DX9Device::SetD3DTexture(int stage, IDirect3DTexture9* texture)
{
	if (!_d3DDevice)
		return false;

	HRESULT hr = _d3DDevice->SetTexture(stage, texture);
	if (FAILED(hr))
	{
		Debuger::Error("DX9RenderContent::SetD3DTexture - 设置纹理失败:%u", GetLastError());
	}
	return hr == S_OK ? true : false;
}
void DX9Device::SetDeviceInLostState(bool b) 
{ 
	if (_deviceInLostState != b)
	{
		_deviceInLostState = b;
	}
}
void DX9Device::HandlePreLostDevice()
{
	this->TriggerEvent(PreLostDeviceEvt);
}
void DX9Device::HandleLostDevice()
{
	this->TriggerEvent(LostDeviceEvt);
}
void DX9Device::HandleResetDevice()
{
	this->TriggerEvent(ResetDeviceEvt);
}
void DX9Device::CheckDeviceLost()
{
	if (!_d3DDevice)
		return;

	if (!this->IsDeviceInLostState())
		return;

	HRESULT hr = _d3DDevice->TestCooperativeLevel();
	if (FAILED(hr))
	{
		switch (hr)
		{
		case D3DERR_DEVICELOST://需要继续等待
			Thread::Sleep(100);
			break;

		case D3DERR_DEVICENOTRESET://可以恢复
			this->HandleLostDevice();
			hr = _d3DDevice->Reset(&_d3DParameters);
			if (SUCCEEDED(hr))
			{
				this->SetDeviceInLostState(false);
				this->HandleResetDevice();
			}
			else
			{
				if (hr != D3DERR_DEVICELOST)
				{
					Debuger::Warning("D3D device reset failed [%s]", DX9GetError(hr));
				}
			}
			break;
		}
	}
}
void DX9Device::GetAdapterDisplaySize(int &w, int &h)
{
	D3DDISPLAYMODE d3ddm;
	DX9HR(_d3DInstance->GetAdapterDisplayMode(_d3DAdapter, &d3ddm));
	w = d3ddm.Width;
	h = d3ddm.Height;
}
/********************************************************************/
VideoModeList::VideoModeList(LPDIRECT3D9 d3d)
{
	_videoModes.Reserve(30);
	Enumerate(d3d);
}

VideoModeList::~VideoModeList()
{
	_videoModes.Clear();
}

void VideoModeList::Enumerate(LPDIRECT3D9 d3d)
{
	for (uint i = 0; i < d3d->GetAdapterModeCount(0, D3DFMT_R5G6B5); i++)//16bit
	{
		D3DDISPLAYMODE kTempMode;
		DX9HR(d3d->EnumAdapterModes(0, D3DFMT_R5G6B5, i, &kTempMode));
		if (kTempMode.Width < 640 || kTempMode.Height < 400)
		{
			continue;
		}

		bool bFind = false;
		auto it = _videoModes.begin();
		while (it != _videoModes.end())
		{
			D3DDISPLAYMODE kOldMode = *it;
			if (kOldMode.Format == kTempMode.Format && kOldMode.Width == kTempMode.Width && kOldMode.Height == kTempMode.Height)
			{
				bFind = true;
				break;
			}
			it++;
		}

		if(!bFind)
		{
			_videoModes.Add(kTempMode);
		}
	}

	for (uint i = 0; i < d3d->GetAdapterModeCount(0, D3DFMT_X8R8G8B8); i++)//32bit
	{
		D3DDISPLAYMODE kTempMode;
		DX9HR(d3d->EnumAdapterModes(0, D3DFMT_X8R8G8B8, i, &kTempMode));
		if (kTempMode.Width < 640 || kTempMode.Height < 400)
		{
			continue;
		}

		bool bFind = false;
		auto it = _videoModes.begin();
		while (it != _videoModes.end())
		{
			D3DDISPLAYMODE kOldMode = *it;
			if (kOldMode.Format == kTempMode.Format && kOldMode.Width == kTempMode.Width && kOldMode.Height == kTempMode.Height)
			{
				bFind = true;
				break;
			}
			it++;
		}

		if(!bFind)
		{
			_videoModes.Add(kTempMode);
		}
	}
}
DC_END_NAMESPACE
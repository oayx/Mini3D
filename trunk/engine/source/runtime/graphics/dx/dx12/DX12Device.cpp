
#include "DX12Device.h"
#include "DX12Caps.h"
#include "DX12HardwareVertexBuffer.h"
#include "DX12HardwareIndexBuffer.h"
#include "DX12PostProcess.h"
#include "DX12RenderContent.h"
#include "DX12Texture.h"
#include "DX12RenderTexture.h"
#include "DX12Program.h"
#include "../DXGPUAdapter.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12Device, GraphicsDevice);
DX12Device* GetDX12Device()
{
	return dynamic_cast<DX12Device*>(Application::GetGraphics());
}
DX12Device::DX12Device()
{
	_gpuAdapter = DXGPUAdapter::Create();
	_graphicsCaps = DX12Caps::Create();
}
DX12Device::~DX12Device()
{
	this->FlushCommandQueue();
#ifdef DC_DEBUG
	IDXGIDebug* pDebug = NULL;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug))))
	{
		pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
		pDebug->Release();
	}
#endif
	SAFE_RELEASE(_fence);
	SAFE_CLOSEHANDLE(_fenceEvent);
	SAFE_RELEASE(_commandList);
	SAFE_RELEASE(_commandAllocator);
	SAFE_RELEASE(_commandQueue);
	SAFE_RELEASE(_dXGIFactory);
	SAFE_RELEASE(_rootSignature);
	SAFE_RELEASE(_d3Device);
}
bool DX12Device::CreateDevice(RenderWindow* window)
{
	DX_ERROR(CreateDXGIFactory1(IID_PPV_ARGS(&_dXGIFactory)));

	//尝试获得显卡
	D3D_FEATURE_LEVEL feature_evel = D3D_FEATURE_LEVEL_12_0;
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
			feature_evel = ((DXGPUAdapterInfo*)adapter)->FeatureLevel;
		}
	}

#if DC_DEBUG
	ID3D12Debug* dx12Debug = NULL;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dx12Debug))))
	{
		dx12Debug->EnableDebugLayer();
		dx12Debug->Release();
	}
#endif
	HRESULT result = D3D12CreateDevice(nullptr, feature_evel, IID_PPV_ARGS(&_d3Device));
	SAFE_RELEASE(selected_adapter);
	if (FAILED(result))
	{
		Debuger::Warning("Failed to D3D12CreateDevice use Hardware");
		if (FAILED(_dXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&selected_adapter))))
		{
			Debuger::Error("Cannot get the Wrap adapter");
			return false;
		}
		result = D3D12CreateDevice(selected_adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_d3Device));
		SAFE_RELEASE(selected_adapter);
		if (FAILED(result))
		{
			Debuger::Error("Failed to D3D12CreateDevice use Wrap");
			return false;
		}
		Debuger::Warning("D3D12CreateDevice use Wrap");
		feature_evel = D3D_FEATURE_LEVEL_11_0;
	}

	//feature_evel
	Debuger::Log("feature level:0x%04X", feature_evel);
	if (feature_evel >= D3D_FEATURE_LEVEL_11_0 && feature_evel < D3D_FEATURE_LEVEL_12_0)
	{
		_shaderVersion = String::ToString(D3D11_SHADER_MAJOR_VERSION) + "_0";
		//_maxTextureSize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
	}
	else if(feature_evel >= D3D_FEATURE_LEVEL_12_0)
	{
		_shaderVersion = String::ToString(D3D12_SHADER_MAJOR_VERSION) + "_0";
		//_maxTextureSize = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
	}
	else
	{
		AssertEx(false, "feature level error");
	}
	_featureLevel = feature_evel;

	_rtvDescriptorSize = _d3Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_dsvDescriptorSize = _d3Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	_cbvSrvUavDescriptorSize = _d3Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	this->CreateRootSignature();
	this->CreateCommandQueue();
	_graphicsCaps->Initialize();

	return result == S_OK;
}
bool DX12Device::CreateSwapChain(RenderWindow* window)
{
	CHECK_RETURN_PTR_FALSE(window);
	DX12SwapChain* swap_chain = DX12SwapChain::Create();
	swap_chain->CreateSwapChain(window);
	_swapChains[int(window->GetTargetDisplay())] = swap_chain;
	return true;
}
void DX12Device::CreateRenderContent()
{
	RenderContent* content = DX12RenderContent::Create();
	content->Initialize();
	_renderContent = content;
}
void DX12Device::Resize(const WindowResizeDesc& desc)
{	
	base::Resize(desc);
}
void DX12Device::PreRender(RenderWindow* window)
{
	base::PreRender(window);
	_commandAllocator->Reset();
	_commandList->Reset(_commandAllocator, nullptr);
	_commandList->SetGraphicsRootSignature(_rootSignature);
}
void DX12Device::PostRender(RenderWindow* window)
{
	_commandList->Close();
	_commandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&_commandList);
	base::PostRender(window);
}
HardwareVertexBuffer* DX12Device::CreateVertexBuffer()
{
	return DX12HardwareVertexBuffer::Create();
}
HardwareIndexBuffer* DX12Device::CreateIndexBuffer()
{
	return DX12HardwareIndexBuffer::Create();
}
CGProgram* DX12Device::CreateShader()
{
	return DX12Program::Create();
}
Texture* DX12Device::CreateTexture(const TextureDesc& desc)
{
	return DX12Texture::Create(desc);
}
RenderTexture* DX12Device::CreateRenderTexture(const TextureDesc& desc)
{
	return DX12RenderTexture::Create(desc);
}
FinalProcess* DX12Device::CreateFinalProcess()
{
	return DX12FinalProcess::Create();
}
void DX12Device::CreateCommandQueue()
{
	{//fence
		DX_ERROR(_d3Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
		_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		AssertEx(_fenceEvent != NULL, "");
	}
	{//command
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 1;
		DX_ERROR(_d3Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue)));
		DX_ERROR(_d3Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator)));
		DX_ERROR(_d3Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator, NULL, IID_PPV_ARGS(&_commandList)));
		DX_ERROR(_commandList->Close());
	}
}
void DX12Device::FlushCommandQueue()
{
	_fenceValue = _fenceValue + 1;//CPU传完命令并关闭后，将当前围栏值+1
	_commandQueue->Signal(_fence, _fenceValue);//当GPU处理完CPU传入的命令后，将fence接口中的围栏值+1，即fence->GetCompletedValue()+1
	if (_fence->GetCompletedValue() < _fenceValue)//如果小于，说明GPU没有处理完所有命令
	{
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);//当围栏达到_fenceValue值（即执行到Signal（）指令修改了围栏值）时触发的eventHandle事件
		::WaitForSingleObject(_fenceEvent, INFINITE);//等待GPU命中围栏，激发事件
	}
}
void DX12Device::CreateRootSignature()
{
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER rootParameter[MaxTextureDescriptor + MaxCBufferDescriptor];
	CD3DX12_DESCRIPTOR_RANGE cbvTable[MaxTextureDescriptor];
	for (int i = 0; i < MaxTextureDescriptor; ++i)
	{
		cbvTable[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
		rootParameter[i].InitAsDescriptorTable(1, &cbvTable[i], D3D12_SHADER_VISIBILITY_PIXEL);
	}
	for (int i = 0; i < MaxCBufferDescriptor; ++i)
	{
		rootParameter[i + MaxTextureDescriptor].InitAsConstantBufferView(i);
	}

	// Static samplers
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, 									// shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT,		// filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);	// addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, 									// shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT,		// filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	// addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	// addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);	// addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, 									// shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);	// addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, 									// shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	// addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	// addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);	// addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4,									// shaderRegister
		D3D12_FILTER_ANISOTROPIC,			// filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressW
		0.0f,								// mipLODBias
		8);									// maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, 									// shaderRegister
		D3D12_FILTER_ANISOTROPIC,			// filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	// addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	// addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	// addressW
		0.0f,								// mipLODBias
		8);									// maxAnisotropy
	D3D12_STATIC_SAMPLER_DESC staticSamplers[6] = { pointWrap, pointClamp,linearWrap, linearClamp,anisotropicWrap, anisotropicClamp };

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(ARRAY_SIZE(rootParameter), rootParameter, (UINT)ARRAY_SIZE(staticSamplers), staticSamplers, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob);
	if (errorBlob != nullptr)
	{
		Debuger::Error("D3D12SerializeRootSignature error:%s", (char*)errorBlob->GetBufferPointer());
		SAFE_RELEASE(errorBlob);
	}
	DX_ERROR(hr);
	DX_ERROR(_d3Device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&_rootSignature)));
	SAFE_RELEASE(serializedRootSig);
}
DC_END_NAMESPACE


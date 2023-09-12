#include "RenderTexture.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
RenderTexture::RenderTextures RenderTexture::_renderTextures;
IMPL_DERIVED_REFECTION_TYPE(RenderTexture, Texture);
RenderTexture* RenderTexture::Alloc(const TextureDesc& desc)
{
	RenderTargetKey key;
	key.width = desc.width;
	key.height = desc.height;
	key.color_format = desc.format;
	key.depth_format = desc.format;
	key.filter_mode = desc.filter;
	key.wrap_mode = desc.address;
	key.flags = desc.flags;

	RenderTexture* rt = nullptr;
	List<RenderTexture*> *list;
	if (_renderTextures.TryGet(key.u, &list))
	{
		if (list->Size() > 0)
		{
			rt = list->First();
			list->RemoveFirst();
		}
	}

	if (rt == nullptr)
	{
		rt = Application::GetGraphics()->CreateRenderTexture(desc);
		rt->Retain();
	}

	rt->_key = key;
	return rt;
}
void RenderTexture::Free(RenderTexture* rt)
{
	if (rt == nullptr)return;

	List<RenderTexture*> *list;
	if (_renderTextures.TryGet(rt->_key.u, &list))
	{
		list->AddLast(rt);
	}
	else
	{
		List<RenderTexture*> new_list;
		new_list.AddLast(rt);
		_renderTextures.Add(rt->_key.u, new_list);
	}
}
void RenderTexture::Free()
{
	for (auto list : _renderTextures)
	{
		for (auto rt : list.second)
		{
			SAFE_RELEASE(rt);
		}
	}
	_renderTextures.Clear();
}
void RenderTexture::BeginFrame(RenderFrameDesc& desc)
{
	Application::GetGraphics()->GetRenderContent()->BeginFrame(desc);
}
void RenderTexture::EndFrame()
{
	Application::GetGraphics()->GetRenderContent()->EndFrame();
}
DC_END_NAMESPACE

#include "VKRenderTexture.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKRenderTexture, RenderTexture);
VKRenderTexture::VKRenderTexture(const TextureDesc& desc)
: base(desc)
{

}
VKRenderTexture::~VKRenderTexture()
{
}
void VKRenderTexture::PreRender()
{
}
void VKRenderTexture::BeginFrame(RenderFrameDesc& desc)
{
	base::BeginFrame(desc);
}
void VKRenderTexture::PostRender()
{
}
bool VKRenderTexture::Copy(Texture* dst)
{
	AssertEx(dst->GetWidth() == this->GetWidth(), "");
	AssertEx(dst->GetHeight() == this->GetHeight(), "");
	AssertEx(dst->GetFormat() == this->GetFormat(), "");
	return true;
}
void VKRenderTexture::SaveToFile(const String& name, ImageType type)
{

}
DC_END_NAMESPACE
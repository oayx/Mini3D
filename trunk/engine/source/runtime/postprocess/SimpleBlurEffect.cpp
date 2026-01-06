#include "SimpleBlurEffect.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(SimpleBlurEffect, PostEffect);
SimpleBlurEffect::SimpleBlurEffect()
{
	_material = Material::Create("internal/material/PostProcess/SimpleBlur.material");
	_material->Retain();
}
SimpleBlurEffect::~SimpleBlurEffect()
{
	SAFE_RELEASE(_material);
}
Object* SimpleBlurEffect::Clone(Object* newObj)
{
	base::Clone(newObj);
	SimpleBlurEffect* obj = dynamic_cast<SimpleBlurEffect*>(newObj);
	if (!obj)return newObj;

	obj->_blurRadius = _blurRadius;

	return obj;
}
void SimpleBlurEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
{
	if (_material != nullptr)
	{
		TextureDesc desc;
		desc.width = source->GetWidth() >> 1; desc.height = source->GetHeight() >> 1; desc.format = source->GetFormat();
		desc.flags = TextureFlag::COLOR;
		RenderTexture* rt1 = RenderTexture::Alloc(desc);
		RenderTexture* rt2 = RenderTexture::Alloc(desc);
		_material->SetFloat("_BlurRadius", _blurRadius);
		Blit(source, rt1);
		//进行迭代，一次迭代进行了两次模糊操作，使用两张RT交叉处理
		Blit(rt1, rt2, _material);
		Blit(rt2, rt1, _material);
		Blit(rt1, dest);
		RenderTexture::Free(rt1);
		RenderTexture::Free(rt2);
	}
	else
	{
		Blit(source, dest);
	}
}
void SimpleBlurEffect::OnDrawEditor()
{
	base::OnDrawEditor();
	ImGuiEx::Label("Blur Radius");
	ImGui::DragFloat("##BlurRadius", &_blurRadius, 0.01f, 0.0f, 5.0f);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(SimpleBlurEffect);
template<class TransferFunction> inline
void SimpleBlurEffect::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_blurRadius);
}
DC_END_NAMESPACE
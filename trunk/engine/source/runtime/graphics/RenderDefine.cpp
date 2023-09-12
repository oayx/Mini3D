#include "RenderDefine.h"
#include "runtime/resources/AssetsManager.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(ShaderVariable);
template<class TransferFunction> inline
void ShaderVariable::Transfer(TransferFunction& transfer, void* ptr)
{
	TRANSFER_SIMPLE(Name);
	TRANSFER_ENUM(Type);
	TRANSFER_SIMPLE(Value);
	TRANSFER_SIMPLE(Range);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(ShaderTexture);
template<class TransferFunction> inline
void ShaderTexture::Transfer(TransferFunction& transfer, void* ptr)
{
	String old_file = File;
	if (!transfer.IsRead())
	{
		AssetMeta* meta = AssetsManager::GetMetaByFile(File);
		if (meta)File = meta->GetGUID();
	}
	TRANSFER_SIMPLE(Name);
	TRANSFER_SIMPLE(File);
	TRANSFER_ENUM(Type);
	TRANSFER_SIMPLE(Scale);
	TRANSFER_SIMPLE(Offset);

	if (transfer.IsRead())
	{
		AssetMeta* meta = AssetsManager::GetMeta(File);
		if (meta)
			File = meta->GetFullPath();
		else
			File = "";
	}
	else
	{
		File = old_file;
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(ViewPortDesc);
template<class TransferFunction> inline
void ViewPortDesc::Transfer(TransferFunction& transfer, void* ptr)
{
	TRANSFER_SIMPLE(x);
	TRANSFER_SIMPLE(y);
	TRANSFER_SIMPLE(w);
	TRANSFER_SIMPLE(h);
	TRANSFER_SIMPLE(z_near);
	TRANSFER_SIMPLE(z_far);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(FogDesc);
template<class TransferFunction> inline
void FogDesc::Transfer(TransferFunction& transfer, void* ptr)
{
	TRANSFER_SIMPLE(enable);
	TRANSFER_ENUM(mode);
	TRANSFER_SIMPLE(color);
	TRANSFER_SIMPLE(start);
	TRANSFER_SIMPLE(end);
	TRANSFER_SIMPLE(density);
}
DC_END_NAMESPACE
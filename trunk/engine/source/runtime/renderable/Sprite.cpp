#include "Sprite.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h" 
#include "runtime/resources/AssetsManager.h"
#include "runtime/ui/GUI.h"
#include "runtime/Application.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Sprite, Renderer);
void Sprite::Awake()
{
	base::Awake();

	float w = 1, h = 1;
	this->SetVertexs({ Vector3(-0.5f*w, 0.5f*h, 0), Vector3(0.5f*w, 0.5f*h, 0), Vector3(0.5f*w, -0.5f*h, 0), Vector3(-0.5f*w, -0.5f*h, 0) });
	this->SetColor(_color);
	this->SetTexcoords1({ Vector2(0, 0) ,Vector2(1, 0),Vector2(1, 1),Vector2(0, 1) });
	this->SetIndices({ 0,1,2,0,2,3 });

	this->CreateMaterial();
}
void Sprite::Update()
{
	base::Update();
	if (_isDirty)
	{
		_isDirty = false;
		this->UploadData();
	}
}
Object* Sprite::Clone(Object* newObj)
{
	base::Clone(newObj);
	Sprite* obj = dynamic_cast<Sprite*>(newObj);
	if (!obj)return newObj;

	obj->SetColor(_color);

	return obj;
}
void Sprite::SetColor(const Color& color)
{
	base::SetColor(color);
	_color = color;
	_isDirty = true;
}
void Sprite::SetTexture(Texture* tex)
{
	GetMaterial()->SetTexture(tex);
	if (tex)
	{
		float w = tex->GetWidth()*0.01f, h = tex->GetHeight()*0.01f;
		SetVertexs({ Vector3(-0.5f*w, 0.5f*h, 0), Vector3(0.5f*w, 0.5f*h, 0), Vector3(0.5f*w, -0.5f*h, 0), Vector3(-0.5f*w, -0.5f*h, 0) });
	}
	this->UploadData();
}
Size Sprite::GetSize()
{
	Texture* tex = GetMaterial()->GetTexture();
	if (tex)
	{
		float w = tex->GetWidth()*0.01f, h = tex->GetHeight()*0.01f;
		const Vector3& scale = GetTransform()->GetScale();
		return Size(w*scale.x, h*scale.y);
	}
	return Size::zero;
}
void Sprite::CreateMaterial()
{
	Material* mat = base::SetMaterial("internal/material/Sprite.material");
	mat->SetTexture(Texture::GetWhiteTexture());
}
void Sprite::OnDrawEditor()
{
	base::OnDrawEditor();
	{
		ImGuiEx::Label("File");

		Texture* texture = GetMaterial()->GetTexture();
		String file = texture ? texture->GetResFile() : "";
		ImGui::Image((ImTextureID)GUITextureCache::Get(file), ImVec2(22, 22));
		if (ImGui::IsItemClicked(0) && !file.IsEmpty())
		{
			EMain_Project::SetSelectFile(file);
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
			{
				const String& filePath = EditorCursor::GetDragFile();
				if (Resource::GetResourceType(filePath) == ResourceType::Texture)
				{
					SetTexture(Texture::Create(filePath));
				}
				EditorCursor::EndDrag();
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		if (ImGui::Selectable(ICON_FA_LINK, false, 0, ImGui::CalcTextSize(ICON_FA_LINK)))
		{
			auto OnSelectAsset = [this](const String& file)
			{
				SetTexture(Texture::Create(file));
			};

			EAssetSelect::Search(ResourceType::Texture, OnSelectAsset);
		}
	}

	ImGuiEx::Label("Color");
	if (ECommonComponent::ShowColor("Color", _color.p, false))
	{
		this->SetColor(_color);
	}

	base::OnDrawOrder();
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Sprite);
template<class TransferFunction> inline
void Sprite::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	String File = "";
	if (transfer.IsRead())
	{
		TRANSFER_SIMPLE(File);
		AssetMeta* meta = AssetsManager::GetMeta(File);
		if (meta)File = meta->GetFullPath();
	}
	else
	{
		Texture* texture = GetMaterial()->GetTexture();
		if (texture)
		{
			File = texture->GetResFile();
			AssetMeta* meta = AssetsManager::GetMetaByFile(File);
			if (meta)File = meta->GetGUID();
			TRANSFER_SIMPLE(File);
		}
	}
	TRANSFER_SIMPLE(_color);

	if (transfer.IsRead())
	{
		if (!File.IsEmpty())
		{
			SetTexture(Texture::Create(File));
		}
		SetColor(_color);
	}
}
DC_END_NAMESPACE
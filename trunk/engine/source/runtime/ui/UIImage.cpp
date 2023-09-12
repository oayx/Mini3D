#include "UIImage.h"
#include "UIAtlas.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/resources/AssetsManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UIImage, UIView);
Object* UIImage::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UIImage* obj = dynamic_cast<UIImage*>(new_obj);
	if (!obj)return new_obj;

	obj->SetAtlas(_atlasFile, _imageName);
	obj->SetImageType(_imageType);
	obj->SetImageBorder(_imageBorder);
	obj->SetFillMethod(_fillMethod, _fillOrigin);
	obj->SetFillAmount(_fillAmount);

	return obj;
}
bool UIImage::FillMesh(VariablePrimitive* primitive, Material* material, int& vxt_offset, int& idx_offset)
{
	Texture* texture = nullptr;
	if (!_atlasFile.IsEmpty())
		texture = UIAtlasManager::GetAtlasTexture(_atlasFile);
	if (texture == nullptr)
		texture = Texture::GetWhiteTexture();
	material->SetTexture(texture);

	bool result = false;
	switch (_imageType)
	{
	case UIImageType::Simple:
		result = FillSimpleMesh(primitive, vxt_offset, idx_offset);
		break;
	case UIImageType::Sliced:
		result = FillSlicedMesh(primitive, vxt_offset, idx_offset);
		if(!result)
			result = FillSimpleMesh(primitive, vxt_offset, idx_offset);
		break;
	case UIImageType::Filled:
		switch (_fillMethod)
		{
		case UIImageFillMethod::Horizontal:
			result = FillHorizontalMesh(primitive, vxt_offset, idx_offset);
			if (!result)
				result = FillSimpleMesh(primitive, vxt_offset, idx_offset);
			break;
		case UIImageFillMethod::Vertical:
			result = FillVerticalMesh(primitive, vxt_offset, idx_offset);
			if (!result)
				result = FillSimpleMesh(primitive, vxt_offset, idx_offset);
			break;
		}
		break;
		default:break;
	}
	return result;
}
bool UIImage::FillSimpleMesh(VariablePrimitive* primitive, int& vxt_offset, int& idx_offset)
{
	const Size& size = this->GetSize();
	float w = size.width, h = size.height;
	const Matrix4& mat_world = this->GetTransform()->GetLocalToWorldMatrix();

	//position
	primitive->AddVertex(mat_world.TransformPoint(Vector3(-0.5f*w, 0.5f*h, 0)));
	primitive->AddVertex(mat_world.TransformPoint(Vector3(0.5f*w, 0.5f*h, 0)));
	primitive->AddVertex(mat_world.TransformPoint(Vector3(0.5f*w, -0.5f*h, 0)));
	primitive->AddVertex(mat_world.TransformPoint(Vector3(-0.5f*w, -0.5f*h, 0)));

	//uv
	Vector2 uv1 = { 0,0 }, uv2 = { 1,0 }, uv3 = { 1,1 }, uv4 = { 0,1 };
	UIAtlasInfo atlas_info;
	if (UIAtlasManager::GetAtlasInfo(this->GetAtlasFile(), this->GetImageName(), atlas_info))
	{
		uv1 = Vector2(atlas_info.uv_rect.x, atlas_info.uv_rect.y);
		uv2 = Vector2(atlas_info.uv_rect.x + atlas_info.uv_rect.width, atlas_info.uv_rect.y);
		uv3 = Vector2(atlas_info.uv_rect.x + atlas_info.uv_rect.width, atlas_info.uv_rect.y + atlas_info.uv_rect.height);
		uv4 = Vector2(atlas_info.uv_rect.x, atlas_info.uv_rect.y + atlas_info.uv_rect.height);
	}
	primitive->AddTexcoord1(uv1);
	primitive->AddTexcoord1(uv2);
	primitive->AddTexcoord1(uv3);
	primitive->AddTexcoord1(uv4);

	//color
	primitive->AddColor(this->GetColor());
	primitive->AddColor(this->GetColor());
	primitive->AddColor(this->GetColor());
	primitive->AddColor(this->GetColor());

	//index
	primitive->AddIndex(vxt_offset + 0);
	primitive->AddIndex(vxt_offset + 1);
	primitive->AddIndex(vxt_offset + 2);
	primitive->AddIndex(vxt_offset + 0);
	primitive->AddIndex(vxt_offset + 2);
	primitive->AddIndex(vxt_offset + 3);

	//submesh
	vxt_offset += 4;
	idx_offset += 6;

	return true;
}
bool UIImage::FillSlicedMesh(VariablePrimitive* primitive, int& vxt_offset, int& idx_offset)
{
	if (_imageBorder == Vector4::zero)
		return false;
	UIAtlasInfo atlas_info;
	if (!UIAtlasManager::GetAtlasInfo(this->GetAtlasFile(), this->GetImageName(), atlas_info))
		return false;
	Texture* texture = UIAtlasManager::GetAtlasTexture(_atlasFile);
	if (texture == nullptr)
		return false;
	float tex_width = (float)texture->GetWidth();
	float tex_height = (float)texture->GetHeight();

	const Size& size = this->GetSize();
	Rect rect(-0.5f*size.width, 0.5f*size.height, size.width, size.height);
	int border_l = (int)_imageBorder.x;
	int border_r = (int)_imageBorder.z;
	int border_t = (int)_imageBorder.y;
	int border_b = (int)_imageBorder.w;
	const Matrix4& mat_world = this->GetTransform()->GetLocalToWorldMatrix();

	int x = 0;
	int y = 0;
	Vector3 vertex[16];
	Vector2 uv[16];
	vertex[0] = Vector3(rect.x, rect.y, 0);
	uv[0] = Vector2(atlas_info.rect.x / tex_width, atlas_info.rect.y / tex_height);

	x = (int)(rect.x + border_l);
	vertex[1] = Vector3((float)x, vertex[0].y, 0);
	uv[1] = Vector2((atlas_info.rect.x + border_l) / tex_width, uv[0].y);

	x = (int)(rect.x + rect.width - border_r);
	vertex[2] = Vector3((float)x, vertex[0].y, 0);
	uv[2] = Vector2((atlas_info.rect.x + atlas_info.rect.width - border_r) / tex_width, uv[0].y);

	vertex[3] = Vector3(rect.x + rect.width, vertex[0].y, 0);
	uv[3] = Vector2((atlas_info.rect.x + atlas_info.rect.width) / tex_width, uv[0].y);

	y = (int)(rect.y - border_t);
	vertex[4] = Vector3(vertex[0].x, (float)y, 0);
	uv[4] = Vector2(uv[0].x, (atlas_info.rect.y + border_t) / tex_height);

	vertex[5] = Vector3(vertex[1].x, vertex[4].y, 0);
	uv[5] = Vector2(uv[1].x, uv[4].y);
	vertex[6] = Vector3(vertex[2].x, vertex[4].y, 0);
	uv[6] = Vector2(uv[2].x, uv[4].y);
	vertex[7] = Vector3(vertex[3].x, vertex[4].y, 0);
	uv[7] = Vector2(uv[3].x, uv[4].y);

	y = (int)(rect.y - rect.height + border_b);
	vertex[8] = Vector3(vertex[0].x, (float)y, 0);
	uv[8] = Vector2(uv[0].x, (atlas_info.rect.y + atlas_info.rect.height - border_b) / tex_height);

	vertex[9] = Vector3(vertex[1].x, vertex[8].y, 0);
	uv[9] = Vector2(uv[1].x, uv[8].y);
	vertex[10] = Vector3(vertex[2].x, vertex[8].y, 0);
	uv[10] = Vector2(uv[2].x, uv[8].y);
	vertex[11] = Vector3(vertex[3].x, vertex[8].y, 0);
	uv[11] = Vector2(uv[3].x, uv[8].y);

	vertex[12] = Vector3(vertex[0].x, rect.y - rect.height, 0);
	uv[12] = Vector2(uv[0].x, (atlas_info.rect.y + atlas_info.rect.height) / tex_height);

	vertex[13] = Vector3(vertex[1].x, vertex[12].y, 0);
	uv[13] = Vector2(uv[1].x, uv[12].y);
	vertex[14] = Vector3(vertex[2].x, vertex[12].y, 0);
	uv[14] = Vector2(uv[2].x, uv[12].y);
	vertex[15] = Vector3(vertex[3].x, vertex[12].y, 0);
	uv[15] = Vector2(uv[3].x, uv[12].y);

	//position
	for (int i = 0; i < 16; ++i)vertex[i] = mat_world.TransformPoint(vertex[i]);
	primitive->AddVertexRange(vertex, 16);
	//uv
	primitive->AddTexcoord1Range(uv, 16);
	//color
	Vector<Color> colors(16, this->GetColor());
	primitive->AddColorRange(colors);

	//index
	int iPerEdgeVerCount = 3 + 1;
	int q = 0; int index = 0;
	for (ushort j = 0; j < 3; j += 1)
	{
		int p = 0;
		for (ushort i = 0; i < 3; i += 1)
		{
			primitive->AddIndex(vxt_offset + p*iPerEdgeVerCount + q);
			primitive->AddIndex(vxt_offset + p*iPerEdgeVerCount + q + iPerEdgeVerCount + 1);
			primitive->AddIndex(vxt_offset + p*iPerEdgeVerCount + q + iPerEdgeVerCount);
			primitive->AddIndex(vxt_offset + p*iPerEdgeVerCount + q);
			primitive->AddIndex(vxt_offset + p*iPerEdgeVerCount + q + 1);
			primitive->AddIndex(vxt_offset + p*iPerEdgeVerCount + q + iPerEdgeVerCount + 1);
			p++;
		}
		q++;
	}

	//submesh
	vxt_offset += 16;
	idx_offset += 54;

	return true;
}
bool UIImage::FillHorizontalMesh(VariablePrimitive* primitive, int& vxt_offset, int& idx_offset)
{
	UIAtlasInfo atlas_info;
	if (!UIAtlasManager::GetAtlasInfo(this->GetAtlasFile(), this->GetImageName(), atlas_info))
		return false;
	Texture* texture = UIAtlasManager::GetAtlasTexture(_atlasFile);
	if (texture == nullptr)
		return false;
	float tex_width = (float)texture->GetWidth();
	float tex_height = (float)texture->GetHeight();

	const Size& size = this->GetSize();
	Rect rect(-0.5f*size.width, 0.5f*size.height, size.width, size.height);
	const Matrix4& mat_world = this->GetTransform()->GetLocalToWorldMatrix();

	Vector3 vertex[4];
	Vector2 uv[4];
	if (_fillOrigin == (int)UIImageOriginHorizontal::Left)
	{
		vertex[0] = Vector3(rect.x, rect.y, 0);
		uv[0] = Vector2(atlas_info.rect.x / tex_width, atlas_info.rect.y / tex_height);

		vertex[1] = Vector3(vertex[0].x, rect.y - rect.height, 0);
		uv[1] = Vector2(uv[0].x, (atlas_info.rect.y + atlas_info.rect.height) / tex_height);

		vertex[2] = Vector3(rect.x + rect.width * _fillAmount, vertex[1].y, 0);
		uv[2] = Vector2((atlas_info.rect.x + atlas_info.rect.width * _fillAmount) / tex_width, uv[1].y);

		vertex[3] = Vector3(vertex[2].x, vertex[0].y, 0);
		uv[3] = Vector2(uv[2].x, uv[0].y);
	}
	else if (_fillOrigin == (int)UIImageOriginHorizontal::Right)
	{
		vertex[0] = Vector3(rect.x + rect.width * (1.0f - _fillAmount), rect.y, 0);
		uv[0] = Vector2((atlas_info.rect.x + atlas_info.rect.width * (1.0f - _fillAmount)) / tex_width, atlas_info.rect.y / tex_height);

		vertex[1] = Vector3(vertex[0].x, rect.y - rect.height, 0);
		uv[1] = Vector2(uv[0].x, (atlas_info.rect.y + atlas_info.rect.height) / tex_height);

		vertex[2] = Vector3(rect.x + rect.width, vertex[1].y, 0);
		uv[2] = Vector2((atlas_info.rect.x + atlas_info.rect.width) / tex_width, uv[1].y);

		vertex[3] = Vector3(vertex[2].x, vertex[0].y, 0);
		uv[3] = Vector2(uv[2].x, uv[0].y);
	}
	//position
	for (int i = 0; i < 4; ++i)vertex[i] = mat_world.TransformPoint(vertex[i]);
	primitive->AddVertexRange(vertex, 4);

	//uv
	primitive->AddTexcoord1Range(uv, 4);

	//color
	Vector<Color> colors(4, this->GetColor());
	primitive->AddColorRange(colors);

	//index
	uint indexes[] = { uint(vxt_offset + 0) ,uint(vxt_offset + 1) ,uint(vxt_offset + 2) ,uint(vxt_offset + 0) ,uint(vxt_offset + 2) ,uint(vxt_offset + 3) };
	primitive->AddIndexRange(indexes, 6);

	//submesh
	vxt_offset += 4;
	idx_offset += 6;

	return true;
}
bool UIImage::FillVerticalMesh(VariablePrimitive* primitive, int& vxt_offset, int& idx_offset)
{
	UIAtlasInfo atlas_info;
	if (!UIAtlasManager::GetAtlasInfo(this->GetAtlasFile(), this->GetImageName(), atlas_info))
		return false;
	Texture* texture = UIAtlasManager::GetAtlasTexture(_atlasFile);
	if (texture == nullptr)
		return false;
	float tex_width = (float)texture->GetWidth();
	float tex_height = (float)texture->GetHeight();

	const Size& size = this->GetSize();
	Rect rect(-0.5f*size.width, 0.5f*size.height, size.width, size.height);
	const Matrix4& mat_world = this->GetTransform()->GetLocalToWorldMatrix();

	Vector3 vertex[4];
	Vector2 uv[4];
	if (_fillOrigin == (int)UIImageOriginVertical::Bottom)
	{
		vertex[0] = Vector3(rect.x, rect.y - rect.height * (1.0f - _fillAmount), 0);
		uv[0] = Vector2(atlas_info.rect.x / tex_width, (atlas_info.rect.y + atlas_info.rect.height * (1.0f - _fillAmount)) / tex_height);

		vertex[1] = Vector3(vertex[0].x, rect.y - rect.height, 0);
		uv[1] = Vector2(uv[0].x, (atlas_info.rect.y + atlas_info.rect.height) / tex_height);

		vertex[2] = Vector3(rect.x + rect.width, vertex[1].y, 0);
		uv[2] = Vector2((atlas_info.rect.x + atlas_info.rect.width) / tex_width, uv[1].y);

		vertex[3] = Vector3(vertex[2].x, vertex[0].y, 0);
		uv[3] = Vector2(uv[2].x, uv[0].y);
	}
	else if (_fillOrigin == (int)UIImageOriginVertical::Top)
	{
		vertex[0] = Vector3(rect.x, rect.y, 0);
		uv[0] = Vector2(atlas_info.rect.x / tex_width, atlas_info.rect.y / tex_height);

		vertex[1] = Vector3(vertex[0].x, rect.y - rect.height * _fillAmount, 0);
		uv[1] = Vector2(uv[0].x, (atlas_info.rect.y + atlas_info.rect.height * _fillAmount) / tex_height);

		vertex[2] = Vector3(rect.x + rect.width, vertex[1].y, 0);
		uv[2] = Vector2((atlas_info.rect.x + atlas_info.rect.width) / tex_width, uv[1].y);

		vertex[3] = Vector3(vertex[2].x, vertex[0].y, 0);
		uv[3] = Vector2(uv[2].x, uv[0].y);
	}
	//position
	for (int i = 0; i < 4; ++i)vertex[i] = mat_world.TransformPoint(vertex[i]);
	primitive->AddVertexRange(vertex, 4);

	//uv
	primitive->AddTexcoord1Range(uv, 4);

	//color
	Vector<Color> colors(4, this->GetColor());
	primitive->AddColorRange(colors);

	//index
	uint indexes[] = { uint(vxt_offset + 0) ,uint(vxt_offset + 1) ,uint(vxt_offset + 2) ,uint(vxt_offset + 0) ,uint(vxt_offset + 2) ,uint(vxt_offset + 3) };
	primitive->AddIndexRange(indexes, 6);

	//submesh
	vxt_offset += 4;
	idx_offset += 6;
	 
	return true;
}
void UIImage::SetNativeSize()
{
	UIAtlasInfo info;
	if (UIAtlasManager::GetAtlasInfo(_atlasFile, _imageName, info))
	{
		SetSize(info.rect.width, info.rect.height);
	}
}
void UIImage::SetAtlas(const String& file)
{
	_atlasFile = file;
	this->SetNeedRebuild();
}
void UIImage::SetAtlas(const String& file, const String& name)
{
	_atlasFile = file;
	_imageName = name;
	this->SetNeedRebuild();
}
String UIImage::GetAtlasFile()
{ 
	if (!_atlasFile.IsEmpty())
		return _atlasFile;
	else
		return "";
}
void UIImage::OnDrawEditor()
{
	base::OnDrawEditor();

	UIImage::OnDrawEditor_Image();
	UIView::OnDrawEditor_Color();
	UIImage::OnDrawEditor_ImageType();
	UIView::OnDrawEditor_RaycastTarget();
}	
void UIImage::OnDrawEditor_Image()
{
	{
		ImGuiEx::Label("Source Altas");
		const String& atlas_file = GetAtlasFile();
		const char* sz_name = atlas_file.c_str();
		ImGui::Button(sz_name, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
		if (ImGui::IsItemClicked(0) && !atlas_file.IsEmpty())
		{
			EMain_Project::SetSelectFile(atlas_file);
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
			{
				const String& file_path = EditorCursor::GetDragFile();
				if (UIAtlasManager::IsAtlasFile(file_path))
				{
					this->SetAtlas(file_path);
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
				this->SetAtlas(file);
			};

			EAssetSelect::Search(ResourceType::SpriteAtlas, OnSelectAsset);
		}
	}
	{
		int select_index = -1;
		VecString altas_names;
		UIAtlas* ui_atlas = UIAtlasManager::GetAtlas(_atlasFile);
		if (ui_atlas)
		{
			const Map<String, UIAtlasInfo>& atlas_infos = ui_atlas->GetAtlasInfoes();
			for (auto obj : atlas_infos)
			{
				altas_names.Add(obj.first);
				if (obj.first == _imageName)
				{
					select_index = altas_names.Size() - 1;
				}
			}
		}

		ImGuiEx::Label("Source Image");
		if (ECommonComponent::ShowDropDown("Source Image", altas_names, select_index))
		{
			SetAtlas(_atlasFile, altas_names[select_index]);
		}
	}
}
void UIImage::OnDrawEditor_ImageType()
{
	ImVec2 start_pos;
	float width;
	{
		ImGuiEx::Label("Image Type");
		start_pos = ImGui::GetCursorScreenPos();
		width = ImGui::GetContentRegionAvail().x;
		const char* values[] = { "Simple", "Sliced", "Filled" };
		int select_index = (int)_imageType;
		if (ImGui::Combo("##Image Type", &select_index, values, ARRAY_SIZE(values)))
		{
			SetImageType((UIImageType)select_index);
		}
	}
	switch (_imageType)
	{
	case UIImageType::Simple:
	{
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x, ImGui::GetCursorScreenPos().y));
		if (ImGui::Button("SetNativeSize", ImVec2(width, 0.0f)))
		{
			this->SetNativeSize();
		}
		break;
	}
	case UIImageType::Sliced:
	{
		ImGuiEx::Label("    Border");

		ImGui::SetCursorScreenPos(ImVec2(start_pos.x, ImGui::GetCursorScreenPos().y));
		ImGui::TextUnformatted("L");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(width*0.5f - ImGui::CalcTextSize("L").x * 3.0f);
		int l = (int)_imageBorder.x;
		if (ImGui::DragInt("##L", &l, 0.1f, 0, (int)GetSize().width))
		{
			SetImageBorder(Vector4(l, _imageBorder.y, _imageBorder.z, _imageBorder.w));
		}

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x + width * 0.5f, ImGui::GetCursorScreenPos().y));
		ImGui::TextUnformatted("T");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(width*0.5f - ImGui::CalcTextSize("L").x * 3.0f);
		int t = (int)_imageBorder.y;
		if (ImGui::DragInt("##T", &t, 0.1f, 0, (int)GetSize().width))
		{
			SetImageBorder(Vector4(_imageBorder.x, t, _imageBorder.z, _imageBorder.w));
		}

		ImGui::SetCursorScreenPos(ImVec2(start_pos.x, ImGui::GetCursorScreenPos().y));
		ImGui::TextUnformatted("R");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(width*0.5f - ImGui::CalcTextSize("L").x * 3.0f);
		int r = (int)_imageBorder.z;
		if (ImGui::DragInt("##R", &r, 0.1f, 0, (int)GetSize().height))
		{
			SetImageBorder(Vector4(_imageBorder.x, _imageBorder.y, r, _imageBorder.w));
		}

		ImGui::SameLine();
		ImGui::SetCursorScreenPos(ImVec2(start_pos.x + width * 0.5f, ImGui::GetCursorScreenPos().y));
		ImGui::TextUnformatted("B");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(width*0.5f - ImGui::CalcTextSize("L").x * 3.0f);
		int b = (int)_imageBorder.w;
		if (ImGui::DragInt("##B", &b, 0.1f, 0, (int)GetSize().height))
		{
			SetImageBorder(Vector4(_imageBorder.x, _imageBorder.y, _imageBorder.z, b));
		}
		break;
	}
	case UIImageType::Filled:
	{
		{
			ImGuiEx::Label("    Fill Method");
			const char* values[] = { "Horizontal", "Vertical" };
			int select_index = (int)_fillMethod;
			if (ImGui::Combo("##Fill Method", &select_index, values, ARRAY_SIZE(values)))
			{
				SetFillMethod((UIImageFillMethod)select_index, _fillOrigin);
			}
		}
		{
			ImGuiEx::Label("    Fill Origin");
			switch (_fillMethod)
			{
			case UIImageFillMethod::Horizontal:
			{
				const char* values[] = { "Left", "Right" };
				int select_index = (int)_fillOrigin;
				if (ImGui::Combo("##Fill Origin", &select_index, values, ARRAY_SIZE(values)))
				{
					SetFillMethod(_fillMethod, select_index);
				}
				break;
			}
			case UIImageFillMethod::Vertical:
			{
				const char* values[] = { "Bottom", "Top" };
				int select_index = (int)_fillOrigin;
				if (ImGui::Combo("##Fill Origin", &select_index, values, ARRAY_SIZE(values)))
				{
					SetFillMethod(_fillMethod, select_index);
				}
				break;
			}
			}
		}
		{
			ImGuiEx::Label("    Fill Amount");
			if (ImGui::SliderFloat("Fill Amount", &_fillAmount, 0, 1, "%.3f"))
			{
				SetFillAmount(_fillAmount);
			}
		}
		break;
	}
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UIImage);
template<class TransferFunction> inline
void UIImage::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	String AtlasFile = "";
	if (!transfer.IsRead())
	{
		AssetMeta* meta = AssetsManager::GetMetaByFile(_atlasFile);
		if (meta)AtlasFile = meta->GetGUID();
	}

	TRANSFER_SIMPLE(AtlasFile);
	TRANSFER_SIMPLE(_imageName);
	TRANSFER_ENUM(_imageType);
	TRANSFER_SIMPLE(_imageBorder);
	TRANSFER_ENUM(_fillMethod);
	TRANSFER_SIMPLE(_fillOrigin);
	TRANSFER_SIMPLE(_fillAmount);

	if (transfer.IsRead())
	{
		AssetMeta* meta = AssetsManager::GetMeta(AtlasFile);
		if (meta)_atlasFile = meta->GetFullPath();

		SetAtlas(_atlasFile, _imageName);
	}
}
DC_END_NAMESPACE
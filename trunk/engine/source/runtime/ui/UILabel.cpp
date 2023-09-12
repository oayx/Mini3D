#include "UILabel.h"
#include "UIAtlas.h"
#include "runtime/component/Component.inl"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/font/FontAtlas.h"
#include "runtime/font/FontManager.h"
#include "runtime/resources/AssetsManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UILabel, UIView);
UILabel::UILabel()
{
	_font = FontManager::GetDefaultFontName();
}
UILabel::~UILabel()
{
}
Object* UILabel::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UILabel* obj = dynamic_cast<UILabel*>(new_obj);
	if (!obj)return new_obj;

	obj->SetText(_utf8Text);
	obj->SetAlignment(_alignment);
	obj->SetFont(_font);
	obj->SetFontSize(_fontSize);
	obj->SetFontSize(_fontSize);
	obj->SetWordSpace(_wordSpace);
	obj->SetLineSpace(_lineSpace);
	obj->SetHorizontalOverflow(_horizontalOverflow);
	obj->SetVerticalOverflow(_verticalOverflow);

	return obj;
}
bool UILabel::FillMesh(VariablePrimitive* primitive, Material* material, int& vxt_offset, int& idx_offset)
{
	if (_utf8Text.IsEmpty())
	{
		_contentRealSize.Set(0.0f, 0.0f);
		return true;
	}

	Vector<char32_t> chs = _utf8Text.ToUnicode32();
	UITextOutline* outline = this->GetComponent<UITextOutline>();
	UITextShadow* shadow = this->GetComponent<UITextShadow>();

	//预先分配内存
	int per_vertex = 1;
	Vector<Vector3> vertexs;
	Vector<Color> colors;
	Vector<Vector2> texcoords;
	Vector<uint> indices;
	if (outline && outline->IsEnable())per_vertex += 4;	//描边需要多4倍顶点
	if (shadow && shadow->IsEnable())per_vertex++;		//阴影需要多1倍顶点
	vertexs.Reserve(vertexs.Size() + chs.Size() * 4 * per_vertex);
	colors.Reserve(vertexs.Size() + chs.Size() * 4 * per_vertex);
	texcoords.Reserve(vertexs.Size() + chs.Size() * 4 * per_vertex);
	indices.Reserve(vertexs.Size() + chs.Size() * 6 * per_vertex);

	uint start_vertex_count = (uint)vertexs.Size();
	bool bold = (_fontStyle & UIFontStyle::Bold);
	bool italic = (_fontStyle & UIFontStyle::Italic);
	material->SetTexture(FontAtlasManager::GetTexture(_font, _fontSize, bold, italic));

	float cursor_x = 0, cursor_y = 0;//当前写入位置
	float max_line_width = 0, max_line_height = _fontSize;//单行最大高度
	float total_width = 0, total_height = 0;//已经有的高度，用于计算高度上是否超出

	float max_height = 0, max_advance = 0;
	int last_line_index = 0;//最后一行的索引，用于计算最后一行的左右对齐
	float last_line_width = 0;
	Size content_size = GetSize();
	for (int i = 0; i < chs.Size(); ++i)
	{
		char32_t ch = chs[i];
		if (ch == u'\n')
		{//换行
			cursor_x = 0;
			cursor_y = cursor_y - (max_line_height + _lineSpace);
			total_height += (max_line_height + _lineSpace);
			max_line_width = 0;
			last_line_index = i;
			last_line_width = 0;
			continue;
		}
		FontAtlasInfo atlas_info;
		FontAtlasManager::GetFont(_font, ch, _fontSize, bold, italic, atlas_info);
		float font_w = atlas_info.rect.width, font_h = atlas_info.rect.height;
		bool is_overflow = false;
		max_height = Math::Max<float>(max_height, font_h);
		max_advance = Math::Max<float>(max_advance, font_h - atlas_info.bearing_y);

		//cal position
		if (_horizontalOverflow == UIHorizontalWrapMode::Wrap)
		{
			if (cursor_x + atlas_info.advance_x > content_size.width)
			{
				if (total_height + 2 * max_line_height + _lineSpace < content_size.height)
				{//可以换行
					cursor_x = 0;
					cursor_y = cursor_y - (max_line_height + _lineSpace);
					total_height += (max_line_height + _lineSpace);
					max_line_width = 0;
					last_line_index = i;
					last_line_width = 0;
				}
				else
				{
					if (_verticalOverflow == UIVerticalWrapMode::Overflow)
					{//可以换行
						cursor_x = 0;
						cursor_y = cursor_y - (max_line_height + _lineSpace);
						total_height += (max_line_height + _lineSpace);
						max_line_width = 0;
						last_line_index = i;
						last_line_width = 0;
					}
					else
					{
						is_overflow = true;
						break;
					}
				}
			}
		}
		if (is_overflow)
		{//裁剪
			break;
		}

		//position
		float x0 = cursor_x + atlas_info.bearing_x;
		float y0 = cursor_y + atlas_info.bearing_y - _fontSize * 0.85f;
		float x1 = cursor_x + atlas_info.bearing_x + font_w;
		float y1 = cursor_y + atlas_info.bearing_y - _fontSize * 0.85f - font_h;

		//uv
		const Rect& uv_rect = atlas_info.uv_rect;
		Vector2 uv1 = { uv_rect.x,uv_rect.y };
		Vector2 uv2 = { uv_rect.x + uv_rect.width,uv_rect.y };
		Vector2 uv3 = { uv_rect.x + uv_rect.width,uv_rect.y + uv_rect.height };
		Vector2 uv4 = { uv_rect.x,uv_rect.y + uv_rect.height };

		if (outline && outline->IsEnable())
		{//描边
			Vector3 offsets[4];
			offsets[0] = Vector3(-outline->distance_x,  outline->distance_y, 0.0f);
			offsets[1] = Vector3( outline->distance_x,  outline->distance_y, 0.0f);
			offsets[2] = Vector3( outline->distance_x, -outline->distance_y, 0.0f);
			offsets[3] = Vector3(-outline->distance_x, -outline->distance_y, 0.0f);

			for (int j = 0; j < 4; ++j)
			{
				//position
				vertexs.Add(Vector3(x0, y0, 0) + offsets[j]);
				vertexs.Add(Vector3(x1, y0, 0) + offsets[j]);
				vertexs.Add(Vector3(x1, y1, 0) + offsets[j]);
				vertexs.Add(Vector3(x0, y1, 0) + offsets[j]);

				//uv
				const Rect& uv_rect = atlas_info.uv_rect;
				texcoords.Add(uv1);
				texcoords.Add(uv2);
				texcoords.Add(uv3);
				texcoords.Add(uv4);

				//color
				colors.Add(outline->color);
				colors.Add(outline->color);
				colors.Add(outline->color);
				colors.Add(outline->color);

				//index
				indices.Add(vxt_offset + 0);
				indices.Add(vxt_offset + 1);
				indices.Add(vxt_offset + 2);
				indices.Add(vxt_offset + 0);
				indices.Add(vxt_offset + 2);
				indices.Add(vxt_offset + 3);

				//submesh
				vxt_offset += 4;
				idx_offset += 6;
			}
		}
		if (shadow && shadow->IsEnable())
		{//阴影
			Vector3 offset = Vector3(shadow->distance_x, shadow->distance_y, 0);

			//position
			vertexs.Add(Vector3(x0, y0, 0.0f) + offset);
			vertexs.Add(Vector3(x1, y0, 0.0f) + offset);
			vertexs.Add(Vector3(x1, y1, 0.0f) + offset);
			vertexs.Add(Vector3(x0, y1, 0.0f) + offset);

			//uv
			const Rect& uv_rect = atlas_info.uv_rect;
			texcoords.Add(uv1);
			texcoords.Add(uv2);
			texcoords.Add(uv3);
			texcoords.Add(uv4);

			//color
			colors.Add(shadow->color);
			colors.Add(shadow->color);
			colors.Add(shadow->color);
			colors.Add(shadow->color);

			//index
			indices.Add(vxt_offset + 0);
			indices.Add(vxt_offset + 1);
			indices.Add(vxt_offset + 2);
			indices.Add(vxt_offset + 0);
			indices.Add(vxt_offset + 2);
			indices.Add(vxt_offset + 3);

			//submesh
			vxt_offset += 4;
			idx_offset += 6;
		}
		{//本身
			vertexs.Add(Vector3(x0, y0, 0));
			vertexs.Add(Vector3(x1, y0, 0));
			vertexs.Add(Vector3(x1, y1, 0));
			vertexs.Add(Vector3(x0, y1, 0));

			//uv
			texcoords.Add(uv1);
			texcoords.Add(uv2);
			texcoords.Add(uv3);
			texcoords.Add(uv4);

			//color
			colors.Add(this->GetColor());
			colors.Add(this->GetColor());
			colors.Add(this->GetColor());
			colors.Add(this->GetColor());

			//index
			indices.Add(vxt_offset + 0);
			indices.Add(vxt_offset + 1);
			indices.Add(vxt_offset + 2);
			indices.Add(vxt_offset + 0);
			indices.Add(vxt_offset + 2);
			indices.Add(vxt_offset + 3);

			//submesh
			vxt_offset += 4;
			idx_offset += 6;
		}
		cursor_x += atlas_info.advance_x + _wordSpace;
		max_line_width += atlas_info.advance_x;
		last_line_width += atlas_info.advance_x;
		total_width = Math::Max<float>(total_width, max_line_width);
	}
	total_height += max_line_height;
	_contentRealSize.Set(total_width, total_height);

	//变换
	float alignment_x = 0, alignment_y = 0;
	switch (_alignment)
	{
	case UITextAnchor::UpperLeft:
		alignment_x = -content_size.width * 0.5f;
		alignment_y = content_size.height * 0.5f;
		break;
	case UITextAnchor::UpperCenter:
		alignment_x = -content_size.width * 0.5f + (content_size.width - total_width)*0.5f;
		alignment_y = content_size.height * 0.5f;
		break;
	case UITextAnchor::UpperRight:
		alignment_x = -content_size.width * 0.5f + (content_size.width - total_width);
		alignment_y = content_size.height * 0.5f;
		break;
	case UITextAnchor::MiddleLeft:
		alignment_x = -content_size.width * 0.5f;
		alignment_y = total_height * 0.5f;
		break;
	case UITextAnchor::MiddleCenter:
		alignment_x = -content_size.width * 0.5f + (content_size.width - total_width)*0.5f;
		alignment_y = total_height * 0.5f;
		break;
	case UITextAnchor::MiddleRight:
		alignment_x = -content_size.width * 0.5f + (content_size.width - total_width);
		alignment_y = total_height * 0.5f;
		break;
	case UITextAnchor::LowerLeft:
		alignment_x = -content_size.width * 0.5f;
		alignment_y = -content_size.height * 0.5f + total_height;
		break;
	case UITextAnchor::LowerCenter:
		alignment_x = -content_size.width * 0.5f + (content_size.width - total_width)*0.5f;
		alignment_y = -content_size.height * 0.5f + total_height;
		break;
	case UITextAnchor::LowerRight:
		alignment_x = -content_size.width * 0.5f + (content_size.width - total_width);
		alignment_y = -content_size.height * 0.5f + total_height;
		break;
	}

	Vector3 alignment_pos = Vector3(alignment_x, alignment_y, 0);
	const Matrix4& mat_world = this->GetTransform()->GetLocalToWorldMatrix();
	for (uint i = start_vertex_count; i < vertexs.Size(); ++i)
	{
		Vector3 pos = vertexs[i];
		vertexs[i] = mat_world.TransformPoint(pos + alignment_pos);
	}

	//针对最后一行对齐(需要有多行)
	if (last_line_index != 0 && last_line_index < chs.Size() && _horizontalOverflow != UIHorizontalWrapMode::Overflow)
	{
		alignment_x = 0;
		switch (_alignment)
		{
		case UITextAnchor::UpperLeft:
		case UITextAnchor::MiddleLeft:
		case UITextAnchor::LowerLeft:
			//undo
			break;
		case UITextAnchor::UpperCenter:
		case UITextAnchor::MiddleCenter:
		case UITextAnchor::LowerCenter:
			alignment_x = (total_width - last_line_width)*0.5f;
			break;
		case UITextAnchor::UpperRight:
		case UITextAnchor::MiddleRight:
		case UITextAnchor::LowerRight:
			alignment_x = total_width - last_line_width;
			break;
		}
		alignment_pos = Vector3(alignment_x, 0, 0);
		alignment_pos = mat_world.TransformPoint(alignment_pos);
		int per_vertex_size = 4;
		if (outline && outline->IsEnable())per_vertex_size += 4 * 4;
		if (shadow && shadow->IsEnable())per_vertex_size += 4;
		for (uint i = start_vertex_count + last_line_index * per_vertex_size; i < vertexs.Size(); ++i)
		{
			Vector3 pos = vertexs[i];
			vertexs[i] = pos + alignment_pos;
		}
	}
	primitive->AddVertexRange(vertexs);
	primitive->AddColorRange(colors);
	primitive->AddTexcoord1Range(texcoords);
	primitive->AddIndexRange(indices);

	return true;
}
String UILabel::GetAtlasFile()
{
	bool bold = (_fontStyle & UIFontStyle::Bold);
	bool italic = (_fontStyle & UIFontStyle::Italic);
	return FontAtlasManager::GetAtlasKey(_font, _fontSize, bold, italic);
}
void UILabel::OnDrawEditor()
{
	base::OnDrawEditor();
	{
		ImGui::TextUnformatted("Text");
		char text_buff[1024 * 16] = { 0 };
		Memory::Copy(text_buff, _utf8Text.c_str(), _utf8Text.Size());
		if (ImGui::InputTextMultiline("##Text", text_buff, ARRAY_SIZE(text_buff), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 3)))
		{
			SetText(text_buff);
		}
	}
	{
		ImGui::TextUnformatted("Character");
		{
			int select_index = -1;
			VecString font_names;
			const Vector<AssetMeta*>& font_files = AssetsManager::GetAssetGroup(ResourceType::Font);
			for(int i = 0; i < font_files.Size(); ++i)
			{
				FontMeta* meta = dynamic_cast<FontMeta*>(font_files[i]);
				if (meta)
				{
					font_names.Add(meta->GetFontName());
					if (meta->GetFontName().Equals(_font, true))
						select_index = i;
				}
			}

			ImGuiEx::Label("    Font");
			if (ECommonComponent::ShowDropDown("FontType", font_names, select_index))
			{
				SetFont(font_names[select_index]);
			}
		}
		{
			ImGuiEx::Label("    Font Style");
			const char* values[] = { "Normal", "Bold", "Italic", "Bold And Italic" };
			int select_index = (int)_fontStyle;
			if (ImGui::Combo("##Font Style", &select_index, values, ARRAY_SIZE(values)))
			{
				SetFontStyle((UIFontStyle)select_index);
			}
		}
		{
			ImGuiEx::Label("    Font Size");
			int font_size = _fontSize;
			if (ImGui::DragInt("##Font Size", &font_size, 0.1f, 0, MAX_FONT_SIZE))
			{
				SetFontSize((ushort)font_size);
			}
		}
		{
			ImGuiEx::Label("    Line Space");
			if (ImGui::DragFloat("##Line Space", &_lineSpace, 0.01f, MIN_float, MAX_float))
			{
				SetLineSpace(_lineSpace);
			}
		}
	}
	{
		ImGui::TextUnformatted("Paragraph");
		{
			ImGuiEx::Label("    Alignment");
			float width = ImGui::CalcTextSize(ICON_FA_ARROW_LEFT).x;
			if (ImGui::Selectable(ICON_FA_ARROW_LEFT, _alignment == UITextAnchor::UpperLeft || _alignment == UITextAnchor::MiddleLeft || _alignment == UITextAnchor::LowerLeft, 0, ImVec2(width, 0.0f)))
			{
				switch (_alignment)
				{
				case UITextAnchor::UpperCenter: SetAlignment(UITextAnchor::UpperLeft); break;
				case UITextAnchor::UpperRight: SetAlignment(UITextAnchor::UpperLeft); break;
				case UITextAnchor::MiddleCenter: SetAlignment(UITextAnchor::MiddleLeft); break;
				case UITextAnchor::MiddleRight: SetAlignment(UITextAnchor::MiddleLeft); break;
				case UITextAnchor::LowerCenter: SetAlignment(UITextAnchor::LowerLeft); break;
				case UITextAnchor::LowerRight: SetAlignment(UITextAnchor::LowerLeft); break;
				default: break;
				}
			}
			ImGui::SameLine();
			if (ImGui::Selectable(ICON_FA_ARROWS_ALT_H, _alignment == UITextAnchor::UpperCenter || _alignment == UITextAnchor::MiddleCenter || _alignment == UITextAnchor::LowerCenter, 0, ImVec2(width, 0.0f)))
			{
				switch (_alignment)
				{
				case UITextAnchor::UpperLeft: SetAlignment(UITextAnchor::UpperCenter); break;
				case UITextAnchor::UpperRight: SetAlignment(UITextAnchor::UpperCenter); break;
				case UITextAnchor::MiddleLeft: SetAlignment(UITextAnchor::MiddleCenter); break;
				case UITextAnchor::MiddleRight: SetAlignment(UITextAnchor::MiddleCenter); break;
				case UITextAnchor::LowerLeft: SetAlignment(UITextAnchor::LowerCenter); break;
				case UITextAnchor::LowerRight: SetAlignment(UITextAnchor::LowerCenter); break;
				default: break;
				}
			}
			ImGui::SameLine();
			if (ImGui::Selectable(ICON_FA_ARROW_RIGHT, _alignment == UITextAnchor::UpperRight || _alignment == UITextAnchor::MiddleRight || _alignment == UITextAnchor::LowerRight, 0, ImVec2(width, 0.0f)))
			{
				switch (_alignment)
				{
				case UITextAnchor::UpperLeft: SetAlignment(UITextAnchor::UpperRight); break;
				case UITextAnchor::UpperCenter: SetAlignment(UITextAnchor::UpperRight); break;
				case UITextAnchor::MiddleLeft: SetAlignment(UITextAnchor::MiddleRight); break;
				case UITextAnchor::MiddleCenter: SetAlignment(UITextAnchor::MiddleRight); break;
				case UITextAnchor::LowerLeft: SetAlignment(UITextAnchor::LowerRight); break;
				case UITextAnchor::LowerCenter: SetAlignment(UITextAnchor::LowerRight); break;
				default: break;
				}
			}

			ImGui::SameLine(0.0f, 20.0f);
			if (ImGui::Selectable(ICON_FA_ARROW_UP, _alignment == UITextAnchor::UpperLeft || _alignment == UITextAnchor::UpperCenter || _alignment == UITextAnchor::UpperRight, 0, ImVec2(width, 0.0f)))
			{
				switch (_alignment)
				{
				case UITextAnchor::MiddleLeft:SetAlignment(UITextAnchor::UpperLeft); break;
				case UITextAnchor::MiddleCenter: SetAlignment(UITextAnchor::UpperCenter); break;
				case UITextAnchor::MiddleRight: SetAlignment(UITextAnchor::UpperRight); break;
				case UITextAnchor::LowerLeft: SetAlignment(UITextAnchor::UpperLeft); break;
				case UITextAnchor::LowerCenter: SetAlignment(UITextAnchor::UpperCenter); break;
				case UITextAnchor::LowerRight: SetAlignment(UITextAnchor::UpperRight); break;
				default: break;
				}
			}
			ImGui::SameLine();
			if (ImGui::Selectable(ICON_FA_ARROWS_ALT_V, _alignment == UITextAnchor::MiddleLeft || _alignment == UITextAnchor::MiddleCenter || _alignment == UITextAnchor::MiddleRight, 0, ImVec2(width, 0.0f)))
			{
				switch (_alignment)
				{
				case UITextAnchor::UpperLeft: SetAlignment(UITextAnchor::MiddleLeft); break;
				case UITextAnchor::UpperCenter: SetAlignment(UITextAnchor::MiddleCenter); break;
				case UITextAnchor::UpperRight: SetAlignment(UITextAnchor::MiddleRight);  break;
				case UITextAnchor::LowerLeft: SetAlignment(UITextAnchor::MiddleLeft); break;
				case UITextAnchor::LowerCenter: SetAlignment(UITextAnchor::MiddleCenter); break;
				case UITextAnchor::LowerRight: SetAlignment(UITextAnchor::MiddleRight); break;
				default: break;
				}
			}
			ImGui::SameLine();
			if (ImGui::Selectable(ICON_FA_ARROW_DOWN, _alignment == UITextAnchor::LowerLeft || _alignment == UITextAnchor::LowerCenter || _alignment == UITextAnchor::LowerRight, 0, ImVec2(width, 0.0f)))
			{
				switch (_alignment)
				{
				case UITextAnchor::UpperLeft: SetAlignment(UITextAnchor::LowerLeft); break;
				case UITextAnchor::UpperCenter: SetAlignment(UITextAnchor::LowerCenter); break;
				case UITextAnchor::UpperRight: SetAlignment(UITextAnchor::LowerRight); break;
				case UITextAnchor::MiddleLeft:SetAlignment(UITextAnchor::LowerLeft); break;
				case UITextAnchor::MiddleCenter: SetAlignment(UITextAnchor::LowerCenter); break;
				case UITextAnchor::MiddleRight: SetAlignment(UITextAnchor::LowerRight); break;
				default: break;
				}
			}
		}
		{
			ImGuiEx::Label("    Horizontal Overflow");
			const char* values[] = { "Wrap", "Overflow" };
			int select_index = (int)_horizontalOverflow;
			if (ImGui::Combo("##HorizontalOverflow", &select_index, values, ARRAY_SIZE(values)))
			{
				SetHorizontalOverflow((UIHorizontalWrapMode)select_index);
			}
		}
		{
			ImGuiEx::Label("    Vertical Overflow");
			const char* values[] = { "Truncate", "Overflow" };
			int select_index = (int)_verticalOverflow;
			if (ImGui::Combo("##VerticalOverflow", &select_index, values, ARRAY_SIZE(values)))
			{
				SetVerticalOverflow((UIVerticalWrapMode)select_index);
			}
		}
	}

	ImGui::NewLine();
	UIView::OnDrawEditor_Color();
	UIView::OnDrawEditor_RaycastTarget();
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UILabel);
template<class TransferFunction> inline
void UILabel::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
	
	TRANSFER_SIMPLE(_utf8Text);
	TRANSFER_ENUM(_alignment);
	TRANSFER_SIMPLE(_font);
	TRANSFER_ENUM(_fontStyle);
	TRANSFER_SIMPLE(_fontSize);
	TRANSFER_SIMPLE(_wordSpace);
	TRANSFER_SIMPLE(_lineSpace);
	TRANSFER_ENUM(_horizontalOverflow);
	TRANSFER_ENUM(_verticalOverflow);

	if (transfer.IsRead())
	{
		SetText(_utf8Text);
		SetAlignment(_alignment);
		SetFont(_font);
		SetFontStyle(_fontStyle);
		SetFontSize(_fontSize);
		SetWordSpace(_wordSpace);
		SetLineSpace(_lineSpace);
		SetHorizontalOverflow(_horizontalOverflow);
		SetVerticalOverflow(_verticalOverflow);
	}
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UITextShadow, Component);
Object* UITextShadow::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UITextShadow* obj = dynamic_cast<UITextShadow*>(new_obj);
	if (!obj)return new_obj;

	obj->SetColor(color);
	obj->SetDistance(distance_x, distance_y);

	return obj;
}
void UITextShadow::Awake()
{
	base::Awake();
	this->SetNeedRebuild();
}
bool UITextShadow::CanAdd(GameObject* object)
{
	if (!object)return false;
	UIView* view = object->GetComponent<UILabel>();
	return view != nullptr;
}
void UITextShadow::OnDestroy()
{
	this->SetNeedRebuild();
}
void UITextShadow::SetNeedRebuild()
{
	UIView* view = this->GetComponent<UILabel>();
	if (view)view->SetNeedRebuild();
}
void UITextShadow::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("Effect Color");
	if (ECommonComponent::ShowColor("Effect Color", color.p, false))
	{
		this->SetColor(color);
	}

	ImGuiEx::Label("Effect Distance");
	const float width = ImGui::GetContentRegionAvail().x;
	const float char_width = ImGui::GetFontSize() + 2.0f;

	ImGui::SetNextItemWidth(char_width);
	ImGui::Text("X  ");

	float min_v = MIN_float;
	float max_v = MAX_float;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width * 0.5f - char_width);
	if (ImGui::DragScalar("##X", ImGuiDataType_Float, &distance_x, 0.01f, &min_v, &max_v, "%.2f"))
	{
		SetDistance(distance_x, distance_y);
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(char_width);
	ImGui::TextUnformatted("Y");

	ImGui::SameLine();
	ImGui::SetNextItemWidth(width * 0.5f - char_width);
	if (ImGui::DragScalar("##Y", ImGuiDataType_Float, &distance_y, 0.01f, &min_v, &max_v, "%.2f"))
	{
		SetDistance(distance_x, distance_y);
	}
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UITextShadow);
template<class TransferFunction> inline
void UITextShadow::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(color);
	TRANSFER_SIMPLE(distance_x);
	TRANSFER_SIMPLE(distance_y);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UITextOutline, Component);
Object* UITextOutline::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UITextOutline* obj = dynamic_cast<UITextOutline*>(new_obj);
	if (!obj)return new_obj;

	obj->SetColor(color);
	obj->SetDistance(distance_x, distance_y);

	return obj;
}
void UITextOutline::Awake()
{
	base::Awake();
	this->SetNeedRebuild();
}
bool UITextOutline::CanAdd(GameObject* object)
{
	if (!object)return false;
	UIView* view = object->GetComponent<UILabel>();
	return view != nullptr;
}
void UITextOutline::OnDestroy()
{
	this->SetNeedRebuild();
}
void UITextOutline::SetNeedRebuild()
{
	UIView* view = this->GetComponent<UILabel>();
	if (view)view->SetNeedRebuild();
}
void UITextOutline::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("Effect Color");
	if (ECommonComponent::ShowColor("Effect Color", color.p, false))
	{
		this->SetColor(color);
	}

	ImGuiEx::Label("Effect Distance");
	const float width = ImGui::GetContentRegionAvail().x;
	const float char_width = ImGui::GetFontSize() + 2.0f;

	ImGui::SetNextItemWidth(char_width);
	ImGui::Text("X  ");

	float min_v = MIN_float;
	float max_v = MAX_float;
	ImGui::SameLine();
	ImGui::SetNextItemWidth(width * 0.5f - char_width);
	if (ImGui::DragScalar("##X", ImGuiDataType_Float, &distance_x, 0.01f, &min_v, &max_v, "%.2f"))
	{
		SetDistance(distance_x, distance_y);
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(char_width);
	ImGui::TextUnformatted("Y");

	ImGui::SameLine();
	ImGui::SetNextItemWidth(width * 0.5f - char_width);
	if (ImGui::DragScalar("##Y", ImGuiDataType_Float, &distance_y, 0.01f, &min_v, &max_v, "%.2f"))
	{
		SetDistance(distance_x, distance_y);
	}
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UITextOutline);
template<class TransferFunction> inline
void UITextOutline::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(color);
	TRANSFER_SIMPLE(distance_x);
	TRANSFER_SIMPLE(distance_y);
}
DC_END_NAMESPACE
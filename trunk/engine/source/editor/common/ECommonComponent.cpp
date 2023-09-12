#include "ECommonComponent.h"
#include "runtime/mesh/Mesh.h"
#include "runtime/mesh/MeshRender.h"
#include "runtime/particle/ParticleRender.h"
#include "runtime/particle/ParticleSystem.h"
#include "runtime/renderable/Sprite.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/scene/Light.h"
#include "runtime/terrain/Terrain.h"
#include "runtime/ui/GUI.h"
#include "runtime/ui/UICanvas.h"
#include "runtime/ui/UICanvasScaler.h"
#include "runtime/ui/UICheckbox.h"
#include "runtime/ui/UIImage.h"
#include "runtime/ui/UIButton.h"
#include "runtime/ui/UIInputField.h"
#include "runtime/ui/UILabel.h"
#include "runtime/ui/UINormalButton.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorDefine.h"
#include "external/imgui/imgui_internal.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void ECommonComponent::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
//样式选择
bool ECommonComponent::ShowStyleSelector(const char* label)
{
	static int style_idx = -1;
	if (ImGui::Combo(label, &style_idx, "Dark\0Light\0Classic\0"))
	{
		switch (style_idx)
		{
		case 0: ImGui::StyleColorsDark(); break;
		case 1: ImGui::StyleColorsLight(); break;
		case 2: ImGui::StyleColorsClassic(); break;
		}
		return true;
	}
	return false;
}
void ECommonComponent::ShowFontSelector(const char* label)
{
	ImGuiIO& io = ImGui::GetIO();
	ImFont* font_current = ImGui::GetFont();
	if (ImGui::BeginCombo(label, font_current->GetDebugName()))
	{
		for (int n = 0; n < io.Fonts->Fonts.Size; n++)
		{
			ImFont* font = io.Fonts->Fonts[n];
			ImGui::PushID((void*)font);
			if (ImGui::Selectable(font->GetDebugName(), font == font_current))
				io.FontDefault = font;
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	HelpMarker(
		"- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
		"- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
		"- Read FAQ and docs/FONTS.md for more details.\n"
		"- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");
}
bool ECommonComponent::ShowColor(const char* label, float* init_color, bool hdr)
{
	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0);
	return ImGui::ColorEdit4(label, init_color, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview | misc_flags);
}
bool ECommonComponent::ShowCullmask(const char* label, uint& cullmask)
{
	uint all_mask = LayerMask::GetAllMask();
	String label_name = "Mixd ...";
	if (cullmask == 0xffffffff || cullmask == all_mask)label_name = "Everything";
	else if (cullmask == 0)label_name = "Nothing";

	ImVec2 popup_start = ImGui::GetCursorScreenPos();
	ImVec2 button_size(ImGui::GetContentRegionAvail().x, 0);
	if (ImGui::Button(label_name.c_str(), button_size))
	{
		ImGui::OpenPopup("CullMaskPopup");
	}

	bool result = false;
	popup_start.y += ImGui::GetFrameHeight() + 5.0f;
	ImGui::SetNextWindowPos(popup_start);
	ImGui::SetNextWindowSize(ImVec2(button_size.x, 0.0f));
	if (ImGui::BeginPopup("CullMaskPopup", 0))
	{
		bool selected[32 + 2] = { false };
		if (cullmask == 0xffffffff || cullmask == all_mask)selected[1] = true;
		for (byte index = 2; index < 32 + 2; ++index)
		{
			selected[index] = cullmask & (1 << (index - 2));
		}
		if (ImGui::MenuItem("Nothing", "", &selected[0]))
		{
			cullmask = 0;
			result = true;
		}
		if (ImGui::MenuItem("Everything", "", &selected[1]))
		{
			cullmask = 0xffffffff;
			result = true;
		}

		for (int i = 0; i < 32; ++i)
		{
			const char* sz_name = LayerMask::LayerToName(i).c_str();
			if (sz_name[0] == '\0')continue;
			if (ImGui::MenuItem(sz_name, "", &selected[i + 2]))
			{
				if (selected[i + 2])
				{
					cullmask |= (1 << i);
				}
				else
				{
					cullmask ^= (1 << i);
				}
				result = true;
			}
		}

		ImGui::EndPopup();
	}
	return result;
}
bool ECommonComponent::ShowPopupList(const char* label, int& curr_select, const VecString& list)
{
	ImVec2 popup_start = ImGui::GetCursorScreenPos();
	ImVec2 button_size(ImGui::GetContentRegionAvail().x, 0);

	if (ImGui::Button(label, button_size))
	{
		ImGui::OpenPopup(label);
	}

	bool result = false;
	popup_start.y += ImGui::GetFrameHeight() + 5.0f;
	ImGui::SetNextWindowPos(popup_start);
	ImGui::SetNextWindowSize(ImVec2(button_size.x, 300.0f));
	if (ImGui::BeginPopup(label, 0))
	{
		static char filter[128] = "";
		const float w = ImGui::CalcTextSize(ICON_FA_TIMES).x + ImGui::GetStyle().ItemSpacing.x * 2.0f;
		ImGui::SetNextItemWidth(-w);
		ImGui::InputTextWithHint("##filter", "Filter", filter, sizeof(filter));
		ImGui::SameLine();
		if (ImGuiEx::IconButton(ICON_FA_TIMES, "Clear filter"))
		{
			filter[0] = '\0';
		}

		for(int i = 0; i < list.Size(); ++i)
		{
			const char* sz_name = list[i].c_str();
			if (filter[0] == '\0' || String::Stristr(sz_name, filter) != nullptr)
			{
				if (ImGui::Selectable(sz_name))
				{
					curr_select = i;
					result = true;
				}
			}
		}

		ImGui::EndPopup();
	}
	return result;
}
bool ECommonComponent::ShowFileIcon(const char* label, bool isSelected, Texture* icon, const Vector2& _size)
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiContext& g = *ImGui::GetCurrentContext();
	ImGuiWindow* window = g.CurrentWindow;

	float window_space = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size(_size.x, _size.y);
	bool ret = false;

	if (ImGui::InvisibleButton(label, size))
		ret = true;
	if (ImGui::IsItemClicked(1))
		ret = true;

	bool hovered = ImGui::IsItemHovered();
	bool active = ImGui::IsItemActive();
	bool double_click = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
	if (double_click && hovered)
		ret = true;

	float icon_size = size.y - g.FontSize * 2;
	float icon_pos_x = pos.x + (size.x - icon_size) / 2.0f;

	//裁剪文字
	ImVec2 textSize;
	int valid_size = Strlen(label);
	while (valid_size > 0)
	{
		textSize = ImGui::CalcTextSize(label, label + valid_size, true, size.x);
		if (textSize.x <= icon_size)break;
		valid_size -= 2;
	}
	char new_label[MAX_FILE_NAME] = { 0 };
	if (valid_size != Strlen(label))
	{//有裁剪
		Memory::Copy(new_label, label, valid_size);
		for (int i = 0; i < 3; ++i) { new_label[valid_size] = '.'; ++valid_size; }
		new_label[valid_size] = '\0';
		textSize = ImGui::CalcTextSize(new_label, 0, true, size.x);
	}
	else
	{
		Strncpy(new_label, label, valid_size);
	}

	window->DrawList->AddImage((ImTextureID)icon, ImVec2(icon_pos_x, pos.y), ImVec2(icon_pos_x + icon_size, pos.y + icon_size));

	ImVec2 txt_pos = ImVec2(pos.x + (size.x - textSize.x) / 2.0f, pos.y + icon_size);
	if (hovered || active || isSelected)
	{
		window->DrawList->AddRectFilled(ImVec2(txt_pos.x - 6, txt_pos.y + 2), ImVec2(txt_pos.x + textSize.x + 4, txt_pos.y + g.FontSize + 2),
			ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[active ? ImGuiCol_HeaderActive : (isSelected ? ImGuiCol_Header : ImGuiCol_HeaderHovered)]));
	}

	window->DrawList->AddText(g.Font, g.FontSize, ImVec2(pos.x + (size.x - textSize.x) / 2.0f, pos.y + icon_size),
		ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]), new_label);

	//是否换行
	float last_buttom_pos = ImGui::GetItemRectMax().x;
	float this_buttom_pos = last_buttom_pos + style.ItemSpacing.x + size.x;
	if (this_buttom_pos < window_space)
		ImGui::SameLine();

	return ret;
}
void ECommonComponent::ShowAddComponent(std::function<void(String)> action, const EComponents& parent_component)
{
	int id = 0;
	for (const auto& component : parent_component.Components)
	{
		ImGui::PushID(id++);
		const char* sz_name = component.second.c_str();
		const Type* type = Type::GetType(sz_name);
		if (type && type->HasFlag(TypeFlag::ShowInEditor))
		{
			if (ImGui::MenuItem(component.first.c_str()))
			{
				if (action)action(component.second);
			}
		}
		ImGui::PopID();
	}

	for (const auto& child : parent_component.Childrends)
	{
		ImGui::PushID(id++);
		const char* sz_name = child.first.c_str();
		if (ImGui::BeginMenu(sz_name))
		{
			ShowAddComponent(action, child.second);
			ImGui::EndMenu();
		}
		ImGui::PopID();
	}
}
void ECommonComponent::ShowCreateObject()
{
	//创建空对象
	auto OnCreateEmpty = []() -> GameObject*
	{
		Transform* parent_node = nullptr;
		if (EditorAppliction::GetInspectorId())
		{
			GameObject* obj = SceneManager::GetGameObject(EditorAppliction::GetInspectorId());
			if (obj)parent_node = obj->GetTransform();
		}
		GameObject* new_obj = GameObject::Instantiation("GameObject", parent_node);
		EditorAppliction::SetInspectorType(InspectorType::Hierarchy, new_obj->GetInstanceId());
		return new_obj;
	};

	//创建空UI对象
	auto OnCreateUIEmpty = []() -> GameObject*
	{
		if (SceneManager::GetUICanvasCount() == 0)
		{
			GameObject* ui_root = GameObject::InstantiationUI("Canvas");
			ui_root->GetTransform()->SetLocalScale(Vector3::one * 0.01f);
			Camera* camera = ui_root->AddComponent<Camera>();
			UICanvas* canvas = ui_root->AddComponent<UICanvas>();
			canvas->SetRenderMode(UIRenderMode::ScreenSpace_Camera);
			canvas->SetCamera(camera);
			camera->SetOrthographic(true);
			camera->SetDepth(1);
			camera->SetZNear(-100);
			camera->SetZFar(1000);
			camera->SetClearFlag(ClearFlag::DepthOnly);
			camera->SetCullMask(LayerMask::GetMask(LayerMask::UI));

			UICanvasScaler* scaler = ui_root->AddComponent<UICanvasScaler>();
			scaler->SetReferenceResolution(iSize(Screen::GetWidth(), Screen::GetHeight()));
			scaler->SetScaleMatchMode(UIScaleMatchMode::MatchWidthOrHeight);
			scaler->SetMatchWidth(true);
		}

		Transform* parent_node = nullptr;
		if (EditorAppliction::GetInspectorId())
		{
			GameObject* obj = SceneManager::GetGameObject(EditorAppliction::GetInspectorId());
			if (obj)parent_node = obj->GetTransform();
		}
		else
		{
			UICanvas* canvas = SceneManager::GetUICanvas(0);
			if (canvas)parent_node = canvas->GetTransform();
		}
		GameObject* new_obj = GameObject::InstantiationUI("GameObject", parent_node);
		EditorAppliction::SetInspectorType(InspectorType::Hierarchy, new_obj->GetInstanceId());
		return new_obj;
	};

	ImGui::Separator();
	if (ImGui::MenuItem("Create Empty"))
	{
		OnCreateEmpty();
	}
	if (ImGui::BeginMenu("3D Object"))
	{
		{
			String file = "";
			if (ImGui::Selectable("Cube"))
				file = "internal/mesh/cube.fbx";
			if (ImGui::Selectable("Sphere"))
				file = "internal/mesh/sphere.fbx";
			if (ImGui::Selectable("Capsule"))
				file = "internal/mesh/capsule.obj";
			if (ImGui::Selectable("Cylinder"))
				file = "internal/mesh/cylinder.fbx";
			if (ImGui::Selectable("Plane"))
				file = "internal/mesh/plane.obj";
			if (ImGui::Selectable("Quad"))
				file = "internal/mesh/quad.obj";
			if (!file.IsEmpty())
			{
				GameObject* new_obj = OnCreateEmpty();
				MeshRender* mesh_render = new_obj->AddComponent<MeshRender>();
				mesh_render->SetMesh(Mesh::Create(file));
			}
		}
		ImGui::Separator();
		if (ImGui::Selectable("Terrain"))
		{
			GameObject* new_obj = OnCreateEmpty();
			Terrain* render = new_obj->AddComponent<Terrain>();
			render->SetHeightMap("internal/texture/height.jpg");
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("2D Object"))
	{
		if (ImGui::Selectable("Sprite"))
		{
			GameObject* new_obj = OnCreateEmpty();
			new_obj->AddComponent<Sprite>();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Effects"))
	{
		if (ImGui::Selectable("Particle System"))
		{
			GameObject* new_obj = OnCreateEmpty();
			ParticleSystem* ps = new_obj->AddComponent<ParticleSystem>();
			new_obj->GetTransform()->Pitch(-90.0f);
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Light"))
	{
		if (ImGui::Selectable("Directional Light"))
		{
			GameObject* new_obj = OnCreateEmpty();
			Light* component = new_obj->AddComponent<Light>();
			component->mType = LightType::Direction;
		}
		if (ImGui::Selectable("Point Light"))
		{
			GameObject* new_obj = OnCreateEmpty();
			Light* component = new_obj->AddComponent<Light>();
			component->mType = LightType::Point;
		}
		if (ImGui::Selectable("Spot Light"))
		{
			GameObject* new_obj = OnCreateEmpty();
			Light* component = new_obj->AddComponent<Light>();
			component->mType = LightType::Spot;
		}
		if (ImGui::Selectable("Relection Probe"))
		{
			Debuger::Log("Create Relection Probe");
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("UI"))
	{
		if (ImGui::Selectable("Image"))
		{
			GameObject* new_obj = OnCreateUIEmpty();
			new_obj->SetInstanceName("Image");
			UIImage* image = new_obj->AddComponent<UIImage>();
			image->SetAtlas("", "");
		}
		if (ImGui::Selectable("Text"))
		{
			GameObject* new_obj = OnCreateUIEmpty();
			new_obj->SetInstanceName("Text");
			UILabel* label = new_obj->AddComponent<UILabel>();
			label->SetText("");
		}
		if (ImGui::Selectable("Button"))
		{
			GameObject* new_obj = OnCreateUIEmpty();
			new_obj->SetInstanceName("Button");
			new_obj->AddComponent<UIButton>();
		}
		if (ImGui::Selectable("LabelButton"))
		{
			GameObject* new_obj = OnCreateUIEmpty();
			new_obj->SetInstanceName("LabelButton");
			new_obj->AddComponent<UIButton>();

			//label
			GameObject* obj_label = GameObject::Instantiation("Text", new_obj->GetTransform());
			UILabel* label = obj_label->AddComponent<UILabel>();
			label->SetText("Button");
			label->SetColor(Color::Black);
			label->SetAlignment(UITextAnchor::MiddleCenter);
			label->SetHorizontalOverflow(UIHorizontalWrapMode::Overflow);
			label->SetRaycastTarget(false);
		}
		if (ImGui::Selectable("NormalButton"))
		{
			GameObject* new_obj = OnCreateUIEmpty();
			new_obj->SetInstanceName("NormalButton");
			UINormalButton* button = new_obj->AddComponent<UINormalButton>();
			button->SetStatus(UIButtonStatus::Normal);
		}
		if (ImGui::Selectable("InputField"))
		{
			GameObject* new_obj = OnCreateUIEmpty();
			new_obj->SetInstanceName("InputField");
			UIInputField* input_field = new_obj->AddComponent<UIInputField>();

			//label
			GameObject* obj_label = GameObject::Instantiation("Text", new_obj->GetTransform());
			UILabel* label = obj_label->AddComponent<UILabel>();
			label->SetColor(Color::Black);
			label->SetAlignment(UITextAnchor::MiddleLeft);
			label->SetHorizontalOverflow(UIHorizontalWrapMode::Overflow);
			label->SetRaycastTarget(false);

			//caret
			GameObject* obj_caret = GameObject::Instantiation("Placeholder", new_obj->GetTransform());
			UIImage* caret = obj_caret->AddComponent<UIImage>();
			caret->SetColor(label->GetColor());
			caret->SetSize(1, label->GetFontSize());
			caret->SetRaycastTarget(false);

			input_field->SetText("");
			input_field->SetAtlas("internal/atlas/ui.png", "InputFieldBackground.png");
			input_field->SetImageType(UIImageType::Sliced);
			input_field->SetImageBorder(Vector4(10.0f, 10.0f, 10.0f, 10.0f));
			input_field->SetPlaceholderTextColor(label->GetColor());
		}
		if (ImGui::Selectable("Checkbox"))
		{
			GameObject* new_obj = OnCreateUIEmpty();
			new_obj->SetInstanceName("Checkbox");
			UICheckbox* checkbox = new_obj->AddComponent<UICheckbox>();
			checkbox->SetChecked(false);
		}
		if (ImGui::Selectable("Canvas"))
		{
			GameObject* new_obj = OnCreateUIEmpty();
			new_obj->SetInstanceName("Canvas");
			new_obj->AddComponent<UICanvas>();
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Camera"))
	{
		GameObject* new_obj = OnCreateEmpty();
		new_obj->AddComponent<Camera>();
	}
}
void ECommonComponent::ShowCreateAsset()
{
	if (ImGui::BeginMenu("Create"))
	{
		const String& select_path = EMain_Project::GetSelectPath();
		ImGui::Separator();
		//if (ImGui::MenuItem("Folder"))
		//{
		//	Directory::Create(Path::Combine(Resource::GetFullDataPath(select_path), "New Folder"));
		//}

		if (ImGui::MenuItem("Material"))
		{
			String path = Path::Combine(Resource::GetFullDataPath(select_path), "New Material.material");
			File::Copy(Resource::GetFullSavePath("data/template/MaterialTemplate.material"), path);
		}

		ImGui::EndMenu();
	}
}
bool ECommonComponent::ShowDropDown(const char* label, const VecString& list, int& curr_select)
{
	bool result = false;
	const char* pre_view = nullptr;
	if (curr_select >= list.Size())curr_select = 0;
	if (list.Size() > 0 && curr_select >= 0) pre_view = list[curr_select].c_str();
	if (ImGui::BeginCombo(label, pre_view))
	{
		for (int i = 0; i < list.Size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::Selectable(list[i].c_str()))
			{
				curr_select = i;
				result = true;
			}
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	return result;
}
bool ECommonComponent::ToggleButton(const char* str_id, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.50f;

	bool result = false;
	if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
	{
		*v = !*v;
		result = true;
	}
	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = *v ? IM_COL32(145 + 20, 211, 68 + 20, 255) : IM_COL32(218 - 20, 218 - 20, 218 - 20, 255);
	else
		col_bg = *v ? IM_COL32(145, 211, 68, 255) : IM_COL32(218, 218, 218, 255);

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));

	return result;
}
DC_END_NAMESPACE
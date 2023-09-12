#include "UICanvas.h"
#include "UIView.h"
#include "UIAtlas.h"
#include "UILabel.h"
#include "UICanvasScaler.h"
#include "runtime/component/Component.inl"
#include "runtime/input/Input.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/Pass.h"
#include "runtime/renderable/Gizmos.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "platform/WindowManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_GameView.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UICanvas, Renderer);
UICanvas::UICanvas()
	: _renderMode(UIRenderMode::None)
{
}
UICanvas::~UICanvas()
{
	SceneManager::RemoveUICanvas(this);
	_camera = nullptr;
}
Object* UICanvas::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UICanvas* obj = dynamic_cast<UICanvas*>(new_obj);
	if (!obj)return new_obj;

	obj->SetRenderMode(_renderMode);
	obj->SetCamera(_camera);

	return obj;
}
void UICanvas::Awake()
{
	base::Awake();
	GetGameObject()->InsertFlag(GameObjectFlag::AlwaysFindInScene);
	Transform* transorm = GetGameObject()->GetTransform();
	transorm->SetEditorCanTranslate(false);
	transorm->SetEditorCanRotate(false);
	transorm->SetEditorCanScale(false);
	SetMaterial("internal/material/UI.material");
}
void UICanvas::OnDestroy()
{
	base::OnDestroy();
	Transform* transorm = GetGameObject()->GetTransform();
	if (transorm)
	{
		transorm->SetEditorCanTranslate(true);
		transorm->SetEditorCanRotate(true);
		transorm->SetEditorCanScale(true);
	}
}
bool UICanvas::CanRemove()
{
	if (_renderMode != UIRenderMode::None)
	{
		return false;
	}
	return base::CanRemove();
}
void UICanvas::OnDrawGizmos(Camera* camera)
{
	if (_drawGizmos)
	{
		UICanvasScaler* scaler = GetComponent<UICanvasScaler>();
		if (scaler)
		{
			const iSize& size = scaler->GetRealResolution();
			Color color = Gizmos::SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));
			Gizmos::SetColor(Color::White);
			Gizmos::DrawRect(Vector3::zero, Vector2(size.width * 0.01f, size.height * 0.01f));
			Gizmos::SetColor(color);
		}
	}
}
void UICanvas::Update()
{
	DC_PROFILE_FUNCTION();
	base::Update();
	//big->small
	const auto GetSortCanvas = [this](List<UICanvas*>& sort_canvas)
	{
		List<UICanvas*> list_canvas = this->GetGameObject()->GetComponentsInChildren<UICanvas>(true);
		for (const auto& canvas : list_canvas)
		{
			int64 sort_layer = canvas->GetSortLayer();
			int64 order = canvas->GetOrderInLayer();
			int64 max_value = UINT_MAX;
			int64 group_id = (max_value*(sort_layer < 0 ? -1 : (sort_layer > 0 ? 1 : 0)) + sort_layer) + (order);

			bool is_insert = false;
			for (auto it = sort_canvas.begin(); it != sort_canvas.end(); ++it)
			{
				int64 s_sort_layer = (*it)->GetSortLayer();
				int64 s_order = (*it)->GetOrderInLayer();
				int64 s_group_id = (max_value*(s_sort_layer < 0 ? -1 : (s_sort_layer > 0 ? 1 : 0)) + s_sort_layer) + (s_order);
				if (group_id >= s_group_id)
				{
					sort_canvas.Insert(it, canvas);
					is_insert = true;
					break;
				}
			}
			if (!is_insert)
			{
				sort_canvas.Add(canvas);
			}
		}
	};

	for (int i = 0; i < (int)MouseBtnID::Max; ++i)
	{
		MouseBtnID btn = (MouseBtnID)i;
		if (Input::GetMouseButtonDown(btn))
		{
			Vector2 ui_pos;
			if(!ScreenToUIPoint(Input::mousePosition, ui_pos))
				continue;

			List<UICanvas*> sort_canvas;
			GetSortCanvas(sort_canvas);
			for (const auto& canvas : sort_canvas)
			{
				List<UIView*> list_views;
				this->GetViewsInChildren(canvas->GetTransform(), list_views);
				for (auto it = list_views.rbegin(); it != list_views.rend(); ++it)
				{
					UIView* view = *it;
					if (view->GetRaycastTarget() && view->HitTest((int)ui_pos.x, (int)ui_pos.y))
					{
						view->OnMouseDown(ui_pos, btn);
					}
				}
			}
		}
		if (Input::GetMouseButtonUp(btn))
		{
			Vector2 ui_pos;
			if (!ScreenToUIPoint(Input::mousePosition, ui_pos))
				continue;

			List<UICanvas*> sort_canvas;
			GetSortCanvas(sort_canvas);
			for (const auto& canvas : sort_canvas)
			{
				List<UIView*> list_views;
				this->GetViewsInChildren(canvas->GetTransform(), list_views);
				for (auto it = list_views.rbegin(); it != list_views.rend(); ++it)
				{
					UIView* view = *it;
					if (view->_isMouseDown[(int)btn])
					{
						if (view->HitTest((int)ui_pos.x, (int)ui_pos.y))
						{
							view->OnMouseUpInside(ui_pos, btn);
							view->OnMouseClick(ui_pos, btn);
						}
						else
						{
							view->OnMouseUpOutside(ui_pos, btn);
						}
					}
				}
			}
		}
	}
}
void UICanvas::Refresh()
{
	if (_isNeedRebuild)
	{
		//step1:sort out by material
		List<UIView*> list_views;
		this->GetViewsInChildren(this->GetTransform(), list_views);

		List<std::pair<String, List<UIView*>>> map_views;
		for (const auto& view : list_views)
		{
			if (!view->IsEnable() || !view->GetGameObject()->ActiveInHierarchy())continue;

			String tex_name = view->GetAtlasFile();
			auto it = map_views.begin();
			for (; it != map_views.end(); ++it)
			{
				if ((*it).first == tex_name)
				{
					break;
				}
			}
			if (it == map_views.end())
			{
				List<UIView*> list;
				list.Add(view);
				map_views.Add({ tex_name, list });
			}
			else
			{
				(*it).second.Add(view);
			}
		}

		//step2:build mesh
		this->ClearData();
		int prim_index = 0;
		for (const auto& batches : map_views)
		{
			VariablePrimitive* primitive = dynamic_cast<VariablePrimitive*>(GetPrimitive(prim_index));
			Material* material = GetMaterial(primitive, prim_index);
			material->SetTexture(Texture::GetWhiteTexture());

			int vxt_offset = 0, idx_offset = 0;
			for (auto& view : batches.second)
			{
				view->FillMesh(primitive, material, vxt_offset, idx_offset);
			}
			prim_index++;
		}
		this->UploadData();

		//step3:rebuild complete
		_isNeedRebuild = false;
	}

	//update child canvas
	List<UICanvas*> list_canvas = this->GetGameObject()->GetComponentsInChildren<UICanvas>(false);
	for (auto& canvas : list_canvas)
	{
		canvas->Refresh();
	}
}
void UICanvas::SetRenderMode(UIRenderMode mode)
{
	AssertEx(mode != UIRenderMode::None, "error UIRenderMode");
	_renderMode = mode;
	SceneManager::AddUICanvas(this);
}
void UICanvas::SetCamera(Camera* camera) 
{
	_camera = camera;
}
bool UICanvas::ScreenToUIPoint(const Vector3& position, Vector2& ui_pos)
{
	Size screen_size(Screen::GetWidth(), Screen::GetHeight());
	if (_camera)
	{
		RenderWindow* window = WindowManager::GetWindow(_camera->GetTargetDisplay());
		if (window)screen_size.Set(window->GetWidth(), window->GetHeight());
	}

	ui_pos = Vector2(position.x, position.y);
	UICanvasScaler* scaler = this->GetComponent<UICanvasScaler>();
	if (Application::IsEditor())
	{
		const Rect& game_rect = EMain_GameView::GetViewPort();//左上角为(0,0);
		ui_pos = Vector2(ui_pos.x - game_rect.x, ui_pos.y - (screen_size.height - (game_rect.y + game_rect.height)));//鼠标位置转换成相对Game视图位置
		if (ui_pos.x < 0.0f || ui_pos.x > game_rect.width || ui_pos.y < 0.0f || ui_pos.y > game_rect.height)return false;
		if (scaler)
		{
			ui_pos.x = ui_pos.x * (scaler->GetRealResolution().width / game_rect.width);
			ui_pos.y = ui_pos.y * (scaler->GetRealResolution().height / game_rect.height);
			ui_pos = ui_pos - Vector2(scaler->GetRealResolution().width, scaler->GetRealResolution().height)*0.5f;
		}
		else
		{
			ui_pos = ui_pos - Vector2(game_rect.width, game_rect.height)*0.5f;
		}
	}
	else
	{
		if (scaler)
		{
			ui_pos.x = ui_pos.x * (scaler->GetRealResolution().width / screen_size.width);
			ui_pos.y = ui_pos.y * (scaler->GetRealResolution().height / screen_size.height);
			ui_pos = ui_pos - Vector2(scaler->GetRealResolution().width, scaler->GetRealResolution().height)*0.5f;
		}
		else
		{
			ui_pos = ui_pos - Vector2(screen_size.width, screen_size.height)*0.5f;
		}
	}
	return true;
}
void UICanvas::GetViewsInChildren(Transform* node, List<UIView*>& coms) const
{
	int child_count = node->GetChildCount();
	for (int i = 0; i < child_count; ++i)
	{
		GameObject* child = node->GetChild(i)->GetGameObject();
		if (child->GetComponent<UICanvas>() != nullptr)
			break;
		UIView* view = child->GetComponent<UIView>();
		if (view != nullptr)
			coms.Add(view);
		GetViewsInChildren(child->GetTransform(), coms);
	}
}
Material* UICanvas::GetMaterial(Primitive* primitive, int index)
{
	CHECK_RETURN_PTR_NULL(primitive && index >= 0);

	Material* material = nullptr;
	if (index >= _materials.Size())
	{
		material = Material::Create("internal/material/UI.material");
		material->Retain();
		_materials.Add(material);
	}
	material = _materials[index];

	//设置材质
	primitive->SetMaterial(material);
	return material;
}
void UICanvas::OnDrawEditor()
{
	Component::OnDrawEditor();
	if (_renderMode != UIRenderMode::None)
	{
		const char* sz_flags[] = { "ScreenSpace - Overlay", "ScreenSpace - Camera", "WorldSpace" };
		ImGuiEx::Label("Render Mode");
		static int current_flags = (int)_renderMode - 1;
		if (ImGui::Combo("##RenderMode", &current_flags, sz_flags, ARRAY_SIZE(sz_flags)))
		{
			_renderMode = UIRenderMode(current_flags + 1);
		}
	}
	ImGuiEx::Label("Sort Order");
	if (ImGui::DragInt("##OrderInLayer", &_orderInLayer, 0.5f, INT_MIN, INT_MAX))
	{
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UICanvas);
template<class TransferFunction> inline
void UICanvas::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_ENUM(_renderMode);
	if (transfer.IsRead())
	{
		if (_renderMode != UIRenderMode::None)
		{
			SetRenderMode(_renderMode);
			if (_renderMode == UIRenderMode::ScreenSpace_Camera)
			{
				Camera* camera = GetComponent<Camera>();
				AssertEx(camera, "");
				if (camera)SetCamera(camera);
			}
		}
	}
}
DC_END_NAMESPACE

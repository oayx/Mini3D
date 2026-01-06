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
Object* UICanvas::Clone(Object* newObj)
{
	base::Clone(newObj);
	UICanvas* obj = dynamic_cast<UICanvas*>(newObj);
	if (!obj)return newObj;

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
	DC_PROFILE_FUNCTION;
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
	DC_PROFILE_FUNCTION;
	base::Update();
	//big->small
	const auto GetSortCanvas = [this](List<UICanvas*>& sortCanvas)
	{
		List<UICanvas*> listCanvas = this->GetGameObject()->GetComponentsInChildren<UICanvas>(true);
		for (const auto& canvas : listCanvas)
		{
			int64 sortLayer = canvas->GetSortLayer();
			int64 order = canvas->GetOrderInLayer();
			int64 maxValue = UINT_MAX;
			int64 groupId = (maxValue*(sortLayer < 0 ? -1 : (sortLayer > 0 ? 1 : 0)) + sortLayer) + (order);

			bool isInsert = false;
			for (auto it = sortCanvas.begin(); it != sortCanvas.end(); ++it)
			{
				int64 sortLayer = (*it)->GetSortLayer();
				int64 order = (*it)->GetOrderInLayer();
				int64 s_group_id = (maxValue*(sortLayer < 0 ? -1 : (sortLayer > 0 ? 1 : 0)) + sortLayer) + (order);
				if (groupId >= s_group_id)
				{
					sortCanvas.Insert(it, canvas);
					isInsert = true;
					break;
				}
			}
			if (!isInsert)
			{
				sortCanvas.Add(canvas);
			}
		}
	};

	for (int i = 0; i < (int)MouseBtnID::Max; ++i)
	{
		MouseBtnID btn = (MouseBtnID)i;
		if (Input::GetMouseButtonDown(btn))
		{
			Vector2 uiPos;
			if(!ScreenToUIPoint(Input::mousePosition, uiPos))
				continue;

			List<UICanvas*> sortCanvas;
			GetSortCanvas(sortCanvas);
			for (const auto& canvas : sortCanvas)
			{
				List<UIView*> listViews;
				this->GetViewsInChildren(canvas->GetTransform(), listViews);
				for (auto it = listViews.rbegin(); it != listViews.rend(); ++it)
				{
					UIView* view = *it;
					if (view->GetRaycastTarget() && view->HitTest((int)uiPos.x, (int)uiPos.y))
					{
						view->OnMouseDown(uiPos, btn);
					}
				}
			}
		}
		if (Input::GetMouseButtonUp(btn))
		{
			Vector2 uiPos;
			if (!ScreenToUIPoint(Input::mousePosition, uiPos))
				continue;

			List<UICanvas*> sortCanvas;
			GetSortCanvas(sortCanvas);
			for (const auto& canvas : sortCanvas)
			{
				List<UIView*> listViews;
				this->GetViewsInChildren(canvas->GetTransform(), listViews);
				for (auto it = listViews.rbegin(); it != listViews.rend(); ++it)
				{
					UIView* view = *it;
					if (view->_isMouseDown[(int)btn])
					{
						if (view->HitTest((int)uiPos.x, (int)uiPos.y))
						{
							view->OnMouseUpInside(uiPos, btn);
							view->OnMouseClick(uiPos, btn);
						}
						else
						{
							view->OnMouseUpOutside(uiPos, btn);
						}
					}
				}
			}
		}
	}
}
void UICanvas::Refresh()
{
	DC_PROFILE_FUNCTION;
	if (_isNeedRebuild)
	{
		//step1:sort out by material
		List<UIView*> listViews;
		this->GetViewsInChildren(this->GetTransform(), listViews);

		List<std::pair<String, List<UIView*>>> mapViews;
		for (const auto& view : listViews)
		{
			if (!view->IsEnable() || !view->GetGameObject()->ActiveInHierarchy())continue;

			String tex_name = view->GetAtlasFile();
			auto it = mapViews.begin();
			for (; it != mapViews.end(); ++it)
			{
				if ((*it).first == tex_name)
				{
					break;
				}
			}
			if (it == mapViews.end())
			{
				List<UIView*> list;
				list.Add(view);
				mapViews.Add({ tex_name, list });
			}
			else
			{
				(*it).second.Add(view);
			}
		}

		//step2:build mesh
		this->ClearData();
		int primIndex = 0;
		for (const auto& batches : mapViews)
		{
			VariablePrimitive* primitive = dynamic_cast<VariablePrimitive*>(GetPrimitive(primIndex));
			Material* material = GetMaterial(primitive, primIndex);
			material->SetTexture(Texture::GetWhiteTexture());

			int vxtOffset = 0, idxOffset = 0;
			for (auto& view : batches.second)
			{
				view->FillMesh(primitive, material, vxtOffset, idxOffset);
			}
			primIndex++;
		}
		this->UploadData();

		//step3:rebuild complete
		_isNeedRebuild = false;
	}

	//update child canvas
	List<UICanvas*> listCanvas = this->GetGameObject()->GetComponentsInChildren<UICanvas>(false);
	for (auto& canvas : listCanvas)
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
bool UICanvas::ScreenToUIPoint(const Vector3& position, Vector2& uiPos)
{
	DC_PROFILE_FUNCTION;
	Size screenSize(Screen::GetWidth(), Screen::GetHeight());
	if (_camera)
	{
		RenderWindow* window = WindowManager::GetWindow(_camera->GetTargetDisplay());
		if (window)screenSize.Set(window->GetWidth(), window->GetHeight());
	}

	uiPos = Vector2(position.x, position.y);
	UICanvasScaler* scaler = this->GetComponent<UICanvasScaler>();
	if (Application::IsEditor())
	{
		const Rect& gameRect = EMain_GameView::GetViewPort();//左上角为(0,0);
		uiPos = Vector2(uiPos.x - gameRect.x, uiPos.y - (screenSize.height - (gameRect.y + gameRect.height)));//鼠标位置转换成相对Game视图位置
		if (uiPos.x < 0.0f || uiPos.x > gameRect.width || uiPos.y < 0.0f || uiPos.y > gameRect.height)return false;
		if (scaler)
		{
			uiPos.x = uiPos.x * (scaler->GetRealResolution().width / gameRect.width);
			uiPos.y = uiPos.y * (scaler->GetRealResolution().height / gameRect.height);
			uiPos = uiPos - Vector2(scaler->GetRealResolution().width, scaler->GetRealResolution().height)*0.5f;
		}
		else
		{
			uiPos = uiPos - Vector2(gameRect.width, gameRect.height)*0.5f;
		}
	}
	else
	{
		if (scaler)
		{
			uiPos.x = uiPos.x * (scaler->GetRealResolution().width / screenSize.width);
			uiPos.y = uiPos.y * (scaler->GetRealResolution().height / screenSize.height);
			uiPos = uiPos - Vector2(scaler->GetRealResolution().width, scaler->GetRealResolution().height)*0.5f;
		}
		else
		{
			uiPos = uiPos - Vector2(screenSize.width, screenSize.height)*0.5f;
		}
	}
	return true;
}
void UICanvas::GetViewsInChildren(Transform* node, List<UIView*>& coms) const
{
	int childCount = node->GetChildCount();
	for (int i = 0; i < childCount; ++i)
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
	DC_PROFILE_FUNCTION;
	Component::OnDrawEditor();
	if (_renderMode != UIRenderMode::None)
	{
		const char* szFlags[] = { "ScreenSpace - Overlay", "ScreenSpace - Camera", "WorldSpace" };
		ImGuiEx::Label("Render Mode");
		static int currentFlags = (int)_renderMode - 1;
		if (ImGui::Combo("##RenderMode", &currentFlags, szFlags, ARRAY_SIZE(szFlags)))
		{
			_renderMode = UIRenderMode(currentFlags + 1);
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

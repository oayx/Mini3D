#include "GUI.h"
#include "core/Encoding.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/null/SwapChain.h"
#include "runtime/scene/Camera.h"
#include "runtime/Screen.h"
#include "runtime/Application.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(GUIContext);
void GUIContext::Initialize()
{
	IMGUI_CHECKVERSION();
	InitImGUI();
}
void GUIContext::Destroy()
{
	ReleaseImGUI();
	for (auto obj : _primitives)
	{
		SAFE_DELETE(obj);
	}
	_primitives.Clear();

	GUITextureCache::Destroy();
}
void GUIContext::InitImGUI()
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	//输入
	{
		io.KeyMap[ImGuiKey_Tab] = (int)KeyCode::Tab;
		io.KeyMap[ImGuiKey_LeftArrow] = (int)KeyCode::LeftArrow;
		io.KeyMap[ImGuiKey_RightArrow] = (int)KeyCode::RightArrow;
		io.KeyMap[ImGuiKey_UpArrow] = (int)KeyCode::UpArrow;
		io.KeyMap[ImGuiKey_DownArrow] = (int)KeyCode::DownArrow;
		io.KeyMap[ImGuiKey_PageUp] = (int)KeyCode::PageUp;
		io.KeyMap[ImGuiKey_PageDown] = (int)KeyCode::PageDown;
		io.KeyMap[ImGuiKey_Home] = (int)KeyCode::Home;
		io.KeyMap[ImGuiKey_End] = (int)KeyCode::End;
		io.KeyMap[ImGuiKey_Insert] = (int)KeyCode::Insert;
		io.KeyMap[ImGuiKey_Delete] = (int)KeyCode::Delete;
		io.KeyMap[ImGuiKey_Backspace] = (int)KeyCode::Backspace;
		io.KeyMap[ImGuiKey_Space] = (int)KeyCode::Space;
		io.KeyMap[ImGuiKey_Enter] = (int)KeyCode::Return;
		io.KeyMap[ImGuiKey_Escape] = (int)KeyCode::Escape;
		io.KeyMap[ImGuiKey_KeyPadEnter] = (int)KeyCode::KeypadEnter;
		io.KeyMap[ImGuiKey_A] = (int)KeyCode::A;
		io.KeyMap[ImGuiKey_C] = (int)KeyCode::C;
		io.KeyMap[ImGuiKey_V] = (int)KeyCode::V;
		io.KeyMap[ImGuiKey_X] = (int)KeyCode::X;
		io.KeyMap[ImGuiKey_Y] = (int)KeyCode::Y;
		io.KeyMap[ImGuiKey_Z] = (int)KeyCode::Z;
	}
}
void GUIContext::ReleaseImGUI()
{
	SAFE_RELEASE(_texture);
	ImGui::DestroyContext();
}
void GUIContext::PreRender()
{
	DC_PROFILE_FUNCTION;
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)Screen::GetWidth(), (float)Screen::GetHeight());
	io.DeltaTime = Time::GetUnscaledDeltaTime();

	// input
	for (int i = 0; i < 3; ++i)
	{
		if (Input::GetMouseButtonDown((MouseBtnID)i) || Input::GetMouseDClick((MouseBtnID)i))
		{//imgui内部处理双击事件
			io.MouseDown[i] = true;
		}
		if (Input::GetMouseButtonUp((MouseBtnID)i))
		{
			io.MouseDown[i] = false;
		}
	}
	const auto& pos = Input::mousePosition;
	io.MousePos = ImVec2(pos.x, Screen::GetHeight() - pos.y - 1);
	io.MouseWheel += Input::GetScrollDelta().y;

	// key
	io.KeyCtrl = Input::GetKey(KeyCode::LeftControl) || Input::GetKey(KeyCode::RightControl);
	io.KeyShift = Input::GetKey(KeyCode::LeftShift) || Input::GetKey(KeyCode::RightShift);
	io.KeyAlt = Input::GetKey(KeyCode::LeftAlt) || Input::GetKey(KeyCode::RightAlt);
	io.KeySuper = false;
	for (int i = 0; i < (int)KeyCode::Max && i < 512; ++i)
	{
		io.KeysDown[i] = Input::GetKey((KeyCode)i);
	}

	// key char
	const List<ushort>& chars = Input::GetInputChar();
	if (chars.Size() > 0)
	{
		Vector<char> cs;
		cs.Reserve(chars.Size());
		for (const auto& c : chars)
		{
			if (c <= 0xff)
			{
				cs.Add(c & 0xff);
			}
			else
			{
				cs.Add((c & 0xff00) >> 8);
				cs.Add(c & 0xff);
			}
		}
		String str = String(&cs[0], cs.Size());
		io.AddInputCharactersUTF8(str.c_str());
	}

	//cursor
	if (Application::IsEditor())
	{
		EditorCursor::Update();
	}

	CreateTexture();

	ImGui::NewFrame();
}
void GUIContext::Render()
{
	DC_PROFILE_FUNCTION;
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();

	//正交投影
	float L = drawData->DisplayPos.x;
	float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
	float T = drawData->DisplayPos.y;
	float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
	Matrix4 mvp(2.0f / (R - L), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (T - B), 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f
	);
#if defined(DC_GRAPHICS_API_VULKAN)
	//Vulkan Y轴朝下，这里修改成Y轴朝上
	mvp.Set(3, 1, -mvp.Get(3, 1));
	mvp.Set(1, 1, -mvp.Get(1, 1));
#endif

	ImVec2 clip_off = drawData->DisplayPos;
	if (drawData->Valid)
	{
		for (int i = 0; i < _primitives.Size(); ++i)
		{
			_primitives[i]->ClearData();
		}

		const static int VERTEX_SIZE = sizeof(ImDrawVert);
		const static int INDEX_SIZE = sizeof(ImDrawIdx);

		FixedPrimitive<VertexColorUVLayout>* primitive = GetPrimitive(0);
		Material* material = primitive->GetMaterial();
		material->SetTexture(_texture);
		material->SetMatrix("ProjectionMatrix", mvp);
		material->SetScissorEnable(true);

		//step1: get all vertex and index
		Vector<uint> vtx_offsetes, idx_offsetes;
		vtx_offsetes.Reserve(100); idx_offsetes.Reserve(100);
		Vector<VertexColorUVLayout>& vertexes = primitive->GetVertexBuffer();
		Vector<uint>& indices = primitive->GetIndexBuffer();
		int totalVertexSize = 0, totalIndexSize = 0;
		{
			DC_PROFILE_NAME(GUIContext::BuildVertex);

			vtx_offsetes.Add(0);
			idx_offsetes.Add(0);
			for (int i = 0; i < drawData->CmdListsCount; ++i)
			{
				const ImDrawList* cmdList = drawData->CmdLists[i];
				totalVertexSize += cmdList->VtxBuffer.size();
				totalIndexSize += cmdList->IdxBuffer.Size;
				vtx_offsetes.Add(totalVertexSize);
				idx_offsetes.Add(totalIndexSize);
			}
			if (vertexes.Size() < totalVertexSize)vertexes.Resize(totalVertexSize);
			if (indices.Size() < totalIndexSize)indices.Resize(totalIndexSize);
			primitive->SetVertexSize(totalVertexSize);

			for (int i = 0; i < drawData->CmdListsCount; ++i)
			{
				const ImDrawList* cmdList = drawData->CmdLists[i];
				::memcpy(&vertexes[vtx_offsetes[i]], cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * VERTEX_SIZE);
				::memcpy(&indices[idx_offsetes[i]], cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * INDEX_SIZE);
			}
		}

		//step2: submesh
		for (int i = 0; i < drawData->CmdListsCount; ++i)
		{
			DC_PROFILE_NAME(GUIContext::Render);
			const ImDrawList* cmdList = drawData->CmdLists[i];
			for (int j = 0; j < cmdList->CmdBuffer.size(); ++j)
			{
				const ImDrawCmd& cmd = cmdList->CmdBuffer[j];
				//子图元
				uint elem_count = cmd.ElemCount; uint vtx_offset = cmd.VtxOffset + vtx_offsetes[i]; uint idxOffset = cmd.IdxOffset + idx_offsetes[i];
				SubPrimitive& subPrim = primitive->AddSubPrimitive(elem_count, vtx_offset, idxOffset, vtx_offset, idxOffset);
				//裁剪区域
				iRect clip = { int(cmd.ClipRect.x - clip_off.x),int(cmd.ClipRect.y - clip_off.y),int(cmd.ClipRect.z - cmd.ClipRect.x),int(cmd.ClipRect.w - cmd.ClipRect.y) };
				subPrim.ScissorRect = clip;
				//纹理
				Texture* tex = (Texture*)cmd.GetTexID();
				if (!tex)tex = _texture;
				subPrim.Tex = tex;
				subPrim.TexName = "_MainTex";
				subPrim.AlphaEnable = !tex->IsRenderTexture();//如果是渲染到纹理，关闭alpha
			}
		}
		//step3:render
		primitive->UploadData(PrimitiveUploadDesc{ GPUResourceUsage::Dynamic });
		Application::GetGraphics()->GetRenderContent()->RenderOnePrimitive(nullptr, primitive->GetMaterial(), primitive, RenderMode::Normal);
	}
}
void GUIContext::PostRender()
{
	DC_PROFILE_FUNCTION;
	ImGui::EndFrame();
}	
FixedPrimitive<VertexColorUVLayout>* GUIContext::GetPrimitive(int index)
{
	if (_primitives.Size() <= index)
	{
		FixedPrimitive<VertexColorUVLayout>* primitive = dynamic_cast<FixedPrimitive<VertexColorUVLayout>*>(Primitive::Create(nullptr, PrimitiveType::TriangleList, VertexLayout::VertexColorTexcoord));
		_primitives.Add(primitive);

		Material* material = Material::Create("internal/material/GUI.material");
		material->SetTexture(_texture);
		primitive->SetMaterial(material);
	}
	return _primitives[index];
}
void GUIContext::CreateTexture()
{
	if (_texture != nullptr)return;

	ImGuiIO& io = ImGui::GetIO();

	unsigned char* fontTexPixels;
	int fontTexWidth;
	int fontTexHeight;
	io.Fonts->GetTexDataAsRGBA32(&fontTexPixels, &fontTexWidth, &fontTexHeight);
	TextureDesc desc;
	desc.width = fontTexWidth; desc.height = fontTexHeight; desc.format = ColorFormat::R8G8B8A8; //所有GUI贴图都是gamma空间执行
	_texture = Texture::CreateFromMemroy(fontTexPixels, desc);
	_texture->Retain();
	_texture->SetResName("GUIRender");
}
/********************************************************************/
void GUITextureCache::Destroy()
{
	for (auto obj : _textures)
	{
		SAFE_RELEASE(obj.second);
	}
	_textures.Clear();
}
Texture* GUITextureCache::Get(const String& file)
{
	String filePath = file;
	if (file.IsEmpty())
		filePath = "internal/texture/editor/highlight_outline.png";

	Texture* tex = nullptr;
	if (_textures.TryGet(filePath, &tex))
		return tex;
	
	TextureDesc desc;//所有GUI贴图都是gamma空间执行
	tex = Texture::Create2D(filePath, desc);
	if (tex)tex->Retain();
	_textures.Add(filePath, tex);
	return tex;
}
DC_END_NAMESPACE
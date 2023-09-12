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
GUIContext::Primitives GUIContext::_primitives;
Texture* GUIContext::_texture = nullptr;
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
	DC_PROFILE_FUNCTION();
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
	DC_PROFILE_FUNCTION();
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();

	//正交投影
	float L = draw_data->DisplayPos.x;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	Matrix4 mvp(2.0f / (R - L), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (T - B), 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f
	);

	ImVec2 clip_off = draw_data->DisplayPos;
	if (draw_data->Valid)
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
		int total_vertex_size = 0, total_index_size = 0;
		{
			DC_PROFILE(GUIContext::BuildVertex);

			vtx_offsetes.Add(0);
			idx_offsetes.Add(0);
			for (int i = 0; i < draw_data->CmdListsCount; ++i)
			{
				const ImDrawList* cmd_list = draw_data->CmdLists[i];
				total_vertex_size += cmd_list->VtxBuffer.size();
				total_index_size += cmd_list->IdxBuffer.Size;
				vtx_offsetes.Add(total_vertex_size);
				idx_offsetes.Add(total_index_size);
			}
			if (vertexes.Size() < total_vertex_size)vertexes.Resize(total_vertex_size);
			if (indices.Size() < total_index_size)indices.Resize(total_index_size);
			primitive->SetVertexSize(total_vertex_size);

			int curr_index_offset = 0;
			for (int i = 0; i < draw_data->CmdListsCount; ++i)
			{
				const ImDrawList* cmd_list = draw_data->CmdLists[i];
				::memcpy(&vertexes[vtx_offsetes[i]], cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * VERTEX_SIZE);
				::memcpy(&indices[idx_offsetes[i]], cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * INDEX_SIZE);
			}
		}

		//step2: submesh
		for (int i = 0; i < draw_data->CmdListsCount; ++i)
		{
			DC_PROFILE(GUIContext::Render);
			const ImDrawList* cmd_list = draw_data->CmdLists[i];
			for (int j = 0; j < cmd_list->CmdBuffer.size(); ++j)
			{
				const ImDrawCmd& cmd = cmd_list->CmdBuffer[j];
				//子图元
				uint elem_count = cmd.ElemCount; uint vtx_offset = cmd.VtxOffset + vtx_offsetes[i]; uint idx_offset = cmd.IdxOffset + idx_offsetes[i];
				SubPrimitive& sub_prim = primitive->AddSubPrimitive(elem_count, vtx_offset, idx_offset, vtx_offset, idx_offset);
				//裁剪区域
				iRect clip = { int(cmd.ClipRect.x - clip_off.x),int(cmd.ClipRect.y - clip_off.y),int(cmd.ClipRect.z - cmd.ClipRect.x),int(cmd.ClipRect.w - cmd.ClipRect.y) };
				sub_prim.ScissorRect = clip;
				//纹理
				Texture* tex = (Texture*)cmd.GetTexID();
				if (!tex)tex = _texture;
				sub_prim.Tex = tex;
				sub_prim.TexName = "_MainTex";
				sub_prim.AlphaEnable = !tex->IsRenderTexture();//如果是渲染到纹理，关闭alpha
			}
		}
		//step3:render
		primitive->UploadData(PrimitiveUploadDesc{ GPUResourceUsage::Dynamic });
		Application::GetGraphics()->GetRenderContent()->RenderOnePrimitive(nullptr, primitive->GetMaterial(), primitive, RenderMode::Normal);
	}
}
void GUIContext::PostRender()
{
	DC_PROFILE_FUNCTION();
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

	unsigned char* font_tex_pixels;
	int font_tex_width;
	int font_tex_height;
	io.Fonts->GetTexDataAsRGBA32(&font_tex_pixels, &font_tex_width, &font_tex_height);
	TextureDesc desc;
	desc.width = font_tex_width; desc.height = font_tex_height; desc.format = ColorFormat::R8G8B8A8; //所有GUI贴图都是gamma空间执行
	_texture = Texture::CreateFromMemroy(font_tex_pixels, desc);
	_texture->Retain();
	_texture->SetResName("GUIRender");
}
/********************************************************************/
Map<String, Texture*> GUITextureCache::_textures;
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
	String file_path = file;
	if (file.IsEmpty())
		file_path = "internal/texture/editor/highlight_outline.png";

	Texture* tex = nullptr;
	if (_textures.TryGet(file_path, &tex))
		return tex;
	
	TextureDesc desc;//所有GUI贴图都是gamma空间执行
	tex = Texture::Create2D(file_path, desc);
	if (tex)tex->Retain();
	_textures.Add(file_path, tex);
	return tex;
}
DC_END_NAMESPACE
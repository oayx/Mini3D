
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：
*****************************************************************************/
#pragma once

#include "runtime/renderable/Renderer.h"
#include "external/imgui/imgui.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GUIContext final : public object
{
	friend class Application;
	friend class EditorAppliction;
	typedef Vector<FixedPrimitive<VertexColorUVLayout>*> Primitives;

	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(GUIContext);
	BEGIN_REFECTION_TYPE(GUIContext)
	END_FINAL_REFECTION_TYPE;

	static void Initialize();
	static void Destroy();

	static void InitImGUI();
	static void ReleaseImGUI();

public:
	static void PreRender();
	static void Render();
	static void PostRender();

private:
	static FixedPrimitive<VertexColorUVLayout>* GetPrimitive(int index);
	static void CreateTexture();

private:
	inline static Primitives _primitives;
	inline static Texture* _texture = nullptr;
};

/********************************************************************/
//贴图缓存
class GUITextureCache final
{
	friend class GUIContext;

	static void Destroy();

public:
	static Texture* Get(const String& file);

private:
	inline static Map<String, Texture*> _textures;
};
DC_END_NAMESPACE
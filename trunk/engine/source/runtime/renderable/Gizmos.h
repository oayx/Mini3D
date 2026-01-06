 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/20
* Description： 
*****************************************************************************/
#pragma once

#include "runtime/renderable/Renderer.h"

DC_BEGIN_NAMESPACE
class Mesh;
class Camera;
/********************************************************************/
class ENGINE_DLL Gizmos final : public Object
{
	friend class Application;
	typedef Delegate<Camera*> Delegates;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Gizmos);
	BEGIN_DERIVED_REFECTION_TYPE(Gizmos, Object)
	END_REFECTION_TYPE;

private:
	static void Initialize();
	static void Destroy();

public:
	static void PreRender(Camera* camera);
	static void PostRender(Camera* camera);
	static void CreateMaterial();

	static void DrawCube(const Vector3& center, const Vector3& size);
	static void DrawRay(const Ray& r);
	static void DrawSphere(const Vector3& center, float radius);
	static void DrawMesh(Mesh* mesh);
	static void DrawFrustum(Camera* camera);
	static void DrawTransform(float length);

	static void DrawRect(const Vector3& center, const Vector2& size);
	static void DrawCircle(const Vector3& center, float radius);
	static void DrawPolygon(const Vector3v& vertices);
	static void DrawLine(const Vector3& start, const Vector3& end);

	static Matrix4  SetMatrix(const Matrix4& mat) { Matrix4 old_mat = _matrix; _matrix = mat; return old_mat; }
	static Color	SetColor(const Color& color) { Color old_color = _color; _color = color; return old_color; }

	static void Register(Function<void(Camera*)>& action) { _handles.Bind(action); }
	static void Unregister(Function<void(Camera*)>& action) { _handles.Unbind(action); }

private:
	inline static FixedPrimitive<VertexColorLayout>* _primitive = nullptr;

	inline static Delegates _handles;
	inline static Matrix4 _matrix = Matrix4::identity;
	inline static Color _color = Color::Black;
};
DC_END_NAMESPACE
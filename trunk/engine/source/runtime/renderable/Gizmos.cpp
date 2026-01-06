#include "Gizmos.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/null/SwapChain.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Gizmos, Object);
void Gizmos::Initialize()
{
	_primitive = dynamic_cast<FixedPrimitive<VertexColorLayout>*>(Primitive::Create(nullptr, PrimitiveType::LineList, VertexLayout::VertexColor));
}
void Gizmos::Destroy()
{
	SAFE_DELETE(_primitive);
}
void Gizmos::PreRender(Camera* camera)
{
	if (_primitive == nullptr)return;
	_primitive->ClearData(); 
}
void Gizmos::PostRender(Camera* camera)
{
	if (_primitive == nullptr)return;
	if (_primitive->GetMaterial() == nullptr)
	{
		CreateMaterial();
	}
	
	//其他固定渲染的
	_handles(camera);

	if (!_primitive->IsEmpty())
	{
		_primitive->UploadData(PrimitiveUploadDesc());
		Application::GetGraphics()->GetRenderContent()->RenderOnePrimitive(camera, _primitive->GetMaterial(), _primitive, RenderMode::Normal);
	}
}
void Gizmos::CreateMaterial()
{
	Material* mat = Material::Create("internal/material/Gizmos.material");
	_primitive->SetMaterial(mat);
}
void Gizmos::DrawCube(const Vector3& center, const Vector3& Size)
{
	if (_primitive == nullptr)return;
	//顶点信息
	{
		float x = Size.x * 0.5f, y = Size.y * 0.5f, z = Size.z * 0.5f;

		Vector3v vertices;
		vertices.Reserve(24);
		// 屏幕里面 0,1,2,3
		vertices.Add(_matrix.TransformPoint(Vector3(x, -y, z) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(-x, -y, z) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(-x, y, z) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(x, y, z) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(x, -y, z) + center));

		// 屏幕外面 4,5,6,7
		vertices.Add(_matrix.TransformPoint(Vector3(x, -y, -z) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(-x, -y, -z) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(-x, y, -z) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(x, y, -z) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(x, -y, -z) + center));

		vertices.Add(_matrix.TransformPoint(Vector3(x, -y, z) + center));//0,4
		vertices.Add(_matrix.TransformPoint(Vector3(x, -y, -z) + center));

		vertices.Add(_matrix.TransformPoint(Vector3(-x, -y, z) + center));//1,5
		vertices.Add(_matrix.TransformPoint(Vector3(-x, -y, -z) + center));

		vertices.Add(_matrix.TransformPoint(Vector3(-x, y, z) + center));//2,6
		vertices.Add(_matrix.TransformPoint(Vector3(-x, y, -z) + center));

		vertices.Add(_matrix.TransformPoint(Vector3(x, y, z) + center));//3,7
		vertices.Add(_matrix.TransformPoint(Vector3(x, y, -z) + center));

		VertexColorLayout vertex;
		for (int i = 1; i < (int)vertices.Size(); ++i)
		{
			vertex.pos = vertices[i - 1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = vertices[i];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
		}
	}
}
void Gizmos::DrawRay(const Ray& r)
{
	if (_primitive == nullptr)return;
	//顶点信息
	{
		VertexColorLayout vertex;
		vertex.pos = _matrix.TransformPoint(r.GetOrigin());
		vertex.color = _color;
		_primitive->AddVertex(vertex);
		vertex.pos = _matrix.TransformPoint(r.GetDir());
		vertex.color = _color;
		_primitive->AddVertex(vertex);
	}
}
void Gizmos::DrawSphere(const Vector3& center, float radius)
{
	if (_primitive == nullptr)return;
	//顶点信息
	{
		int segments = Math::Ceil<int>(20.0f * radius);
		segments = Math::Clamp<int>(segments, 10, segments);
		float k_increment = 2.0f * Math::PI / segments;

		Vector3v vertices;
		{
			float theta = 0.0f;
			Vector3 startPos;
			for (int i = 0; i < segments; ++i)
			{
				Vector3 v = center + radius * Vector3(cosf(theta), sinf(theta), 0);
				vertices.Add(_matrix.TransformPoint(v));
				theta += k_increment;
				if (i == 0)startPos = v;
			}
			vertices.Add(_matrix.TransformPoint(startPos));
		}

		{
			float theta = 0.0f;
			Vector3 startPos;
			_primitive->AddSubPrimitive((int)vertices.Size(), (int)vertices.Size(), 0, (int)vertices.Size(), 0);
			for (int i = 0; i < segments; ++i)
			{
				Vector3 v = center + radius * Vector3(0, sinf(theta), cosf(theta));
				vertices.Add(_matrix.TransformPoint(v));
				theta += k_increment;
				if (i == 0)startPos = v;
			}
			vertices.Add(_matrix.TransformPoint(startPos));
		}

		VertexColorLayout vertex;
		for (int i = 1; i < (int)vertices.Size(); ++i)
		{
			vertex.pos = vertices[i - 1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = vertices[i];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
		}
	}
}
void Gizmos::DrawMesh(Mesh* mesh)
{
	if (_primitive == nullptr)return;
}
static void GetFrustumPoint(Camera* camera, float dis, Vector3v& points)
{
	if (!camera)return;
	Transform* transform = camera->GetTransform();
	float height = Math::Tan(camera->GetFov() / 2.0f) * dis;
	float width = height * camera->GetAspect();
	Vector3 dir = transform->GetPosition() + transform->GetForward() * dis;
	Vector3 right = transform->GetRight() * width;
	Vector3 top = transform->GetUp() * height;

	points.Resize(4);
	points[0] = dir + top - right;//top_left
	points[1] = dir + top + right;//top_right
	points[2] = dir - top + right;//bottom_right
	points[3] = dir - top - right;//bottom_left
}
void Gizmos::DrawFrustum(Camera* camera)
{
	if (_primitive == nullptr)return;

	Vector3v farPoints, nearPoints;
	GetFrustumPoint(camera, 10, farPoints);
	GetFrustumPoint(camera, camera->GetZNear(), nearPoints);

	//顶点信息
	{
		VertexColorLayout vertex;
		{
			vertex.pos = nearPoints[0];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = nearPoints[1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);

			vertex.pos = nearPoints[1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = nearPoints[2];
			vertex.color = _color;
			_primitive->AddVertex(vertex);

			vertex.pos = nearPoints[2];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = nearPoints[3];
			vertex.color = _color;
			_primitive->AddVertex(vertex);

			vertex.pos = nearPoints[3];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = nearPoints[0];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
		}

		{
			vertex.pos = farPoints[0];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = farPoints[1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);

			vertex.pos = farPoints[1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = farPoints[2];
			vertex.color = _color;
			_primitive->AddVertex(vertex);

			vertex.pos = farPoints[2];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = farPoints[3];
			vertex.color = _color;
			_primitive->AddVertex(vertex);

			vertex.pos = farPoints[3];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = farPoints[0];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
		}

		{
			vertex.pos = nearPoints[0];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = farPoints[0];
			vertex.color = _color;
			_primitive->AddVertex(vertex);

			vertex.pos = nearPoints[1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = farPoints[1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);

			vertex.pos = nearPoints[2];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = farPoints[2];
			vertex.color = _color;
			_primitive->AddVertex(vertex);

			vertex.pos = nearPoints[3];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = farPoints[3];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
		}
	}
}
void Gizmos::DrawTransform(float length)
{
	if (_primitive == nullptr)return;
	//顶点信息
	{
		VertexColorLayout vertex;
		//x
		{
			vertex.pos = _matrix.TransformPoint(Vector3(0, 0, 0));
			vertex.color = Color::Red;
			_primitive->AddVertex(vertex);
			vertex.pos = _matrix.TransformPoint(Vector3(length, 0, 0));
			vertex.color = Color::Red;
			_primitive->AddVertex(vertex);
		}
		//y
		{
			vertex.pos = _matrix.TransformPoint(Vector3(0, 0, 0));
			vertex.color = Color::Green;
			_primitive->AddVertex(vertex);
			vertex.pos = _matrix.TransformPoint(Vector3(0, length, 0));
			vertex.color = Color::Green;
			_primitive->AddVertex(vertex);
		}
		//z
		{
			vertex.pos = _matrix.TransformPoint(Vector3(0, 0, 0));
			vertex.color = Color::Blue;
			_primitive->AddVertex(vertex);
			vertex.pos = _matrix.TransformPoint(Vector3(0, 0, length));
			vertex.color = Color::Blue;
			_primitive->AddVertex(vertex);
		}
	}
}
void Gizmos::DrawRect(const Vector3& center, const Vector2& Size)
{
	if (_primitive == nullptr)return;
	//顶点信息
	{
		float x = Size.x * 0.5f, y = Size.y * 0.5f;

		Vector3v vertices; vertices.Reserve(5);
		vertices.Add(_matrix.TransformPoint(Vector3(x, -y, 0) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(-x, -y, 0) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(-x, y, 0) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(x, y, 0) + center));
		vertices.Add(_matrix.TransformPoint(Vector3(x, -y, 0) + center));

		VertexColorLayout vertex;
		for (int i = 1; i < (int)vertices.Size(); ++i)
		{
			vertex.pos = vertices[i - 1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = vertices[i];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
		}
	}
}
void Gizmos::DrawCircle(const Vector3& center, float radius)
{
	if (_primitive == nullptr)return;
	//顶点信息
	{
		int segments = Math::Ceil<int>(20.0f * radius);
		segments = Math::Clamp<int>(segments, 10, segments);
		float k_increment = 2.0f * Math::PI / segments;

		Vector3v vertices;
		{
			float theta = 0.0f;
			Vector3 startPos;
			for (int i = 0; i < segments; ++i)
			{
				Vector3 v = center + radius * Vector3(cosf(theta), sinf(theta), 0);
				vertices.Add(_matrix.TransformPoint(v));
				theta += k_increment;
				if (i == 0)startPos = v;
			}
			vertices.Add(_matrix.TransformPoint(startPos));
		}
		VertexColorLayout vertex;
		for (int i = 1; i < (int)vertices.Size(); ++i)
		{
			vertex.pos = vertices[i - 1];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
			vertex.pos = vertices[i];
			vertex.color = _color;
			_primitive->AddVertex(vertex);
		}
	}
}
void Gizmos::DrawPolygon(const Vector3v& vertices)
{
	if (_primitive == nullptr)return;
	if (vertices.Size() < 2)
	{
		return;
	}
	//顶点信息
	{
		for (int i = 1; i < (int)vertices.Size(); ++i)
		{
			VertexColorLayout vertex;
			{
				vertex.pos = _matrix.TransformPoint(vertices[i - 1]);
				vertex.color = _color;
				_primitive->AddVertex(vertex);
				vertex.pos = _matrix.TransformPoint(vertices[i]);
				vertex.color = _color;
				_primitive->AddVertex(vertex);
			}
		}
	}
}
void Gizmos::DrawLine(const Vector3& start, const Vector3& end)
{
	VertexColorLayout vertex;
	{
		vertex.pos = _matrix.TransformPoint(start);
		vertex.color = _color;
		_primitive->AddVertex(vertex);
		vertex.pos = _matrix.TransformPoint(end);
		vertex.color = _color;
		_primitive->AddVertex(vertex);
	}
}
DC_END_NAMESPACE
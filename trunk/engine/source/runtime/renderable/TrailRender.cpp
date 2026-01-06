#include "TrailRender.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/scene/Camera.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(TrailRender, Renderer);
void TrailRender::Awake()
{
	base::Awake();
	_widthRange.Set(1.0f, 1.0f);
	this->SetPrimitiveType(PrimitiveType::TriangleStrip);
}	
void TrailRender::OnTransformChanged()
{
	base::OnTransformChanged();
	_transformChanged = true;
}
void TrailRender::OnPreRender(Camera* camera)
{
	base::Update();
	float now_time = Time::GetTime();
	bool is_dirty = false;
	//移除过时的
	while (!_timeStamps.empty() && now_time > (_timeStamps.back() + _durationTime))
	{
		_positions.pop_back();
		_timeStamps.pop_back();
		is_dirty = true;
	}

	//增加新的
	if (_transformChanged)
	{
		const Vector3& position = GetTransform()->GetPosition();
		if (_positions.empty() || (_positions.front().SqrDistance(position) > _minVertexDistance * _minVertexDistance))
		{
			_positions.push_front(position);
			_timeStamps.push_front(now_time);
		}

		_transformChanged = false;
		is_dirty = true;
	}

	if (is_dirty)
	{
		this->ClearData();
		if(_positions.size() >= 2)this->Build3DLine(dynamic_cast<VariablePrimitive*>(GetPrimitive(0)), camera);
		this->UploadData();
	}
}
Object* TrailRender::Clone(Object* newObj)
{
	base::Clone(newObj);
	TrailRender* obj = dynamic_cast<TrailRender*>(newObj);
	if (!obj)return newObj;

	obj->SetColor(_color);
	obj->SetWidth(_widthRange.width, _widthRange.height);
	obj->SetDurationTime(_durationTime);
	obj->SetMinVertexDistance(_minVertexDistance);
	obj->GetTransform()->SetTransformDirty();

	return obj;
}
Vector2 Calculate2DLineExtrusion(const Vector3& p0, const Vector3& delta, float halfWidth)
{
	Vector2 dif;
	dif.x = p0.y * delta.z - p0.z * delta.y;
	dif.y = p0.z * delta.x - p0.x * delta.z;

	dif = dif.Normalize();

	dif.x *= halfWidth;
	dif.y *= halfWidth;

	return dif;
}
void TrailRender::Build3DLine(VariablePrimitive* prim, Camera* camera)
{
	DC_PROFILE_FUNCTION;
	Matrix4 worldCameraMatrix = camera->GetViewMatrix();
	Matrix4 cameraWorldMatrix = worldCameraMatrix.Inverse();

	Vector3 delta = cameraWorldMatrix.TransformPoint(_positions[0]) - cameraWorldMatrix.TransformPoint(_positions[1]);
	for (int i = 0; i < (int)_positions.size(); i++)
	{
		//UV
		float u = i / (float)(_positions.size() - 1);

		Vector3 p0 = cameraWorldMatrix.TransformPoint(_positions[i]);
		if (i + 1 != (int)_positions.size())
		{
			Vector3 p1 = cameraWorldMatrix.TransformPoint(_positions[i + 1]);
			delta = p0 - p1;
		}

		//宽度
		float width = Math::Lerp(_widthRange.width, _widthRange.height, u);
		//转弯偏移
		Vector2 dif = Calculate2DLineExtrusion(p0, delta, width * 0.5f);

		//下顶点
		prim->AddVertex(worldCameraMatrix.TransformPoint(Vector3(p0.x - dif.x, p0.y - dif.y, p0.z)));
		prim->AddColor(_color);
		prim->AddTexcoord1(Vector2(u, 1.0f));
		//上顶点
		prim->AddVertex(worldCameraMatrix.TransformPoint(Vector3(p0.x + dif.x, p0.y + dif.y, p0.z)));
		prim->AddColor(_color);
		prim->AddTexcoord1(Vector2(u, 0.0f));
	}
}
DC_END_NAMESPACE
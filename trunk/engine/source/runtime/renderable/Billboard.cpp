#include "Billboard.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/scene/Camera.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Billboard, Renderer);
void Billboard::Awake()
{
	base::Awake();

	float w = 1, h = 1;
	this->SetVertexs({ Vector3(-0.5f*w, 0.5f*h, 0), Vector3(0.5f*w, 0.5f*h, 0), Vector3(0.5f*w, -0.5f*h, 0), Vector3(-0.5f*w, -0.5f*h, 0) });
	this->SetColor(Color::White);
	this->SetTexcoords1({ Vector2(0, 0) ,Vector2(1, 0),Vector2(1, 1),Vector2(0, 1) });
	this->SetIndices({ 0,1,2,0,2,3 });
}
void Billboard::Update()
{
	base::Update();
	if (_isDirty)
	{
		_isDirty = false;
		this->UploadData();
	}
}
void Billboard::OnPreRender(Camera* camera)
{
	base::OnPreRender(camera);
	Vector3 look_dir = GetTransform()->GetPosition() - camera->GetTransform()->GetPosition();
	//look_dir.y = 0;
	//look_dir.Normalize();
	//GetTransform()->LookTo(look_dir, Vector3::up);
	float angle = Math::ATan2(look_dir.x, look_dir.z);
	Quaternion q(angle * Math::Rad2Deg + (angle >= 0 ? 0 : 360), Vector3::up);
	GetTransform()->SetRotation(q);
}
void Billboard::SetColor(const Color& color)
{
	base::SetColor(color);
	_isDirty = true;
}
DC_END_NAMESPACE
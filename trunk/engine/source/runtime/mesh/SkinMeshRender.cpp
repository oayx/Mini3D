#include "SkinMeshRender.h"
#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"
#include "runtime/graphics/Material.h"
#include "runtime/scene/Camera.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(SkinMeshRender, MeshRender);
SkinMeshRender::~SkinMeshRender()
{
	_animation = nullptr;
}
void SkinMeshRender::OnPreRender(Camera* camera)
{
	if (!_mesh || !_animation)return;
	const BoneTransforms& mats = _animation->GetBoneTransform();
	for (int i = 0; i < GetMaterialCount(); ++i)
	{
		GetMaterial(i)->SetMatrixArray("BoneMatrixArray", mats);
	}
}
void SkinMeshRender::OnAddComponent(Component* com)
{
	base::OnAddComponent(com);
	if (com && com->Is<Animation>())
	{
		_animation = dynamic_cast<Animation*>(com);
		SetMesh(_mesh);
	}
}
void SkinMeshRender::OnRemoveComponent(Component* com)
{
	base::OnRemoveComponent(com);
	if (com && com->Is<Animation>())
	{
		_animation = nullptr;
	}
}
void SkinMeshRender::SetMesh(Mesh* mesh)
{
	base::SetMesh(mesh);
	if (_animation)
	{
		_animation->SetMesh(mesh);
	}
}
DC_END_NAMESPACE
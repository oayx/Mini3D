#include "ParticleShape.h"
#include "ParticleSystem.h"
#include "Particle.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ParticleShape, Object);
void ParticleShape::Emit(Particle* particle)
{
	if (!_particleSystem->Shape.Enabled)
		return;
	switch (_particleSystem->Shape.Type)
	{
	case PShapeType::Box:
		EmitBox(particle);
		break;
	case PShapeType::BoxEdge:
		EmitBoxEdge(particle);
		break;
	case PShapeType::Circle:
		EmitCircle(particle);
		break;
	case PShapeType::CircleEdge:
		EmitCircleEdge(particle);
		break;
	case PShapeType::Cone:
		EmitCone(particle);
		break;
	case PShapeType::HemiSphere:
		EmitHemiSphere(particle);
		break;
	case PShapeType::HemiSphereShell:
		EmitHemiSphereShell(particle);
		break;
	case PShapeType::Sphere:
		EmitSphere(particle);
		break;
	case PShapeType::SphereShell:
		EmitSphereShell(particle);
		break;
	}
}
void ParticleShape::EmitBox(Particle* particle)
{
	const ParticleSystem::ShapeModule& shape = _particleSystem->Shape;
	const Matrix4& obj_mat = _particleSystem->GetTransform()->GetLocalToWorldMatrix();
	Matrix4 mat = Matrix4(shape.Position, Quaternion(shape.Rotation), shape.Scale) * obj_mat;

	float x = _particleSystem->RandomRange(-1.0f, 1.0f);
	float y = _particleSystem->RandomRange(-1.0f, 1.0f);
	float z = _particleSystem->RandomRange(-1.0f, 1.0f);
	particle->Position = mat.TransformPoint(Vector3(x, y, z));
	particle->Direction = mat.TransformVector(particle->Direction);
}
void ParticleShape::EmitBoxEdge(Particle* particle)
{
	const ParticleSystem::ShapeModule& shape = _particleSystem->Shape;
	const Matrix4& obj_mat = _particleSystem->GetTransform()->GetLocalToWorldMatrix();
	Matrix4 mat = Matrix4(shape.Position, Quaternion(shape.Rotation), shape.Scale) * obj_mat;

	//先随机面
	int face = _particleSystem->RandomRange(1, 7);//1-6分别对应上、下、左、右、前、后
	//再随机面上的点
	float x = 0, y = 0, z = 0;
	switch (face)
	{
	case 1:
		x = _particleSystem->RandomRange(-1.0f, 1.0f);
		y = 1.0f;
		z = _particleSystem->RandomRange(-1.0f, 1.0f);
		break;
	case 2:
		x = _particleSystem->RandomRange(-1.0f, 1.0f);
		y = -1.0f;
		z = _particleSystem->RandomRange(-1.0f, 1.0f);
		break;
	case 3:
		x = -1.0f;
		y = _particleSystem->RandomRange(-1.0f, 1.0f);
		z = _particleSystem->RandomRange(-1.0f, 1.0f);
		break;
	case 4:
		x = 1.0f;
		y = _particleSystem->RandomRange(-1.0f, 1.0f);
		z = _particleSystem->RandomRange(-1.0f, 1.0f);
		break;
	case 5:
		x = _particleSystem->RandomRange(-1.0f, 1.0f);
		y = _particleSystem->RandomRange(-1.0f, 1.0f);
		z = -1.0f;
		break;
	case 6:
		x = _particleSystem->RandomRange(-1.0f, 1.0f);
		y = _particleSystem->RandomRange(-1.0f, 1.0f);
		z = 1.0f;
		break;
	}
	particle->Position = mat.TransformPoint(Vector3(x, y, z));
	particle->Direction = mat.TransformVector(particle->Direction);
}
void ParticleShape::EmitCircle(Particle* particle)
{
	const ParticleSystem::ShapeModule& shape = _particleSystem->Shape;
	const Matrix4& obj_mat = _particleSystem->GetTransform()->GetLocalToWorldMatrix();
	Matrix4 mat = Matrix4(shape.Position, Quaternion(shape.Rotation), shape.Scale) * obj_mat;

	//随机半径
	float radius = _particleSystem->RandomRange(0.0f, 1.0f);
	//随机角度
	float angle = _particleSystem->RandomRange(0.0f, Math::PI2);

	float x = radius * Math::Cos(angle);
	float y = radius * Math::Sin(angle);
	float z = 0;
	particle->Position = mat.TransformPoint(Vector3(x, y, z));

	//方向朝外面
	particle->Direction = (particle->Position - mat.GetTranslate()).Normalize();
}
void ParticleShape::EmitCircleEdge(Particle* particle)
{
	const ParticleSystem::ShapeModule& shape = _particleSystem->Shape;
	const Matrix4& obj_mat = _particleSystem->GetTransform()->GetLocalToWorldMatrix();
	Matrix4 mat = Matrix4(shape.Position, Quaternion(shape.Rotation), shape.Scale) * obj_mat;

	//随机角度
	float angle = _particleSystem->RandomRange(0.0f, Math::PI2);

	float x = Math::Cos(angle);
	float y = Math::Sin(angle);
	float z = 0;
	particle->Position = mat.TransformPoint(Vector3(x, y, z));

	//方向朝外面
	particle->Direction = (particle->Position - mat.GetTranslate()).Normalize();
}
//球的一种特殊形式
void ParticleShape::EmitCone(Particle* particle)
{
	const ParticleSystem::ShapeModule& shape = _particleSystem->Shape;
	const Matrix4& obj_mat = _particleSystem->GetTransform()->GetLocalToWorldMatrix();
	Matrix4 mat = Matrix4(shape.Position, Quaternion(shape.Rotation), shape.Scale) * obj_mat;

	//随机半径
	float radius = _particleSystem->RandomRange(0.0f, shape.Radius);
	//随机角度
	float angle_1 = _particleSystem->RandomRange(0.0f, Math::Deg2Rad * shape.Degree);
	float angle_2 = _particleSystem->RandomRange(0.0f, Math::PI2);

	float sin_angle_1 = Math::Sin(angle_1);
	float x = sin_angle_1 * Math::Cos(angle_2);
	float y = sin_angle_1 * Math::Sin(angle_2);
	float z = Math::Cos(angle_1);
	particle->Position = mat.TransformPoint(Vector3(x, y, z) * radius);

	//方向朝外面
	particle->Direction = (particle->Position - mat.GetTranslate()).Normalize();
}
//z轴正方向
void ParticleShape::EmitHemiSphere(Particle* particle)
{
	const ParticleSystem::ShapeModule& shape = _particleSystem->Shape;
	const Matrix4& obj_mat = _particleSystem->GetTransform()->GetLocalToWorldMatrix();
	Matrix4 mat = Matrix4(shape.Position, Quaternion(shape.Rotation), shape.Scale) * obj_mat;

	//随机半径
	float radius = _particleSystem->RandomRange(0.0f, shape.Radius);
	//随机角度
	float angle_1 = _particleSystem->RandomRange(0.0f, Math::PI);
	float angle_2 = _particleSystem->RandomRange(0.0f, Math::PI2);

	float sin_angle_1 = Math::Sin(angle_1);
	float cos_angle_1 = Math::Cos(angle_1);
	cos_angle_1 = cos_angle_1 < 0 ? -cos_angle_1 : cos_angle_1;
	float x = sin_angle_1 * Math::Cos(angle_2);
	float y = sin_angle_1 * Math::Sin(angle_2);
	float z = cos_angle_1;
	particle->Position = mat.TransformPoint(Vector3(x, y, z) * radius);

	//方向朝外面
	particle->Direction = (particle->Position - mat.GetTranslate()).Normalize();
}
void ParticleShape::EmitHemiSphereShell(Particle* particle)
{
	const ParticleSystem::ShapeModule& shape = _particleSystem->Shape;
	const Matrix4& obj_mat = _particleSystem->GetTransform()->GetLocalToWorldMatrix();
	Matrix4 mat = Matrix4(shape.Position, Quaternion(shape.Rotation), shape.Scale) * obj_mat;

	//随机角度
	float angle_1 = _particleSystem->RandomRange(0.0f, Math::PI);
	float angle_2 = _particleSystem->RandomRange(0.0f, Math::PI2);

	float sin_angle_1 = Math::Sin(angle_1);
	float cos_angle_1 = Math::Cos(angle_1);
	cos_angle_1 = cos_angle_1 < 0 ? -cos_angle_1 : cos_angle_1;
	float x = sin_angle_1 * Math::Cos(angle_2);
	float y = sin_angle_1 * Math::Sin(angle_2);
	float z = cos_angle_1;
	particle->Position = mat.TransformPoint(Vector3(x, y, z) * shape.Radius);

	//方向朝外面
	particle->Direction = (particle->Position - mat.GetTranslate()).Normalize();
}
/*
假设P（x，y，z）为空间内一点，则点P也可用这样三个有次序的数(r，θ，φ)来确定，其中r为原点O与点P间的距离；
	θ为有向线段OP与z轴正向的夹角；φ为从正z轴来看自x轴按逆时针方向转到OM所转过的角，这里M为点P在xOy面上的投影；
	这样的三个数r，θ，φ叫做点P的球面坐标，显然，这里r，θ，φ的变化范围为r∈[0,+∞)，θ∈[0, π]， φ∈[0,2π]
当r,θ或φ分别为常数时,可以表示如下特殊曲面:r = 常数，即以原点为心的球面；θ= 常数，即以原点为顶点、z轴为轴的圆锥面；φ= 常数，即过z轴的半平面。
x = rsinθcosφ.
y = rsinθsinφ.
z = rcosθ
*/
void ParticleShape::EmitSphere(Particle* particle)
{
	const ParticleSystem::ShapeModule& shape = _particleSystem->Shape;
	const Matrix4& obj_mat = _particleSystem->GetTransform()->GetLocalToWorldMatrix();
	Matrix4 mat = Matrix4(shape.Position, Quaternion(shape.Rotation), shape.Scale) * obj_mat;

	//随机半径
	float radius = _particleSystem->RandomRange(0.0f, shape.Radius);
	//随机角度
	float angle_1 = _particleSystem->RandomRange(0.0f, Math::PI);
	float angle_2 = _particleSystem->RandomRange(0.0f, Math::PI2);

	float sin_angle_1 = Math::Sin(angle_1);
	float x = sin_angle_1 * Math::Cos(angle_2);
	float y = sin_angle_1 * Math::Sin(angle_2);
	float z = Math::Cos(angle_1);
	particle->Position = mat.TransformPoint(Vector3(x, y, z) * radius);

	//方向朝外面
	particle->Direction = (particle->Position - mat.GetTranslate()).Normalize();
}
void ParticleShape::EmitSphereShell(Particle* particle)
{
	const ParticleSystem::ShapeModule& shape = _particleSystem->Shape;
	const Matrix4& obj_mat = _particleSystem->GetTransform()->GetLocalToWorldMatrix();
	Matrix4 mat = Matrix4(shape.Position, Quaternion(shape.Rotation), shape.Scale) * obj_mat;

	//随机角度
	float angle_1 = _particleSystem->RandomRange(0.0f, Math::PI);
	float angle_2 = _particleSystem->RandomRange(0.0f, Math::PI2);

	float sin_angle_1 = Math::Sin(angle_1);
	float x = sin_angle_1 * Math::Cos(angle_2);
	float y = sin_angle_1 * Math::Sin(angle_2);
	float z = Math::Cos(angle_1);
	particle->Position = mat.TransformPoint(Vector3(x, y, z) * shape.Radius);

	//方向朝外面
	particle->Direction = (particle->Position - mat.GetTranslate()).Normalize();
}
DC_END_NAMESPACE

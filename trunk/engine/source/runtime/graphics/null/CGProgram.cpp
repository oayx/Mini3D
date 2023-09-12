#include "CGProgram.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/graphics/ShadowMap.h"
#include "runtime/graphics/ReflectionProbe.h"
#include "runtime/project/QualitySettings.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/renderable/SkyBox.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/Light.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(CGProgram, Resource);
CGProgram* CGProgram::Create(const ShaderDesc& info)
{
	CGProgram* shader = Application::GetGraphics()->CreateShader();
	if (shader == nullptr)return nullptr;
	if (shader->LoadFromFile(info))
	{
		shader->AutoRelease();
		for (const auto& file : info.ShaderFile)
		{
			if (!file.IsEmpty())
			{
				shader->SetResFile(file);
				break;
			}
		}
		return shader;
	}
	else
	{
		SAFE_RELEASE(shader);
		return nullptr;
	}
}
CGProgram* CGProgram::CreateFromMemory(const String& name, const VecString& codes, const VecString& defines)
{
	CGProgram* shader = Application::GetGraphics()->CreateShader();
	if (shader->LoadFromMemory(name, codes, defines))
	{
		shader->AutoRelease();
		shader->SetResName(name);
		return shader;
	}
	else
	{
		SAFE_RELEASE(shader);
		return nullptr;
	}
}
bool CGProgram::LoadFromFile(const ShaderDesc& info)
{
	_reflectVertexSemantic = info.SRVertexSemantic;
	_reflectCBuffers = info.SRBuffers;
	_reflectVariables = info.SRVariables;
	_reflectTextures = info.SRTextures;

	return true;
}
void CGProgram::Render(Camera* camera, Pass* pass, const Matrix4& obj_mat)
{
	DC_PROFILE_FUNCTION();
	SetInternalTable(camera, pass, obj_mat);
	SetExtendTable(pass);
	SetMaterialTable(pass);
}
const sShaderReflectVariable* CGProgram::GetCBVariable(const String& name)const
{
	ShaderReflectVariables::const_iterator it = _reflectVariables.Find(name);
	if (it != _reflectVariables.end())
		return &(it->second);
	return nullptr;
}
const String& CGProgram::GetVariableFullName(const String& name)const
{
	auto it = _reflectVariables.Find(name);
	if (it == _reflectVariables.end())
		return name;
	return it->second.var_fullname;
}
bool CGProgram::GetShaderTextureInfo(const String& name, sShaderReflectTexture& info)const
{
	auto it = _reflectTextures.Find(name);
	if (it == _reflectTextures.end())
		return false;
	info = it->second;
	return true;
}
void CGProgram::SetInternalTable(Camera* camera, Pass* pass, const Matrix4& obj_mat)
{
	DC_PROFILE_FUNCTION();
	Matrix4 mat;
	Matrix4 mat_world = obj_mat;
	Matrix4 mat_view = Matrix4::identity;
	Matrix4 mat_proj = Matrix4::identity;
	Vector3 eye_pos = Vector3::zero;
	Vector3 eye_dir = Vector3::forward;
	if (camera != nullptr)
	{
		mat_view = camera->GetViewMatrix();
		mat_proj = camera->GetProjMatrix();
		eye_pos = camera->GetTransform()->GetPosition();
		eye_dir = camera->GetTransform()->GetForward();
		
		float z_near = camera->GetZNear(), z_far = camera->GetZFar();
		Vector4 _ProjectionParams(1, z_near, z_far, 1.0f / z_far);
		SetVariable("_ProjectionParams", _ProjectionParams.ptr(), 4);

		float target_w = (float)camera->GetRenderTargetWidth(), target_h = (float)camera->GetRenderTargetHeight();
		Vector4 _ScreenParams(target_w, target_h, 1.0f+1.0f/ target_w, 1.0f + 1.0f / target_h);
		SetVariable("_ScreenParams", _ScreenParams.ptr(), 4);

		float x = 1 - z_far / z_near, y = z_far / z_near, z = x / z_far, w = y / z_far;
		Vector4 _ZBufferParams(x, y, z, w);
		SetVariable("_ZBufferParams", _ScreenParams.ptr(), 4);
	}
	SetVariable("MATRIX_V", mat_view);//当前观察矩阵，用于将顶点/方向矢量从世界空间变换到观察空间
	SetVariable("MATRIX_P", mat_proj);//当前投影矩阵，用于将顶点/方向矢量从观察空间变换到裁剪空间

	mat = mat_world * mat_view * mat_proj;
	SetVariable("MATRIX_MVP", mat);//当前模型*观察*投影矩阵，用于将模型顶点/方向矢量从模型空间转换到裁剪空间

	mat = mat_world * mat_view;
	SetVariable("MATRIX_MV", mat);//当前模型*观察矩阵，用于将模型顶点/方向矢量从模型空间转换到观察空间
	SetVariable("MATRIX_T_MV", mat.Transpose());//转置矩阵
	
	mat = mat_world.InverseTranspose();
	SetVariable("MATRIX_IT_M", mat); //逆转置矩阵，可将法线矢量从模型空间转换到世界空间
	mat = mat * mat_view;
	SetVariable("MATRIX_IT_MV", mat);//逆转置矩阵，可将法线矢量从模型空间转换到观察空间(之所以法线不能直接使用MATRIX_MV进行变换，是因为法线是向量，具有方向，在进行空间变换的时候，如果发生非等比缩放，方向会发生偏移)
	
	mat = mat_view * mat_proj;
	SetVariable("MATRIX_VP", mat);//当前观察*投影矩阵，用于将顶点/方向矢量从世界空间变换到裁剪空间
	SetVariable("MATRIX_I_VP", mat.Inverse());

	mat = mat_world;
	SetVariable("MATRIX_M", mat);//当前模型的矩阵，用于将模型顶点/方向矢量从模型空间转换到世界空间
	SetVariable("MATRIX_I_M", mat.Inverse());//世界空间->模型空间
	SetVariable("MATRIX_T_M", mat.Transpose());
	
	SetVariable("_WorldSpaceCameraPos", eye_pos.ptr(), 3);//相机位置
	SetVariable("_WorldSpaceCameraDir", eye_dir.ptr(), 3);//相机朝向

	//时间
	float time = Time::GetTime();
	Vector4 _Time(time / 20.0f, time, time*2.0f, time*3.0f);
	SetVariable("_Time", _Time.ptr(), 4);
	float delta_time = Time::GetDeltaTime();
	float unscale_delta_time = Time::GetUnscaledDeltaTime();
	Vector4 _DeltaTime(delta_time, 1.0f / delta_time, unscale_delta_time, 1.0f / unscale_delta_time);
	SetVariable("_DeltaTime", _DeltaTime.ptr(), 4);
	float sin_time = Math::Sin(time);
	Vector4 _SinTime(sin_time / 8.0f, sin_time / 4.0f, sin_time / 2.0f, sin_time);
	SetVariable("_SinTime", _SinTime.ptr(), 4);
	float cos_time = Math::Cos(time);
	Vector4 _CosTime(cos_time / 8.0f, cos_time / 4.0f, cos_time / 2.0f, cos_time);
	SetVariable("_CosTime", _CosTime.ptr(), 4);

	//贴图
	for (byte i = 0; i < pass->GetTexUnitCount(); i++)
	{
		TextureUnit *tex_unit = pass->GetTexUnit(i);
		if (tex_unit != nullptr)
		{
			sShaderReflectTexture reflect_texture;
			if (GetShaderTextureInfo(tex_unit->Name, reflect_texture))
			{
				SetVariable(reflect_texture.slot, tex_unit);
			}
		}
	}

	//灯光
	Scene* scene = SceneManager::GetCurrScene();
	Material* material = pass->GetMaterial();
	Shader* shader = material->GetShader();
	if (shader->EnableLight())
	{
		const List<Light*>& lightes = material->GetLightes();
		int light_count = lightes.Size();
		SetVariable("_LightCount", &light_count, 1);
		for (int i = 0; i < MAX_DIRECTION_LIGHT + MAX_POINT_LIGHT + MAX_SPOT_LIGHT; ++i)
		{
			Light* light = nullptr;
			if (i < lightes.Size())light = lightes.Get(i);

			ShaderLight shader_light;
			if (light && light->IsEnable() && light->GetGameObject()->ActiveInHierarchy())
			{
				Transform* transform = light->GetTransform();
				const Vector3& light_pos = transform->GetPosition();
				const Vector3& light_dir = transform->GetForward();

				shader_light.Type = (int)light->mType;
				shader_light.Dir = light_dir;
				shader_light.Color = Vector3(light->Diffuse.r, light->Diffuse.g, light->Diffuse.b) * light->Intensity;
				switch (light->mType)
				{
				case LightType::Direction:
				{
					shader_light.Position = -light_dir;
				}
				break;
				case LightType::Point:
				{
					shader_light.Position = light_pos;
					shader_light.Range = light->Range;
					shader_light.Attenuation = light->Attenuation;
				}
				break;
				case LightType::Spot:
				{
					shader_light.Position = light_pos;
					shader_light.Range = light->Range;
					shader_light.Attenuation = light->Attenuation;
					shader_light.SpotAngle = light->SpotAngle;
				}
				break;
				}
			}
			if (i == 0)
			{
				if (light && light->mType == LightType::Direction)
				{
					Color color = light->Diffuse *light->Intensity;
					Transform* transform = light->GetTransform();
					SetVariable("_LightDir0", transform->GetForward().ptr(), 3);
					SetVariable("_LightColor0", color.ptr(), 3);
				}
				else
				{
					SetVariable("_LightDir0", Vector3::forward.ptr(), 3);
					SetVariable("_LightColor0", Color::Black.ptr(), 3);
				}
			}
			SetVariable("_Light", shader_light, i);
		}

		//环境色
		Color ambient_color = Color::Black;
		if (scene)ambient_color = scene->GetAmbientColor();
		SetVariable("_LIGHTMODEL_AMBIENT", ambient_color.ptr(), 3);

		//阴影
		for (const auto& light : lightes)
		{
			if (light->GetValidShadowType() == ShadowType::None)continue;
			RenderTexture* render_texture = light->GetShadowMap() == nullptr ? nullptr : light->GetShadowMap()->GetTexture();
			if (camera && render_texture && material->IsReceiveShadow())
			{
				int shadow_type = (int)light->GetValidShadowType();
				SetVariable("_ShadowType", &shadow_type, 1);

				Transform* light_transform = light->GetTransform();
				sShaderReflectTexture reflect_texture;
				if (GetShaderTextureInfo("_ShadowMap", reflect_texture))
				{
					SetVariable(reflect_texture.slot, "_ShadowMap", render_texture);
				}
				Matrix4 mat_light_view, mat_light_proj;
				if (light->mType == LightType::Direction)
				{
					Aabb aabb = camera->GetRenderableBoundingBox();
					const Vector3& camera_position = camera->GetTransform()->GetPosition();
					mat_light_view = Matrix4::LookTo(Vector3(camera_position.x, aabb.GetMaximum().y + 100.0f, camera_position.z), light_transform->GetForward(), Vector3::up);
					aabb = aabb * mat_light_view;
					float minX = aabb.GetMinimum().x;
					float maxX = aabb.GetMaximum().x;
					float minY = aabb.GetMinimum().y;
					float maxY = aabb.GetMaximum().y;
					float minZ = aabb.GetMinimum().z;
					float maxZ = aabb.GetMaximum().z;
					float w = maxX - minX, h = maxY - minY;
					float size = Math::Max<float>(w, h);
					size = Math::Clamp(size, 30.0f, QualitySettings::GetShadowDistance());//加上偏移是防止阴影在边缘时拉长
					mat_light_proj = Matrix4::Ortho(size, size, -10.0f, 1000.0f);
				}
				else
				{
					mat_light_view = Matrix4::LookTo(light_transform->GetPosition(), light_transform->GetForward(), Vector3::up);
					mat_light_proj = mat_proj;
				}
				mat = mat_world * mat_light_view * mat_light_proj;
				SetVariable("MATRIX_MVP_LIGHT", mat);
				SetVariable("_Strength", light->GetShadowStrength());
				SetVariable("_Bias", light->GetShadowBias());
			}
		}
	}
	
	//Fog
	if(scene && scene->GetFog().enable)
	{ 
		const FogDesc& fog_desc = scene->GetFog();
		SetVariable("_FogMode", int(fog_desc.mode) + 1);
		SetVariable("_FogColor", fog_desc.color.p, 4);
		if (fog_desc.mode == FogMode::Linear)
		{
			SetVariable("_FogStart", fog_desc.start);
			SetVariable("_FogEnd", fog_desc.end);
		}
		else
		{
			SetVariable("_FogDensity", fog_desc.density);
		}
	}
	else
	{
		SetVariable("_FogMode", 0);
	}

	//深度图
	if (camera && camera->GetDepthTextureMode() != DepthTextureMode::None)
	{
		DepthTextureMode depth_mode = camera->GetDepthTextureMode();
		sShaderReflectTexture reflect_texture;
		if (GetShaderTextureInfo("_CameraDepthTexture", reflect_texture))
		{
			if (depth_mode & DepthTextureMode::Depth || depth_mode & DepthTextureMode::Normals)
				SetVariable(reflect_texture.slot, "_CameraDepthTexture", camera->GetDepthTexture());
			if (depth_mode & DepthTextureMode::Normals)
				SetVariable(reflect_texture.slot, "_CameraDepthNormalTexture", camera->GetDepthTexture());
		}
		if (GetShaderTextureInfo("_CameraDepthNormalTexture", reflect_texture))
		{
			if (depth_mode & DepthTextureMode::Normals)
				SetVariable(reflect_texture.slot, "_CameraDepthNormalTexture", camera->GetDepthTexture());
		}
	}

	//天空盒
	if (camera && camera->GetSkyBoxTexture())
	{
		sShaderReflectTexture reflect_texture;
		if (GetShaderTextureInfo("_SkyBox", reflect_texture))
		{
			SetVariable(reflect_texture.slot, "_SkyBox", camera->GetSkyBoxTexture());
		}
	}

	//反射
	if (camera && !camera->IsSame<ReflectionProbe>())
	{//不能是反射探头相机渲染的
		sShaderReflectTexture reflect_texture;
		if (GetShaderTextureInfo("_ReflectionProbe", reflect_texture))
		{//TODO 目前只取第一个反射探头
			ReflectionProbe* probe = SceneManager::GetProbe();
			if (probe)
			{
				SetVariable(reflect_texture.slot, "_ReflectionProbe", probe->GetRenderTexture());
			}
		}
	}
}
void CGProgram::SetExtendTable(Pass* pass)
{
	DC_PROFILE_FUNCTION();
	if (pass == nullptr)return;
	int count = pass->GetMaterial()->GetVariableCount();
	for (int i = 0; i < count; ++i)
	{//NOTE:这里使用材质的变量
		const ShaderVariable& var = pass->GetMaterial()->GetVariable(i);
		switch (var.Type)
		{
		case ShaderVariableType::Int:
		{
			int v = var.Value.ToInt();
			SetVariable(var.Name, &v, 1);
		}
		break;
		case ShaderVariableType::Float:
		{
			float v = var.Value.ToFloat();
			SetVariable(var.Name, &v, 1);
		}
		break;
		case ShaderVariableType::Bool:
		{
			bool b = Bool::FromString(var.Value);
			SetVariable(var.Name, b);
		}
		break;
		case ShaderVariableType::Slider:
		{
			float v = var.Value.ToFloat();
			v = Math::Clamp(v, var.Range.x, var.Range.y);
			SetVariable(var.Name, &v, 1);
		}
		break;
		case ShaderVariableType::Color:
		{
			Vector4 color(var.Value);
			SetVariable(var.Name, color.p, 4);
		}
		break;
		case ShaderVariableType::Vector:
		{
			Vector4 vec(var.Value);
			SetVariable(var.Name, vec.p, 4);
		}
		break;
		case ShaderVariableType::Rect:
		{
			Vector4 rect(var.Value);
			SetVariable(var.Name, rect.p, 4);
		}
		break;
		}
	}
}
void CGProgram::SetMaterialTable(Pass* pass)
{
	DC_PROFILE_FUNCTION();
	if (pass == nullptr)return;
	Material* material = pass->GetMaterial();
	for (const auto& it : material->_shaderIntTable)
	{
		SetVariable(it.first, &it.second, 1);
	}
	for (const auto& it : material->_shaderFloatTable)
	{
		SetVariable(it.first, &it.second, 1);
	}
	for (const auto& it : material->_shaderColorTable)
	{
		SetVariable(it.first, it.second.ptr(), 4);
	}
	for (const auto& it : material->_shaderVector4Table)
	{
		SetVariable(it.first, it.second.ptr(), 4);
	}
	for (const auto& it : material->_shaderMatrix4Table)
	{
		SetVariable(it.first, it.second);
	}
	for (const auto& it : material->_shaderFloatsTable)
	{
		if (it.second.Size() > 0)SetVariable(it.first, (float*)(&it.second[0]), it.second.Size());//取第一个元素地址
	}
	for (const auto& it : material->_shaderVector4sTable)
	{
		if ((int)it.second.Size() > 0)SetVariable(it.first, (Vector4*)(&it.second[0]), it.second.Size());
	}
	for (const auto& it : material->_shaderMatrix4sTable)
	{
		if ((int)it.second.Size() > 0)SetVariable(it.first, (Matrix4*)(&it.second[0]), it.second.Size());
	}
}
DC_END_NAMESPACE
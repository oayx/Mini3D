
/*****************************************************************************
* Author： hannibal
* Description：单个场景
*****************************************************************************/
#pragma once

#include "runtime/graphics/RenderDefine.h"

DC_BEGIN_NAMESPACE
class GameObject;
class Transform;
/********************************************************************/
class ENGINE_DLL Scene : public Object
{
	friend class SceneManager;

	DEFAULT_CREATE(Scene);
	FRIEND_CONSTRUCT_DESTRUCT(Scene);
	DISALLOW_COPY_ASSIGN(Scene);
	DECLARE_OBJECT_SERIALIZE(Scene);
	BEGIN_DERIVED_REFECTION_TYPE(Scene, Object)
	END_REFECTION_TYPE;

	Scene(const String& name);
	~Scene();

private:
	bool Load(const String& file);

	bool Save();
	bool SaveAs(const String& file);
	bool SaveToFile(const String& file);

	void CreateDefault();

	virtual void Serialize()override;
	virtual void Deserialize()override;

public:
	const String&	GetName()const { return _name; }
	const String&	GetFile()const { return _file; }

	GameObject*		GetRootObject()const { return _rootObject; }
	Transform*		GetRootNode()const;

	//环境色
	void			SetAmbientColor(const Color& color) { _ambientColor = color; }
	const Color&	GetAmbientColor() { return _ambientColor; }

	//天空盒
	const String&	GetSkyboxMaterial()const { return _skyboxMaterial; }
	void			SetSkyboxMaterial(const String& file) { _skyboxMaterial = file; }

	//fog
	void			SetFog(const FogDesc& desc) { _fogDesc = desc; }
	void			SetFogEnable(bool b) { _fogDesc.enable = b; }
	void			SetFogColor(const Color& color) { _fogDesc.color = color; }
	void			SetFogMode(FogMode mode) { _fogDesc.mode = mode; }
	void			SetFogRange(float start, float end) { _fogDesc.start = start; _fogDesc.end = end; }
	void			SetFogDensity(float density) { _fogDesc.density = density; }
	const FogDesc&	GetFog() { return _fogDesc; }

	float			GetReflectionsIntensity()const { return _reflectionsIntensity; }
	void			SetReflectionsIntensity(float intensity) { _reflectionsIntensity = intensity; }

private:
	String			_name = "";
	String			_file = "";
	GameObject*		_rootObject = nullptr;

	Color			_ambientColor = Color(0.2f, 0.2f, 0.2f);
	String			_skyboxMaterial = "internal/material/SkyBox.material";
	FogDesc			_fogDesc;

	float			_reflectionsIntensity = 0.5f;	//反射强度
};
DC_END_NAMESPACE
#include "QualitySettings.h"
#include "runtime/resources/Resources.h"
#include "runtime/scene/Light.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
static const String QualityFilePath = "settings/QualitySettings.asset";
/********************************************************************/
void QualitySettings::Load()
{
	String fullPath = Resource::GetFullDataPath(QualityFilePath);
	if (!File::Exist(fullPath))
	{
		return Save();
	}

	SerializeRead transfer(fullPath);
	TRANSFER_ENUM(_mSAAType);
	_mSAAType = (MSAAType)Math::Clamp(int(_mSAAType), 1, int(MSAAType::Level_8));

	TRANSFER_ENUM(_shadowType);
	TRANSFER_ENUM(_shadowResolution);
	TRANSFER_SIMPLE(_shadowDistance);
}
void QualitySettings::Save()
{
	String fullPath = Resource::GetFullDataPath(QualityFilePath);
	SerializeWrite transfer(fullPath);
	TRANSFER_ENUM(_mSAAType);

	TRANSFER_ENUM(_shadowType);
	TRANSFER_ENUM(_shadowResolution);
	TRANSFER_SIMPLE(_shadowDistance);
}
void QualitySettings::SetShadowType(ShadowType type)
{ 
	if (type == _shadowType)return;

	_shadowType = type;
	for (int i = 0; i < SceneManager::GetLightCount(); ++i)
	{
		Light* light = SceneManager::GetLight(i);
		if (light && light->GetShadowType() != ShadowType::None)
		{
			if (_shadowType == ShadowType::None)
			{
				light->DisableShadowMap();
			}
			else if(!light->GetShadowMap())
			{
				light->EnableShadowMap();
			}
		}
	}
	Save();
}
void QualitySettings::SetShadowResolution(ShadowResolution resolution) 
{
	if (resolution == _shadowResolution)return;

	_shadowResolution = resolution;
	for (int i = 0; i < SceneManager::GetLightCount(); ++i)
	{
		Light* light = SceneManager::GetLight(i);
		if (light && light->GetShadowType() != ShadowType::None)
		{
			light->EnableShadowMap();
		}
	}
	Save();
}
DC_END_NAMESPACE
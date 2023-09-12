#include "QualitySettings.h"
#include "runtime/resources/Resources.h"
#include "runtime/scene/Light.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
static const String QualityFilePath = "settings/QualitySettings.asset";
/********************************************************************/
MSAAType QualitySettings::_mSAAType = MSAAType::Disabled;
float QualitySettings::_shadowDistance = 200.0f;
ShadowType QualitySettings::_shadowType = ShadowType::Soft;
ShadowResolution QualitySettings::_shadowResolution = ShadowResolution::Medium;
bool QualitySettings::_enableHDR = true;
float QualitySettings::_hDRExposure = 0.8f;
ColorFormat	QualitySettings::_hDRFormat = ColorFormat::RGBA16F;
void QualitySettings::Load()
{
	String full_path = Resource::GetFullDataPath(QualityFilePath);
	if (!File::Exist(full_path))
	{
		return Save();
	}

	SerializeRead transfer(full_path);
	TRANSFER_ENUM(_mSAAType);
	_mSAAType = (MSAAType)Math::Clamp(int(_mSAAType), 1, int(MSAAType::Level_8));

	TRANSFER_ENUM(_shadowType);
	TRANSFER_ENUM(_shadowResolution);
	TRANSFER_SIMPLE(_shadowDistance);
}
void QualitySettings::Save()
{
	String full_path = Resource::GetFullDataPath(QualityFilePath);
	SerializeWrite transfer(full_path);
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
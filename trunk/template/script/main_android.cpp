
#include "engine_header.h"
using namespace dc;

USING_NAMESPACE_DC;
#if defined(PLATFORM_ANDROID)
#include <jni.h>
extern "C"
{
	void Java_cn_oayx_engine_JNI_engineCreateed(JNIEnv* env, jobject obj)
	{
		Debuger::Log("engineCreateed");

		//相机
		{
			GameObject* camera_obj = GameObject::Instance("MainCamera");
			camera_obj->GetTransform()->SetLocalPosition(Vector3(0, 0, -10));
			Camera* camera = camera_obj->AddComponent<Camera>();
			camera->SetAspect(Screen::GetWidth() / Screen::GetHeight());
			camera->SetBackgroundColor(Color(0.5f, 0.5f, 0.5f, 0));
			uint mask = Math::RemoveFlag(LayerMask::Everything, LayerMask::GetMask(LayerMask::UI));
			camera->SetCullMask(mask);
		}

		Application::Start();
	}
}
#endif
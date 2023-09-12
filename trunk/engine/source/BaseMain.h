#pragma once

#include "runtime/component/Component.h"
#include "runtime/CommonDefine.h"
#include "runtime/CommandLine.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE

#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX)
	#define  DLCARE_MAIN(classname, editor) \
		USING_NAMESPACE_DC;\
		int main(int argc, char* argv[])\
		{\
			CommandLine::Pause(argc, argv);\
			Application::Initialize(editor);\
			GameObject* root_object = SceneManager::GetEngineObject();\
			MainComponent* com = dynamic_cast<MainComponent*>(root_object->AddComponent<classname>());\
			Vector<WindowDesc> descs;\
			com->GetWindowDesc(descs);\
			for (auto& desc : descs)\
			{\
				Application::CreateRenderWindow(desc);\
			}\
			Application::Start();\
		}
#elif defined(DC_PLATFORM_ANDROID)
	#include <jni.h>
	#define  DLCARE_MAIN(classname, editor) \
		USING_NAMESPACE_DC;\
		extern "C"  \
		{ \
			void Java_cn_oayx_engine_JNI_engineCreateed(JNIEnv* env, jobject obj) \
			{ \
				Debuger::Log("engineCreateed"); \
				GameObject* root_object = GameObject::Instantiation("EngineNode");\
				MainComponent* com = dynamic_cast<MainComponent*>(root_object->AddComponent<classname>());\
				Application::Start();\
			} \
		}
#else
	#define  DLCARE_MAIN(classname)
#endif

class ENGINE_DLL MainComponent : public Component
{
	FRIEND_CONSTRUCT_DESTRUCT(MainComponent);
public:
	MainComponent() = default;
	~MainComponent() = default;
	virtual void GetWindowDesc(Vector<WindowDesc>& descs) = 0;
};

DC_END_NAMESPACE
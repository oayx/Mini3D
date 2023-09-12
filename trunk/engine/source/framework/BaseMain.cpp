#include "BaseMain.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
static void Initialize();
static void Shutdown();
#if defined(PLATFORM_WIN32)
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	Initialize();

	return 1;
}
#endif
#if defined(PLATFORM_WIN32) || defined(PLATFORM_LINUX)
int main(int argc, char* argv[])
{
	Initialize();

	return 1;
}
#endif
#if defined(PLATFORM_ANDROID)

#endif

void Initialize()
{
	Application::GetDataPath();
	BaseMain::m_instance->OnInitialize();
}
void Shutdown()
{
	if (BaseMain::m_instance)
	{
		BaseMain::m_instance->OnShutdown();
		SAFE_DELETE(BaseMain::m_instance);
	}
}
/********************************************************************/
BaseMain* BaseMain::m_instance = nullptr;
MainFunction BaseMain::m_InitializeFunc = nullptr;
MainFunction BaseMain::m_ShutdownFunc = nullptr;
void BaseMain::OnInitialize()
{

}
void BaseMain::OnShutdown()
{

}
DC_END_NAMESPACE
#pragma once

#include "runtime/Application.h"

DC_BEGIN_NAMESPACE

typedef bool(*MainFunction)();

#define DLCARE_APPLICATION(classname)\
	public:\
		static bool RegisterMainFactory();\
	private:\
		static bool Initialize();\
		static bool Shutdown();\
		static bool m_RegisterMainFactory;

#define IMPLEMENT_APPLICATION(classname)\
	static bool g_bStreamRegistered_classname= classname::RegisterMainFactory ();\
 	bool classname::m_RegisterMainFactory = false;\
	bool classname::RegisterMainFactory()\
	{\
		if (!m_RegisterMainFactory)\
		{\
			BaseMain::AddInitialFuntion(classname::Initialize);\
			BaseMain::AddShutdownFuntion(classname::Shutdown);\
			m_RegisterMainFactory = true;\
		}\
		return m_RegisterMainFactory;\
	}\
	bool classname::Initialize()\
	{\
		classname::m_instance = DBG_NEW classname();\
		if(!classname::m_instance)\
			return false;\
		return true;\
	}\
	bool classname::Shutdown()\
	{\
		if (classname::m_instance)\
		{\
			SAFE_DELETE(classname::m_instance);\
		}\
		return true;\
	}

class BaseMain
{

public:
	static void AddInitialFuntion(MainFunction func) { m_InitializeFunc = func; }
	static void AddShutdownFuntion(MainFunction func) { m_ShutdownFunc = func; }

public:
	virtual void OnInitialize() = 0;
	virtual void OnShutdown() = 0;

	static BaseMain* m_instance;
	static MainFunction m_InitializeFunc;
	static MainFunction m_ShutdownFunc;
};
DC_END_NAMESPACE
 
#pragma once

#include "engine_header.h"

DC_BEGIN_NAMESPACE

typedef Vector<std::pair<String, std::function<void()>>> MenuCallbacks;

class TestBase : public Component
{
protected:
	MenuCallbacks _menus;
	bool _cameraCanMove = false;
	GameObject* _rootObject = nullptr;
	Vector<uint> _timers;
	std::function<void()> _gUICallback = nullptr;

	float _moveScale = 1.0f;
	float _rotateScale = 1.0f;

public:
	TestBase()
	{
	}
	~TestBase()
	{
	}
	//相机是否会移动
	bool CameraCanMove()const { return _cameraCanMove; }
	void SetCameraCanMove(bool b) { _cameraCanMove = b; }
	void SetCameraSpeed(float s)
	{
		_moveScale = s;
	}

public:
	virtual void OnEnter(int child_unit_index)
	{
		_gUICallback = nullptr;
		this->ResetCamera();
		this->SetCameraCanMove(false);
	}
	virtual void OnLeave()
	{
		for (auto timer_id : _timers)
		{
			Timer::RemoveTimer(timer_id);
		}
		_timers.Clear();

		if (_rootObject)
		{
			GameObject::Destroy(_rootObject);
			_rootObject = nullptr;
		}
		this->ResetCamera();
		_gUICallback = nullptr;
	}
	virtual String GetTitle() = 0;
	virtual MenuCallbacks GetMenus()const { return _menus; }
	virtual void OnGUI()override
	{
		if (_gUICallback)
		{
			_gUICallback();
		}
	}
	//重置相机
	virtual void ResetCamera()
	{
		Camera* camera = SceneManager::GetMainCamera();
		camera->GetTransform()->SetLocalPosition(Vector3(0, 0, -10));
		camera->GetTransform()->LookTo(Vector3::forward, Vector3::up);
		camera->SetClearColor(Color(0.5f, 0.5f, 0.5f, 0));
		camera->SetZNear(0.1f);
		camera->SetZFar(1000.0f);
		TweenManager::Stop(camera->GetTransform());
		SetCameraSpeed(1.0f);
	}

	virtual void AddGameObject(GameObject* obj)
	{
		if (_rootObject && obj)
		{
			obj->GetTransform()->SetParent(_rootObject->GetTransform(), false);
		}
	}
};
DC_END_NAMESPACE

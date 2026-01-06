#include "UIImageAnimation.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UIImageAnimation, UIImage);
Object* UIImageAnimation::Clone(Object* newObj)
{
	base::Clone(newObj);
	UIImageAnimation* obj = dynamic_cast<UIImageAnimation*>(newObj);
	if (!obj)return newObj;

	for (const auto& p : _atlases)
	{
		obj->AddImage(p.first, p.second);
	}

	obj->SetNativeSize(_setNativeSize);
	obj->SetFPS(_fPS);
	obj->SetLoop(_isLoop);
	obj->SetFoward(_isFoward);
	obj->SetSelfUpdate(_isSelfUpdate);

	return obj;
}
void UIImageAnimation::Start()
{
	base::Start();
	if (_isAutoPlay)
	{
		this->Play();
	}
	else
	{
		_isPlaying = false;
	}
}
void UIImageAnimation::OnEnable()
{
	if (_isPlaying)
	{
		this->Rewind();
	}
	else
	{
		_isPlaying = false;
	}
	base::OnEnable();
}
void UIImageAnimation::OnDisable()
{
	this->Stop();
	base::OnDisable();
}
void UIImageAnimation::Update()
{
	if (_isSelfUpdate)
	{
		Simulate(Time::GetDeltaTime());
	}
	base::Update();
}
void UIImageAnimation::Simulate(float detal_time)
{
	if (!_isPlaying || 0 == GetFrameCount())
	{
		return;
	}
	_deltaTime += detal_time;
	if (_deltaTime > 1.0f / (float)_fPS)
	{
		_deltaTime = 0;
		if (_isFoward)
		{
			_curFrame++;
		}
		else
		{
			_curFrame--;
		}
		if (_curFrame >= GetFrameCount())
		{
			if (_isLoop)
			{
				_curFrame = 0;
			}
			else
			{
				this->OnStop();
				return;
			}
		}
		else if (_curFrame < 0)
		{
			if (_isLoop)
			{
				_curFrame = GetFrameCount() - 1;
			}
			else
			{
				this->OnStop();
				return;
			}
		}
		SetFrame(_curFrame);
	}
}
void UIImageAnimation::Play()
{
	_isPlaying = true;
}
void UIImageAnimation::Rewind()
{
	if (_isFoward)
		_curFrame = 0;
	else
		_curFrame = GetFrameCount() - 1;
	SetFrame(_curFrame);
	Play();
}
void UIImageAnimation::Stop()
{
	_curFrame = 0;
	SetFrame(_curFrame);
	_isPlaying = false;
}
void UIImageAnimation::Pause()
{
	_isPlaying = false;
}
void UIImageAnimation::Resume()
{
	if (!_isPlaying)
	{
		_isPlaying = true;
	}
}
void UIImageAnimation::OnStop()
{
	_isPlaying = false;
	if (_completeFun != nullptr)
	{
		_completeFun();
	}
}
void UIImageAnimation::SetFrame(int frame)
{
	AssertEx(frame >= 0 && frame < _atlases.Size(), "image index out range");
	auto atlas = _atlases[frame];
	SetAtlas(atlas.first, atlas.second);
	if (_setNativeSize)
	{
		base::SetNativeSize();
	}
}
DC_END_NAMESPACE

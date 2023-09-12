#pragma once

#include "Component.h"
#include "GameObject.h"

DC_BEGIN_NAMESPACE
template <class T> T* Component::GetComponent()const
{
	if (_gameObject == nullptr)return nullptr;
	return _gameObject->GetComponent<T>();
}
DC_END_NAMESPACE

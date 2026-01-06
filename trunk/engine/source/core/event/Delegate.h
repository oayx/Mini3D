
/*****************************************************************************************************/
// @author hannibal
// @date   2021/02/29
// @brief  代理，实现了类似C# delegate功能
/*****************************************************************************************************/
#pragma once

#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include "core/BaseMacro.h"

DC_BEGIN_NAMESPACE
template<typename>
class Function;
template<typename... Params>
class Delegate;
/********************************************************************/
template<typename ReturnType, typename... Params>
class Function<ReturnType(Params ...)> final
{
	friend Delegate<Params...>;
public:
	typedef ReturnType(*Signature)(Params ...);

private:

	typedef ReturnType(*StubSignature)(void*, Params ...);

	template<ReturnType(*Method)(Params ...)>
	static ReturnType StaticMethodStub(void*, Params ... params)
	{
		return (Method)(std::forward<Params>(params)...);
	}

	static ReturnType StaticPointerMethodStub(void* callee, Params ... params)
	{
		return reinterpret_cast<Signature>(callee)(std::forward<Params>(params)...);
	}

	template<class T, ReturnType(T::*Method)(Params ...)>
	static ReturnType ClassMethodStub(void* callee, Params ... params)
	{
		return (reinterpret_cast<T*>(callee)->*Method)(std::forward<Params>(params)...);
	}

	template<class T, ReturnType(T::*Method)(Params ...) const>
	static ReturnType ClassMethodStub(void* callee, Params ... params)
	{
		return (reinterpret_cast<T*>(callee)->*Method)(std::forward<Params>(params)...);
	}

	void* _callee;
	StubSignature _function;

public:
	Function()
	{
		_callee = nullptr;
		_function = nullptr;
	}

	Function(Signature method)
	{
		_callee = (void*)method;
		_function = &StaticPointerMethodStub;
	}

public:
	template<ReturnType(*Method)(Params ...)>
	void Bind()
	{
		_callee = nullptr;
		_function = &StaticMethodStub<Method>;
	}
	template<class T, ReturnType(T::*Method)(Params ...)>
	void Bind(T* callee)
	{
		_callee = callee;
		_function = &ClassMethodStub<T, Method>;
	}
	void Bind(Signature method)
	{
		_callee = (void*)method;
		_function = &StaticPointerMethodStub;
	}
	void Unbind()
	{
		_callee = nullptr;
		_function = nullptr;
	}

public:
	bool IsBinded() const
	{
		return _function != nullptr;
	}
	void TryCall(Params ... params) const
	{
		if (_function)
			_function(_callee, std::forward<Params>(params)...);
	}
	ReturnType operator()(Params ... params) const
	{
		return _function(_callee, std::forward<Params>(params)...);
	}

	bool operator==(const Function& other) const
	{
		return _function == other._function && _callee == other._callee;
	}

	bool operator!=(const Function& other) const
	{
		return _function != other._function || _callee != other._callee;
	}
};

template<typename... Params>
class Delegate final
{
public:
	Delegate()
	{
	}
	~Delegate()
	{
		::free((void*)_ptr);
	}
	DISALLOW_COPY_ASSIGN(Delegate);

public:
	typedef void(*Signature)(Params ...);
	using FunctionType = Function<void(Params ...)>;

protected:
	intptr_t _ptr = 0;
	intptr_t _size = 0;
	typedef void(*StubSignature)(void*, Params ...);

public:
	template<void(*Method)(Params ...)>
	void Bind()
	{
		FunctionType f;
		f.template Bind<Method>();
		Bind(f);
	}
	template<class T, void(T::*Method)(Params ...)>
	void Bind(T* callee)
	{
		FunctionType f;
		f.template Bind<T, Method>(callee);
		Bind(f);
	}
	void Bind(Signature method)
	{
		FunctionType f(method);
		Bind(f);
	}
	void Bind(const FunctionType& f)
	{
		const intptr_t size = _size;
		FunctionType* bindings = (FunctionType*)_ptr;
		if (bindings)
		{
			// 看看是否有空闲的槽位
			for (intptr_t i = 0; i < size; i++)
			{
				if (bindings[i]._function == 0)
				{
					bindings[i]._function = f._function;
					bindings[i]._callee = f._callee;
					return;
				}
			}
		}

		// 如果执行到这，可能是以前没创建过或者之前的容量不够，新创建一个替换旧的
		const intptr_t newSize = size ? size * 2 : 5;
		auto newBindings = (FunctionType*)::malloc(newSize * sizeof(FunctionType));
		::memcpy(newBindings, bindings, size * sizeof(FunctionType));
		::memset(newBindings + size, 0, (newSize - size) * sizeof(FunctionType));

		// 设置到槽位
		newBindings[size] = f;

		// 设置新的链表
		_ptr = (intptr_t)newBindings;

		//成功替换，ptr已经是新创建的链表，释放旧的
		_size = newSize;
		if(bindings)::free((void*)bindings);
	}
	template<void(*Method)(Params ...)>
	void Unbind()
	{
		FunctionType f;
		f.template Bind<Method>();
		Unbind(f);
	}
	template<class T, void(T::*Method)(Params ...)>
	void Unbind(T* callee)
	{
		FunctionType f;
		f.template Bind<T, Method>(callee);
		Unbind(f);
	}
	void Unbind(Signature method)
	{
		FunctionType f(method);
		Unbind(f);
	}
	void Unbind(FunctionType& f)
	{
		FunctionType* bindings = (FunctionType*)_ptr;
		for (intptr_t i = 0; i < _size; i++)
		{
			if (bindings[i]._callee == f._callee && bindings[i]._function == f._function)
			{
				bindings[i]._callee = 0;
				bindings[i]._function = 0;
				break;
			}
		}
	}
	void UnbindAll()
	{
		FunctionType* bindings = (FunctionType*)_ptr;
		for (intptr_t i = 0; i < _size; i++)
		{
			bindings[i]._function = 0;
			bindings[i]._callee = 0;
		}
	}
	int Count() const
	{
		int count = 0;
		FunctionType* bindings = (FunctionType*)_ptr;
		for (intptr_t i = 0; i < _size; i++)
		{
			if (bindings[i]._function != 0)
				count++;
		}
		return count;
	}
	bool IsBinded() const
	{
		FunctionType* bindings = (FunctionType*)_ptr;
		for (intptr_t i = 0; i < _size; i++)
		{
			if (bindings[i]._function != 0)
				return true;
		}
		return false;
	}
	int GetBindings(FunctionType* buffer, int bufferSize) const
	{
		int count = 0;
		FunctionType* bindings = (FunctionType*)_ptr;
		for (intptr_t i = 0; i < _size && i < bufferSize; i++)
		{
			buffer[count]._function = bindings[i]._function;
			if (buffer[count]._function != nullptr)
			{
				buffer[count]._callee = bindings[i]._callee;
				count++;
			}
		}
		return count;
	}
	void operator()(Params ... params) const
	{
		FunctionType* bindings = (FunctionType*)_ptr;
		for (intptr_t i = 0; i < _size; i++)
		{
			FunctionType f;
			f._function = bindings[i]._function;
			if (f._function != nullptr)
			{
				f._callee = bindings[i]._callee;
				f._function(f._callee, std::forward<Params>(params)...);
			}
		}
	}
};
DC_END_NAMESPACE
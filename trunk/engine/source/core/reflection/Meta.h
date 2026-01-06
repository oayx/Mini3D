#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <typeinfo>
#include <type_traits>
#include <tuple>
#include <utility>
#include <sstream>
#include "core/Compiler.h"

DC_BEGIN_NAMESPACE
//---------- 元数据系统 ----------
struct MetaData {
	std::unordered_map<std::string, std::string> data;

	bool HasFlag(const std::string& flag) const {
		return data.find(flag) != data.end();
	}

	std::string GetValue(const std::string& key) const {
		auto it = data.find(key);
		return it != data.end() ? it->second : "";
	}

	void Parse(const std::vector<std::string>& args) {
		for (const auto& arg : args) {
			size_t eqPos = arg.find('=');
			if (eqPos != std::string::npos) {
				std::string key = arg.substr(0, eqPos);
				std::string value = arg.substr(eqPos + 1);
				// 去除首尾空格
				key.erase(0, key.find_first_not_of(" "));
				key.erase(key.find_last_not_of(" ") + 1);
				value.erase(0, value.find_first_not_of(" "));
				value.erase(value.find_last_not_of(" ") + 1);
				data[key] = value;
			}
			else {
				std::string flag = arg;
				flag.erase(0, flag.find_first_not_of(" "));
				flag.erase(flag.find_last_not_of(" ") + 1);
				data[flag] = ""; // 标记存在
			}
		}
	}
};

//---------- 属性信息 ----------
struct MetaPropertyInfo {
	std::string name;
	std::string type;
	MetaData meta;

	MetaPropertyInfo(const std::string& n, const std::string& t,
		const std::vector<std::string>& metaArgs)
		: name(n), type(t) {
		meta.Parse(metaArgs);
	}
};

//---------- 函数信息 ----------
struct MetaFunctionInfo {
	std::string name;
	std::string returnType;
	std::vector<std::string> paramTypes;
	MetaData meta;
	std::function<void(void*, void**)> invoker;

	MetaFunctionInfo(const std::string& n, const std::string& retType,
		const std::vector<std::string>& params,
		const std::vector<std::string>& metaArgs)
		: name(n), returnType(retType), paramTypes(params) {
		meta.Parse(metaArgs);
	}
};

//---------- 类信息 ----------
struct MetaClass {
	MetaClass* super;
	std::string className;
	std::vector<MetaPropertyInfo> properties;
	std::vector<std::unique_ptr<MetaFunctionInfo>> functions;
	std::unordered_map<std::string, size_t> propertyMap;
	std::unordered_map<std::string, size_t> functionMap;

	MetaClass(const std::string& name, MetaClass* superClass)
		: super(superClass), className(name) {}

	void AddProperty(const std::string& name, const std::string& type, const std::vector<std::string>& meta) {
		properties.emplace_back(name, type, meta);
		propertyMap[name] = properties.size() - 1;
	}

	void AddFunction(const std::string& name, const std::string& returnType,
		const std::vector<std::string>& paramTypes,
		const std::vector<std::string>& meta,
		std::function<void(void*, void**)> invoker) {
		auto func = std::make_unique<MetaFunctionInfo>(name, returnType, paramTypes, meta);
		func->invoker = invoker;
		functionMap[name] = functions.size();
		functions.push_back(std::move(func));
	}

	MetaPropertyInfo* GetProperty(const std::string& name) {
		if (auto it = propertyMap.find(name); it != propertyMap.end()) {
			return &properties[it->second];
		}
		return super ? super->GetProperty(name) : nullptr;
	}

	MetaFunctionInfo* GetFunction(const std::string& name) {
		if (auto it = functionMap.find(name); it != functionMap.end()) {
			return functions[it->second].get();
		}
		return super ? super->GetFunction(name) : nullptr;
	}
};

//---------- 宏系统 ----------
#define GENERATED_BODY(ClassName, SuperClass) \
public: \
    using CurrentType = ClassName; \
    static MetaClass& StaticClass() { \
        static MetaClass classInfo(#ClassName, &SuperClass::StaticClass()); \
        return classInfo; \
    } \
    virtual MetaClass* GetClass() const override { \
        return &StaticClass(); \
    }

// UPROPERTY 宏实现
#define UPROPERTY(Type, Name, ...) \
    struct _PropRegistrar_##Name { \
        _PropRegistrar_##Name() { \
            std::vector<std::string> metaArgs = MetaUtils::ParseMetaArgs(#__VA_ARGS__); \
            CurrentType::StaticClass().AddProperty( \
                #Name, \
                #Type, \
                metaArgs \
            ); \
        } \
    }; \
    inline static _PropRegistrar_##Name _propReg_##Name;

// 带参数列表的 UFUNCTION 宏
#define UFUNCTION(ReturnType, Name, Params, ...) \
    struct _FuncRegistrar_##Name { \
        _FuncRegistrar_##Name() { \
            using ClassType = CurrentType; \
            using FuncType = decltype(&ClassType::Name); \
            \
            /* 处理参数类型 */ \
            std::vector<std::string> paramTypes = MetaUtils::ParseFunctionArgs(#Params); \
            /* 处理元数据 */ \
            std::vector<std::string> metaArgs = MetaUtils::ParseMetaArgs(#__VA_ARGS__); \
            \
            auto invoker = [](void* obj, void** params) { \
                ClassType* instance = static_cast<ClassType*>(obj); \
                static constexpr FuncType func = &ClassType::Name; \
                MetaFunctionInvoker<ClassType, FuncType>::Invoke( \
                    instance, params, func); \
            }; \
            \
            CurrentType::StaticClass().AddFunction( \
                #Name, \
                #ReturnType, \
                paramTypes, \
                metaArgs, \
                invoker \
            ); \
        } \
    }; \
    inline static _FuncRegistrar_##Name _funcReg_##Name;

// 函数注册帮助类
template<typename T, typename FuncPtr>
struct MetaFunctionInvoker;

// 无参数函数特化
template<typename T, typename Ret>
struct MetaFunctionInvoker<T, Ret(T::*)()> {
	static void Invoke(void* obj, void** params, Ret(T::* func)()) {
		T* instance = static_cast<T*>(obj);
		if constexpr (std::is_same_v<Ret, void>) {
			(instance->*func)();
		}
		else {
			Ret result = (instance->*func)();
			*static_cast<Ret*>(params[0]) = result;
		}
	}
};

// 带参数函数特化
template<typename T, typename Ret, typename... Args>
struct MetaFunctionInvoker<T, Ret(T::*)(Args...)> {
	template<size_t... Is>
	static void InvokeImpl(T* instance, void** params, Ret(T::* func)(Args...), std::index_sequence<Is...>) {
		if constexpr (std::is_same_v<Ret, void>) {
			(instance->*func)(
				*static_cast<Args*>(params[Is])...
				);
		}
		else {
			Ret result = (instance->*func)(
				*static_cast<Args*>(params[Is + 1])...
				);
			*static_cast<Ret*>(params[0]) = result;
		}
	}

	static void Invoke(void* obj, void** params, Ret(T::* func)(Args...)) {
		T* instance = static_cast<T*>(obj);
		constexpr size_t numArgs = sizeof...(Args);
		InvokeImpl(instance, params, func, std::make_index_sequence<numArgs>{});
	}
};

class MetaUtils
{
public:
	// 解析元数据参数
	static std::vector<std::string> ParseMetaArgs(const char* metaStr = "") {
		std::vector<std::string> metaArgs;
		if (metaStr != nullptr && strlen(metaStr) > 0) {
			std::string str = metaStr;
			size_t start = 0;
			size_t end = 0;

			while (end != std::string::npos) {
				end = str.find(',', start);
				std::string token = str.substr(start, (end == std::string::npos) ? str.size() - start : end - start);

				// 去除首尾空格
				token.erase(0, token.find_first_not_of(" "));
				token.erase(token.find_last_not_of(" ") + 1);

				if (!token.empty()) {
					metaArgs.push_back(token);
				}

				if (end != std::string::npos) {
					start = end + 1;
				}
			}
		}
		return metaArgs;
	}

	// 解析参数列表
	static std::vector<std::string> ParseFunctionArgs(const std::string& paramStr = "") {
		std::vector<std::string> params;
		std::istringstream iss(paramStr);
		std::string token;

		while (std::getline(iss, token, ',')) {
			// 移除首尾空格
			size_t start = token.find_first_not_of(" ");
			size_t end = token.find_last_not_of(" ");
			if (start != std::string::npos && end != std::string::npos) {
				token = token.substr(start, end - start + 1);
			}

			// 移除参数名，只保留类型
			size_t lastSpace = token.find_last_of(" ");
			if (lastSpace != std::string::npos) {
				token = token.substr(0, lastSpace);
			}

			// 再次移除首尾空格
			start = token.find_first_not_of(" ");
			end = token.find_last_not_of(" ");
			if (start != std::string::npos && end != std::string::npos) {
				token = token.substr(start, end - start + 1);
			}

			params.push_back(token);
		}

		return params;
	}
};

//---------- 反射对象基类 ----------
class ENGINE_DLL MetaObject {
public:
	static MetaClass& StaticClass() {
		static MetaClass info("MetaObject", nullptr);
		return info;
	}

	virtual MetaClass* GetClass()const = 0;

	template<typename Ret = void, typename... Args>
	Ret	Invoke(const std::string& name, Args... args) {
		auto funcInfo = GetClass()->GetFunction(name);
		constexpr size_t numArgs = sizeof...(Args);
		constexpr size_t totalParams = 1 + numArgs; // 返回值 + 参数

		void* paramPtrs[totalParams] = { nullptr };
		std::tuple<Args...> argTuple(args...);

		return InvokeHelper<!std::is_void<Ret>::value, Ret, Args...>::invoke(
			funcInfo, paramPtrs, argTuple, const_cast<MetaObject*>(this));
	}

private:
	// 辅助模板
	template <bool HasReturn, typename Ret, typename... Args>
	struct InvokeHelper;

	// 有返回值的特化版本
	template <typename Ret, typename... Args>
	struct InvokeHelper<true, Ret, Args...> {
		static Ret invoke(MetaFunctionInfo* funcInfo, void* paramPtrs[], std::tuple<Args...>& argTuple, MetaObject* obj) {
			Ret retValueStorage{};
			paramPtrs[0] = &retValueStorage;

			// 展开参数元组到指针数组
			size_t paramOffset = 1;
			unpack_tuple(paramPtrs, paramOffset, argTuple, std::index_sequence_for<Args...>{});

			funcInfo->invoker(obj, paramPtrs);
			return retValueStorage;
		}
	};

	// 无返回值的特化版本
	template <typename Ret, typename... Args>
	struct InvokeHelper<false, Ret, Args...> {
		static void invoke(MetaFunctionInfo* funcInfo, void* paramPtrs[], std::tuple<Args...>& argTuple, MetaObject* obj) {
			// 展开参数元组到指针数组
			size_t paramOffset = 0;
			unpack_tuple(paramPtrs, paramOffset, argTuple, std::index_sequence_for<Args...>{});

			funcInfo->invoker(obj, paramPtrs);
		}
	};

	// 元组展开辅助函数
	template <typename... Args, size_t... I>
	static void unpack_tuple(void* paramPtrs[], size_t& offset, std::tuple<Args...>& argTuple,
		std::index_sequence<I...>) {
		using expander = int[];
		(void)expander {
			0, (void(paramPtrs[offset + I] = &std::get<I>(argTuple)), 0)...
		};
	}
};
DC_END_NAMESPACE
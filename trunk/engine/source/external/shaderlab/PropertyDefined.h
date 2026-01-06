#pragma once

#include <string>
#include <variant>
#include "StringUtils.h"

namespace shaderlab {
	// 属性范围定义
	struct PropertyRange
	{
		float min;
		float max;
		PropertyRange(float m = 0.0f, float M = 1.0f) : min(m), max(M) {}
	};

	// 默认值存储（使用C++17 variant简化类型处理）
	struct PropertyDefaultValue
	{
		std::variant<
			float,					// Float/Range
			std::array<float, 4>,	// Vector/Color
			std::string,			// Texture路径
			int						// Int类型（如果需要）
		> value;

		// 辅助构造方法
		static PropertyDefaultValue Int(int v) { return { v }; }
		static PropertyDefaultValue Float(float v) { return { v }; }
		static PropertyDefaultValue Vector4(const std::array<float, 4>& arr) { return { arr }; }
		static PropertyDefaultValue Vector4(float x, float y, float z, float w) { return { std::array<float,4>{x, y, z, w} }; }
		static PropertyDefaultValue Texture(const std::string& path) { return { path }; }
	};

	enum class PropertyType
	{
		Int = 0,
		Float,
		Range,
		Vector,
		Color,
		Rect,//Non-Power-of-Two, NPOT
		Texture2D,
		Texture2DArray,
		Texture3D,
		TextureCube,
		TextureCubeArray,
		Any,
	};
}
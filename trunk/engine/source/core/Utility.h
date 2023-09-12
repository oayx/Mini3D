/*****************************************************************************************************/
// @author hannibal
// @date   2020/04/21
// @brief  工具类
/*****************************************************************************************************/
#pragma once

#include <random>
#include "Rect.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Utility Final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Utility);

public:
	/// <summary>
	/// 复制数据(必须是相同格式)
	/// </summary>
	/// <param name="src_data">数据源</param>
	/// <param name="src_offset">数据源开始拷贝位置</param>
	/// <param name="src_pitch">数据源一行字节数</param>
	/// <param name="dst_data">目标数据</param>
	/// <param name="dst_offset">目标开始拷贝位置</param>
	/// <param name="dst_pitch">目标一行字节数</param>
	/// <param name="size">拷贝区域大小</param>
	/// <param name="per_bytes">单个像素字节数</param>
	static void StretchMemoryCopy(const byte* src_data, const iPoint& src_offset, int src_pitch, byte* dst_data, const iPoint& dst_offset, int dst_pitch, const iSize& size, int per_bytes);

	static unsigned int RandomChar()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 255);
		return dis(gen);
	}
	static std::string GUID(int len = 16)
	{
		std::stringstream ss;
		for (auto i = 0; i < len; i++)
		{
			const auto rc = RandomChar();
			std::stringstream hexstream;
			hexstream << std::hex << rc;
			auto hex = hexstream.str();
			ss << (hex.length() < 2 ? '0' + hex : hex);
		}
		return ss.str();
	}
};
DC_END_NAMESPACE
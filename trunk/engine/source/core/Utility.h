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
class ENGINE_DLL Utility final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Utility);

public:
	/// <summary>
	/// 复制数据(必须是相同格式)
	/// </summary>
	/// <param name="srcData">数据源</param>
	/// <param name="srcOffset">数据源开始拷贝位置</param>
	/// <param name="srcPitch">数据源一行字节数</param>
	/// <param name="dstData">目标数据</param>
	/// <param name="dstOffset">目标开始拷贝位置</param>
	/// <param name="dstPitch">目标一行字节数</param>
	/// <param name="size">拷贝区域大小</param>
	/// <param name="perBytes">单个像素字节数</param>
	static void StretchMemoryCopy(const byte* srcData, const iPoint& srcOffset, int srcPitch, byte* dstData, const iPoint& dstOffset, int dstPitch, const iSize& size, int perBytes);

	static unsigned int RandomChar() noexcept
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 255);
		return dis(gen);
	}
	static std::string GUID(int len = 16) noexcept
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
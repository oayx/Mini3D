
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：二维码
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL QRCode Final
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(QRCode);
public:
	//获得二维码数据
	//version:二维码的复杂度和大小，有效值是[0,40]，不建议超过10
	//返回值需要在外部使用完毕后删除
	static unsigned char* Generate(const String& content, int version, int* ref_width, int* ref_data_bytes);
	//保存二维码到文件
	static bool SaveToPicture(const String& content, int version, const String& path);
};
DC_END_NAMESPACE

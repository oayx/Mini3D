
/*****************************************************************************************************/
// @author hannibal
// @date   2021/07/30
// @brief  接口导出
/*****************************************************************************************************/

#pragma once

#include "core/Compiler.h"
#include <astc/astc_codec_internals.h>
#include <ddsconv/TextureConv.h>
#include <etcpack/image.h>

extern "C" 
{
	DLL_EXPORT int dll_astc_main(int argc, char ** argv);
	DLL_EXPORT int dll_dxt_main(int argc, char** argv);
	DLL_EXPORT int dll_etc_main(int argc, char** argv);
}
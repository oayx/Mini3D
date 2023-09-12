
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#if defined(DC_GRAPHICS_API_DX9)
	#include "dx/dx9/DX9PostProcess.h"
#elif defined(DC_GRAPHICS_API_DX11)
	#include "dx/dx11/DX11PostProcess.h"
#elif defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
	#include "opengl/GLPostProcess.h"
#else
	#error "Unknown RendererAPI"
#endif

DC_BEGIN_NAMESPACE


DC_END_NAMESPACE
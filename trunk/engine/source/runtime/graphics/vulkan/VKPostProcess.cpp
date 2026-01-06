#include "VKPostProcess.h"
#include "VKProgram.h"
#include "runtime/graphics/Material.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKFinalProcess, FinalProcess);
VKFinalProcess::VKFinalProcess()
{
}
VKFinalProcess::~VKFinalProcess()
{
}
void VKFinalProcess::Render(Camera* camera, RenderTexture* dest)
{
}
DC_END_NAMESPACE
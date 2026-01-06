#include "ManualRender.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/graphics/null/GraphicsDevice.h"
#include "runtime/physics/Collider.h"
#include "runtime/resources/AssetsManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ManualRender, Renderer);
Object* ManualRender::Clone(Object* newObj)
{
	base::Clone(newObj);
	ManualRender* obj = dynamic_cast<ManualRender*>(newObj);
	if (!obj)return newObj;

	return obj;
}
bool ManualRender::CanRemove()
{
	return !GetGameObject()->GetComponent<MeshCollider>();
}
void ManualRender::OnDrawEditor()
{
	base::OnDrawEditor();
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(ManualRender);
template<class TransferFunction> inline
void ManualRender::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
}
DC_END_NAMESPACE
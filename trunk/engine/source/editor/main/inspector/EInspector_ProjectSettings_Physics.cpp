#include "EInspector_ProjectSettings_Physics.h"
#include "runtime/physics/Physics.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
void EInspector_ProjectSettings_Physics::Render()
{
	ImGuiEx::Label("Gravity");
	Vector3 gravity = Physics::GetGravity();
	if (ImGui::DragFloat3("##Gravity", gravity.ptr(), 0.01f, -10000.0f, 10000.0f))
	{
		Physics::SetGravity(gravity);
		Physics::Save();
	}

	ImGuiEx::Label("Default Friction");
	float friction = Physics::GetDefaultFriction();
	if (ImGui::DragFloat("##DefaultFriction", &friction, 0.01f, 0.0f, 1.0f))
	{
		Physics::SetDefaultFriction(friction);
		Physics::Save();
	}
	ImGuiEx::Label("Default Restitution");
	float restitution = Physics::GetDefaultRestitution();
	if (ImGui::DragFloat("##DefaultRestitution", &restitution, 0.01f, 0.0f, 1.0f))
	{
		Physics::SetDefaultRestitution(restitution);
		Physics::Save();
	}
	
	ImGuiEx::Label("Solver Iterations");
	int steps = Physics::GetSubSteps();
	const ImS32 s32_one = 1;
	if (ImGui::InputScalar("##SolverIterations", ImGuiDataType_S32, &steps, &s32_one))
	{
		Physics::SetSubSteps(steps);
		Physics::Save();
	}

	ImGuiEx::Label("Auto Simulate");
	bool auto_simulate = Physics::IsAutoSimulate();
	if (ImGui::Checkbox("##Auto Simulate", &auto_simulate))
	{
		Physics::SetAutoSimulate(auto_simulate);
		Physics::Save();
	}
	ImGuiEx::Label("Auto Sync Transform");
	bool sync_transform = Physics::IsAutoSyncTransform();
	if (ImGui::Checkbox("##Auto Sync Transform", &sync_transform))
	{
		Physics::SetAutoSyncTransform(sync_transform);
		Physics::Save();
	}

	ImGui::TextUnformatted("World Bounds");
	ImGuiEx::Label("    Center");
	Vector3 center = Physics::GetWorldCenter();
	if (ImGui::DragFloat3("##Center", center.ptr(), 1.0f, MIN_float, MAX_float))
	{
		Physics::SetWorldCenter(center);
		Physics::Save();
	}
	ImGuiEx::Label("    Extent");
	Vector3 extent = Physics::GetWorldExtent();
	if (ImGui::DragFloat3("##Extent", extent.ptr(), 1.0f, MIN_float, MAX_float))
	{
		Physics::SetWorldExtent(extent);
		Physics::Save();
	}
}
DC_END_NAMESPACE
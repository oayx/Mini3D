
/*****************************************************************************
* Author： hannibal
* Date：2020/9/23
* Description：spine动画
*****************************************************************************/
#pragma once

#include "runtime/renderable/Renderer.h"
#include <spine/spine.h>

DC_BEGIN_NAMESPACE
class Camera;
/********************************************************************/
class ENGINE_DLL Skeleton2d final : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(Skeleton2d);
	DISALLOW_COPY_ASSIGN(Skeleton2d);
	BEGIN_DERIVED_REFECTION_TYPE(Skeleton2d, Renderer)
		CTORS(DEFAULT_CTOR(Skeleton2d))
		ADD_EDITOR_COMPONENT("Spine")
	END_REFECTION_TYPE;

protected:
	Skeleton2d();
	~Skeleton2d();

	virtual void Awake()override;
	virtual void Update()override;
	virtual void OnDrawGizmos(Camera* camera)override;

public:
	bool LoadFromFile(const String &mesh_file) = delete;
	bool LoadFromFile(const String& json_file, const String& atlas_file);
	bool LoadFromBinary(const String& json_file, const String& atlas_file);
	void SetTimeScale(float scale) { _timeScale = scale; }

	bool SetAnimation(int trackIndex, const String& name, bool loop);
	bool AddAnimation(int trackIndex, const String& name, bool loop, float delay = 0);
	bool SetEmptyAnimation(int trackIndex, float mixDuration);
	bool SetEmptyAnimations(float mixDuration);
	VecString GetAnimations()const;
	bool SetMix(const String& fromAnimation, const String& toAnimation, float duration);
	void SetDefaultMix(float duration);
	void ClearTrack(int trackIndex);
	void ClearTracks();

	bool SetSkin(const String& name);

private:
	void BuildMesh();
	void CalBoundingBox();
	int  ComputeTotalVertexCount(int startSlotIndex, int endSlotIndex);
	void TransformWorldVertices(Vector<float>& vertex, int startSlotIndex, int endSlotIndex);
	void CreateMaterial();

private:
	spine::Skeleton* _skeleton = nullptr;
	spine::AnimationStateData* _animationStateData = nullptr;
	spine::AnimationState* _animationState = nullptr;
	spine::SkeletonClipping* _clipper = nullptr;

	float _timeScale = 1.0f;
};
DC_END_NAMESPACE
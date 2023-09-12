#include "Skeleton2d.h"
#include "SkeletonExtension2d.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/renderable/Gizmos.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
static SpineTextureLoader texture_loader;
static bool slotIsOutRange(spine::Slot& slot, int startSlotIndex, int endSlotIndex)
{
	const int index = slot.getData().getIndex();
	return startSlotIndex > index || endSlotIndex < index;
}
static bool nothingToDraw(spine::Slot& slot, int startSlotIndex, int endSlotIndex)
{
	spine::Attachment *attachment = slot.getAttachment();
	if (!attachment || slotIsOutRange(slot, startSlotIndex, endSlotIndex) || !slot.getBone().isActive() || slot.getColor().a == 0)
		return true;
	if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
	{
		if (static_cast<spine::RegionAttachment*>(attachment)->getColor().a == 0)
			return true;
	}
	else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
	{
		if (static_cast<spine::MeshAttachment*>(attachment)->getColor().a == 0)
			return true;
	}
	return false;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Skeleton2d, Renderer);
Skeleton2d::Skeleton2d()
{
}
Skeleton2d::~Skeleton2d()
{
	delete(_skeleton);
	delete(_animationStateData);
	delete(_animationState);
	delete(_clipper);
}
void Skeleton2d::Awake()
{
	base::Awake();
	this->CreateMaterial();
	_clipper = new spine::SkeletonClipping();
}
void Skeleton2d::Update()
{
	base::Update();
	if (_skeleton == nullptr)
	{
		this->ClearData();
		return;
	}
	float deltaTime = Time::GetDeltaTime() * _timeScale;
	_skeleton->update(deltaTime);
	_animationState->update(deltaTime);
	_animationState->apply(*_skeleton);
	_skeleton->updateWorldTransform();

	this->BuildMesh();
}
void Skeleton2d::OnDrawGizmos(Camera* camera)
{
	Gizmos::SetColor(Color::Red);
	Gizmos::SetMatrix(this->GetTransform()->GetLocalToWorldMatrix());
	const Aabb& aabb = this->GetTransform()->GetLocalBoundingBox();
	Gizmos::DrawRect(Vector3::zero, Vector2(aabb.GetSize().x, aabb.GetSize().y));
}
bool Skeleton2d::LoadFromFile(const String& json_file, const String& atlas_file)
{
	delete(_skeleton);
	delete(_animationStateData);
	delete(_animationState);

	String atlas_res_file = Resource::GetFullDataPath(atlas_file);
	spine::Atlas* atlas = new spine::Atlas(atlas_res_file.c_str(), &texture_loader);

	spine::SkeletonJson json(atlas);
	spine::SkeletonData* skeleton_data = json.readSkeletonDataFile(Resource::GetFullDataPath(json_file).c_str());

	_skeleton = new spine::Skeleton(skeleton_data);

	_animationStateData = new spine::AnimationStateData(skeleton_data);
	_animationStateData->setDefaultMix(0.1f);
	
	_animationState = new spine::AnimationState(_animationStateData);
	return true;
}
bool Skeleton2d::LoadFromBinary(const String& json_file, const String& atlas_file)
{
	delete(_skeleton);
	delete(_animationStateData);
	delete(_animationState);

	String atlas_res_file = Resource::GetFullDataPath(atlas_file);
	spine::Atlas* atlas = new spine::Atlas(atlas_res_file.c_str(), &texture_loader);

	spine::SkeletonBinary binary(atlas);
	spine::SkeletonData* skeleton_data = binary.readSkeletonDataFile(Resource::GetFullDataPath(json_file).c_str());

	_skeleton = new spine::Skeleton(skeleton_data);

	_animationStateData = new spine::AnimationStateData(skeleton_data);
	_animationStateData->setDefaultMix(0.1f);

	_animationState = new spine::AnimationState(_animationStateData);
	return true;
}
bool Skeleton2d::SetAnimation(int trackIndex, const String& name, bool loop)
{
	if (_skeleton == nullptr || _animationState == nullptr)
		return false;
	spine::Animation* animation = _skeleton->getData()->findAnimation(name.c_str());
	if (animation == nullptr) 
		return false;
	spine::TrackEntry* track = _animationState->setAnimation(trackIndex, animation, loop);
	return track != nullptr;
}
bool Skeleton2d::AddAnimation(int trackIndex, const String& name, bool loop, float delay)
{
	if (_skeleton == nullptr || _animationState == nullptr)
		return false;
	spine::Animation* animation = _skeleton->getData()->findAnimation(name.c_str());
	if (animation == nullptr)
		return false;
	spine::TrackEntry* track = _animationState->addAnimation(trackIndex, animation, loop, delay);
	return track != nullptr;
}
bool Skeleton2d::SetEmptyAnimation(int trackIndex, float mixDuration)
{
	if (_animationState == nullptr)
		return false;
	spine::TrackEntry* track = _animationState->setEmptyAnimation(trackIndex, mixDuration);
	return track != nullptr;
}
bool Skeleton2d::SetEmptyAnimations(float mixDuration)
{
	if (_animationState == nullptr)
		return false;
	_animationState->setEmptyAnimations(mixDuration);
	return true;
}
VecString Skeleton2d::GetAnimations()const
{
	VecString vec;
	spine::Vector<spine::Animation*> anims = _skeleton->getData()->getAnimations();
	for (int i = 0; i < (int)anims.size(); ++i)
	{
		vec.Add(anims[i]->getName().buffer());
	}
	return vec;
}
bool Skeleton2d::SetMix(const String& fromAnimation, const String& toAnimation, float duration)
{
	if (_animationState == nullptr)
		return false;
	_animationState->getData()->setMix(fromAnimation.c_str(), toAnimation.c_str(), duration);
	return true;
}
void Skeleton2d::SetDefaultMix(float duration)
{
	if (_animationState != nullptr)
		_animationState->getData()->setDefaultMix(duration);
}
void Skeleton2d::ClearTrack(int trackIndex)
{
	if (_animationState != nullptr)
		_animationState->clearTrack(trackIndex);
}
void Skeleton2d::ClearTracks()
{
	if (_animationState != nullptr)
		_animationState->clearTracks();
}
bool Skeleton2d::SetSkin(const String& name)
{
	if (_skeleton == nullptr)
		return false;
	_skeleton->setSkin(name.c_str());
	return true;
}
void Skeleton2d::BuildMesh()
{
	this->ClearData();
	Material* material = GetMaterial(0);

	int vertex_count = ComputeTotalVertexCount(0, std::numeric_limits<int>::max());
	int vertex_offset = 0;
	Vector<float> vertex(vertex_count);
	TransformWorldVertices(vertex, 0, std::numeric_limits<int>::max());

	for (int i = 0; i < (int)_skeleton->getSlots().size(); ++i)
	{
		spine::Slot* slot = _skeleton->getSlots()[i];
		if (nothingToDraw(*slot, 0, std::numeric_limits<int>::max()))
		{
			_clipper->clipEnd(*slot);
			continue;
		}

		if (slot->getAttachment()->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			int prim_index = GetPrimitiveCount();
			Primitive* primitive = GetPrimitive(prim_index);

			spine::RegionAttachment* attachment = static_cast<spine::RegionAttachment*>(slot->getAttachment());
			spine::AtlasRegion* region = (spine::AtlasRegion*)attachment->getRendererObject();
			Texture* texture = (Texture*)region->page->getRendererObject();
			material->SetTexture(texture);

			Color color(attachment->getColor().r, attachment->getColor().g, attachment->getColor().b, attachment->getColor().a);

			spine::Vector<float>& uvs = attachment->getUVs();
			for (int j = 0; j < 4; ++j)
			{
				AddVertex(Vector3(vertex[vertex_offset + j * 2 + 0], vertex[vertex_offset + j * 2 + 1], 0), prim_index);
				AddColor(color, prim_index);
				AddTexcoord1(Vector2(uvs[j * 2 + 0], uvs[j * 2 + 1]), prim_index);
			}

			SetIndices({ 0,1,2,0,2,3 }, prim_index);
			vertex_offset += 8;
		}
		else if (slot->getAttachment()->getRTTI().isExactly(spine::MeshAttachment::rtti))
		{
			int prim_index = GetPrimitiveCount();
			Primitive* primitive = GetPrimitive(prim_index);

			spine::MeshAttachment* attachment = (spine::MeshAttachment*)slot->getAttachment();
			spine::AtlasRegion* region = (spine::AtlasRegion*)attachment->getRendererObject();
			Texture* texture = (Texture*)region->page->getRendererObject();
			material->SetTexture(texture);

			Color color(attachment->getColor().r, attachment->getColor().g, attachment->getColor().b, attachment->getColor().a);

			spine::Vector<float>& uvs = attachment->getUVs();
			for (uint j = 0; j < attachment->getWorldVerticesLength() >> 1; j += 1)
			{
				AddVertex(Vector3(vertex[vertex_offset + j * 2 + 0], vertex[vertex_offset + j * 2 + 1], 0), prim_index);
				AddColor(color, prim_index);
				AddTexcoord1(Vector2(uvs[j * 2 + 0], uvs[j * 2 + 1]), prim_index);
			}

			for (uint j = 0; j < attachment->getTriangles().size(); ++j)
			{
				AddIndex(attachment->getTriangles()[j], prim_index);
			}
			vertex_offset += (int)attachment->getWorldVerticesLength();
		}
		else if (slot->getAttachment()->getRTTI().isExactly(spine::ClippingAttachment::rtti))
		{
			spine::ClippingAttachment* clip = (spine::ClippingAttachment*)slot->getAttachment();
			_clipper->clipStart(*slot, clip);
			continue;
		}
		else
		{
			_clipper->clipEnd(*slot);
			continue;
		}
	}

	this->UploadData();
}
void Skeleton2d::CalBoundingBox()
{
	for (int i = 0; i < (int)_skeleton->getSlots().size(); ++i) 
	{
		spine::Slot* slot = _skeleton->getSlots()[i];
		if (!slot->getAttachment()) continue;
		if (slot->getAttachment()->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			spine::RegionAttachment* attachment = static_cast<spine::RegionAttachment*>(slot->getAttachment());
		}
	}
}
int Skeleton2d::ComputeTotalVertexCount(int startSlotIndex, int endSlotIndex)
{
	if (_skeleton == nullptr)
		return 0;
	int coordCount = 0;
	for (size_t i = 0; i < _skeleton->getSlots().size(); ++i)
	{
		spine::Slot& slot = *_skeleton->getSlots()[i];
		if (nothingToDraw(slot, startSlotIndex, endSlotIndex)) 
		{
			continue;
		}
		spine::Attachment* const attachment = slot.getAttachment();
		if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			coordCount += 8;
		}
		else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
		{
			spine::MeshAttachment* const mesh = static_cast<spine::MeshAttachment*>(attachment);
			coordCount += (int)mesh->getWorldVerticesLength();
		}
	}
	return coordCount;
}
void Skeleton2d::TransformWorldVertices(Vector<float>& vertex, int startSlotIndex, int endSlotIndex)
{
	float* dst_ptr = &vertex[0];
	for (size_t i = 0; i < _skeleton->getSlots().size(); ++i)
	{
		spine::Slot& slot = *_skeleton->getDrawOrder()[i];
		if (nothingToDraw(slot, startSlotIndex, endSlotIndex))
		{
			continue;
		}
		spine::Attachment* const attachment = slot.getAttachment();
		if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti)) 
		{
			spine::RegionAttachment* const regionAttachment = static_cast<spine::RegionAttachment*>(attachment);
			regionAttachment->computeWorldVertices(slot.getBone(), dst_ptr, 0, 2);
			dst_ptr += 8;
		}
		else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti)) 
		{
			spine::MeshAttachment* const mesh = static_cast<spine::MeshAttachment*>(attachment);
			mesh->computeWorldVertices(slot, 0, mesh->getWorldVerticesLength(), dst_ptr, 0, 2);
			dst_ptr += mesh->getWorldVerticesLength();
		}
	}
}
void Skeleton2d::CreateMaterial()
{
	Material* mat = this->SetMaterial("internal/material/Spine.material");
	mat->SetTexture(Texture::GetWhiteTexture());
}
DC_END_NAMESPACE

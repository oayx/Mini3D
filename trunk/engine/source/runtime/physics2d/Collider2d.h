 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/17
* Description：
*****************************************************************************/
#pragma once

#include "Physics2d_def.h"
class b2Body;

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Collider2d : public Component
{
	typedef List<uint64> Contactes;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	FRIEND_CONSTRUCT_DESTRUCT(Collider2d);
	DISALLOW_COPY_ASSIGN(Collider2d);
	BEGIN_DERIVED_REFECTION_TYPE(Collider2d, Component)
	END_REFECTION_TYPE;

protected:
	Collider2d();
	~Collider2d();
	virtual void Start()override;

public://需要序列化属性
	void	SetIsTrigger(bool b) { _isTrigger = b; }
	bool	IsTrigger()const { return _isTrigger; }
	void	SetFriction(float friction) { _friction = Math::Clamp01(friction); }
	void	SetRestitution(float restitution) { _restitution = Math::Clamp01(restitution); }
	void	SetOffset(const Vector2& offset) { _offset = offset; }

	void	SetCategoryBits(uint bits) { _categoryBits = bits; }
	void	SetMaskBits(uint bits) { _maskBits = bits; }
	void	SetGroupIndex(int index) { _groupIndex = index; }

public:
	b2Body* GetBody()const { return _b2Body; }

	void	ModifyPosition(const Vector2& pos);

	void	AddContact(uint64 id) { if(!HasContact(id)) _contactes.Add(id); }
	void	RemoveContact(uint64 id) { _contactes.Remove(id); }
	bool	HasContact(uint64 id) { return std::find(_contactes.begin(), _contactes.end(), id) != _contactes.end(); }

protected:
	b2Body* _b2Body = nullptr;
	Contactes _contactes;//当前接触对象列表

	bool	_isTrigger = false;
	float	_friction = 0;
	float	_restitution = 0;
	Vector2 _offset = Vector2::zero;

	/**
	 * Box2D 支持 16 个种群，对于任何一个形状你都可以指定它属于哪个种群。你还可以指定这个形状可
	 * 以和其它哪些种群发生碰撞。例如，你可以在一个多人游戏中指定玩家之间不会碰撞，怪物之间也不会
	 * 碰撞，但是玩家和怪物会发生碰撞。这是通过掩码来完成的，例如：
	 * playerShapeDef.filter.categoryBits = 0x0002;
	 * monsterShapeDef.filter.categoryBits = 0x0004;
	 * playerShape.filter.maskBits = 0x0004;
	 * monsterShapeDef.filter.maskBits = 0x0002;
	 */
	uint _categoryBits = 0x0001;

	/**
	 * The collision mask bits. This states the categories that this
	 * shape would accept for collision.
	 */
	uint _maskBits = 0xFFFF;

	/**
	 * 碰撞组可以让你指定一个整数的组索引。你可以让同一个组的所有形状总是相互碰撞(正索引)或永远
	 * 不碰撞(负索引)。组索引通常用于一些以某种方式关联的事物，就像自行车的那些部件。在下面的例子
	 * 中，shape1 和 shape2 总是碰撞，而 shape3 和 shape4 永远不会碰撞。
	 * shape1Def.filter.groupIndex = 2;
	 * shape2Def.filter.groupIndex = 2;
	 * shape3Def.filter.groupIndex = -8;//相同的负数不会发生碰撞
	 * shape4Def.filter.groupIndex = -8;
	 * 不同组索引之间形状的碰撞会按照种群和掩码来筛选。换句话说，组筛选比种群筛选有更高的优选权。
	 */
	 /*
	bool b2ContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
	{
		const b2Filter& filterA = fixtureA->GetFilterData();
		const b2Filter& filterB = fixtureB->GetFilterData();

		if (filterA.groupIndex == filterB.groupIndex && filterA.groupIndex != 0)
		{
			return filterA.groupIndex > 0;
		}

		bool collide = (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0;
		return collide;
	}
	*/
	int _groupIndex = 0;
};

class ENGINE_DLL BoxCollider2d final : public Collider2d
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	FRIEND_CONSTRUCT_DESTRUCT(BoxCollider2d);
	DISALLOW_COPY_ASSIGN(BoxCollider2d);
	BEGIN_DERIVED_REFECTION_TYPE(BoxCollider2d, Collider2d)
		//CTORS(DEFAULT_CTOR(BoxCollider2d))
		//ADD_EDITOR_COMPONENT("Physics2D/BoxCollider")
	END_REFECTION_TYPE;

	using base::base;

protected:
	virtual void Awake()override {}
	virtual void Start()override;

public:
	void SetScale(float w, float h);

private:
	void UpdateSize(b2PolygonShape* shape);

private:
	Size _localSize = Size(1.0f, 1.0f);
};

class ENGINE_DLL CircleCollider2d final : public Collider2d
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	FRIEND_CONSTRUCT_DESTRUCT(CircleCollider2d);
	DISALLOW_COPY_ASSIGN(CircleCollider2d);
	BEGIN_DERIVED_REFECTION_TYPE(CircleCollider2d, Collider2d)
		//CTORS(DEFAULT_CTOR(CircleCollider2d))
		//ADD_EDITOR_COMPONENT("Physics2D/CircleCollider")
	END_REFECTION_TYPE;

	using base::base;

protected:
	virtual void Start()override;

public:
	void SetScale(float r);

private:
	void UpdateSize(b2CircleShape* shape);

private:
	float _radius = 1.0f;
};

class ENGINE_DLL PolygonCollider2d final : public Collider2d
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	FRIEND_CONSTRUCT_DESTRUCT(PolygonCollider2d);
	DISALLOW_COPY_ASSIGN(PolygonCollider2d);
	BEGIN_DERIVED_REFECTION_TYPE(PolygonCollider2d, Collider2d)
		//CTORS(DEFAULT_CTOR(PolygonCollider2d))
		//ADD_EDITOR_COMPONENT("Physics2D/PolygonCollider")
	END_REFECTION_TYPE;

	using base::base;

protected:
	virtual void Start()override;

public:
	void Set(const Vector2v& vertexs);

private:
	Vector2v _vertexs;
};
DC_END_NAMESPACE

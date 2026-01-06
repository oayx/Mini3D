 
/*****************************************************************************
* Author： hannibal
* Date：2021/1/5
* Description：粒子系统
*****************************************************************************/
#pragma once

#include "core/Random.h"
#include "runtime/component/GameObject.h"
#include "runtime/CommonDefine.h"

DC_BEGIN_NAMESPACE
class Particle;
class ParticleEmission;
class ParticleRender;
class ParticleShape;

//特效缩放模式
enum class PScalingMode
{
	Hierarchy = 0,	//受自身的缩放影响，也受父级的缩放影响
	Local,			//只受自身的缩放影响
	Shape,			//不受缩放影响
};
DECLARE_ENUM_OPERATORS(PScalingMode);
//形状类型
enum class PShapeType
{
	Sphere = 0,		//球	
	SphereShell,	//球壳
	HemiSphere,		//半球
	HemiSphereShell,//半球壳
	Cone,			//锥
	Box,			//盒子
	BoxEdge,
	Circle,			//圆
	CircleEdge,		//圆的边缘
};
DECLARE_ENUM_OPERATORS(PShapeType);
//粒子系统停止后的操作
enum class PStopAction
{
	None = 0,		//不处理	
	Disable,		//隐藏
	Destroy,		//销毁
	Callback,		//回调
};
DECLARE_ENUM_OPERATORS(PStopAction);

/********************************************************************/
class ENGINE_DLL ParticleSystem final : public Component
{
	friend class GameObject;
	friend class ParticleRender;
	typedef List<Particle*> Particles;
	FRIEND_CONSTRUCT_DESTRUCT(ParticleSystem);
	DISALLOW_COPY_ASSIGN(ParticleSystem);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(ParticleSystem);
	BEGIN_DERIVED_REFECTION_TYPE(ParticleSystem, Component)
		CTORS(DEFAULT_CTOR(ParticleSystem))
		ADD_EDITOR_COMPONENT("Effects/ParticleSystem")
	END_REFECTION_TYPE;

public:
	//主信息
	struct MainModule
	{
		float		Duration = 5.0f;				//持续时间：粒子系统持续时间，如果不是循环，则超过这个时间后粒子系统关闭
		bool		Loop = true;					//循环播放：如果为true，则在持续时间之后重复发射周期
		float		StartDelay = 0.0f;				//启动延迟：在发射粒子之前等待的延迟时间

		float		StartLifeTime = 5.0f;			//生命周期：以秒为单位开始生命周期，粒子将在其生命周期达到0时消亡。即存活时间。
		float		StartSpeed = 1.0f;				//起始速度：在起始方向施加的粒子的起始速度
		Vector3		StartSize = Vector3::one;		//起始大小：粒子的起始大小
		Vector3		StartRotation = Vector3::zero;	//起始旋转：粒子的起始旋转度数
		Color		StartColor = Color::White;		//起始颜色：粒子的起始颜色

		float		GravityModifier = 0.0f;			//重力修改器：修改重力值会影响粒子发射时所受重力影响的状态
		float		SimulationSpeed = 1.0f;			//模拟速度：缩放粒子系统的播放速度
		bool		UseUnscaledTime = false;		//是否受DeltaTime影响
		bool		PlayOnAwake = true;				//唤醒时播放
		int			MaxParticles = 1000;			//最大粒子：系统中粒子数量受此数量的限制，达到此数量后，将暂时停止发射
		int			RandomSeed = 0;					//自动随机种子：如果为0表示自动随机，否则固定

		SimulationSpace	 Space = SimulationSpace::Local;	//模拟空间
		PScalingMode	 ScalingMode = PScalingMode::Local;	//缩放模式
		PStopAction		 StopAction = PStopAction::None;
		Action			 StopCallback = nullptr;			//Main.StopCallback = [] {Debuger::Log("ParticleSystem stop"); };

		template<class TransferFunction> 
		inline void Transfer(TransferFunction& transfer, void* ptr)
		{
			TRANSFER_SIMPLE(Duration);
			TRANSFER_SIMPLE(Loop);
			TRANSFER_SIMPLE(StartDelay);
			TRANSFER_SIMPLE(StartLifeTime);
			TRANSFER_SIMPLE(StartSpeed);
			TRANSFER_SIMPLE(StartSize);
			TRANSFER_SIMPLE(StartRotation);
			TRANSFER_SIMPLE(StartColor);
			TRANSFER_SIMPLE(GravityModifier);
			TRANSFER_SIMPLE(SimulationSpeed);
			TRANSFER_SIMPLE(UseUnscaledTime);
			TRANSFER_SIMPLE(PlayOnAwake);
			TRANSFER_SIMPLE(MaxParticles);
			TRANSFER_SIMPLE(RandomSeed);
			TRANSFER_ENUM(Space);
			TRANSFER_ENUM(ScalingMode);
			TRANSFER_ENUM(StopAction);
		}
	};
	//发射器
	struct EmissionModule
	{
		bool		Enabled = true;					//是否激活
		int			RateOverTime = 10;				//发射速度

		template<class TransferFunction> 
		inline void Transfer(TransferFunction& transfer, void* ptr)
		{
			TRANSFER_SIMPLE(Enabled);
			TRANSFER_SIMPLE(RateOverTime);
		}
	};
	//发射器形状
	struct ShapeModule
	{
		bool		Enabled = true;					//是否激活
		PShapeType	Type = PShapeType::Cone;
		float		Radius = 1.0f;					//[0,+无穷)半径：适用于球体、半球、锥体、圆形
		float		Degree = 30.0f;					//[0,180)角度：适用于锥体
		Vector3		Position = Vector3::zero;		//发射器相对偏移
		Vector3		Rotation = Vector3::zero;
		Vector3		Scale = Vector3::one;

		template<class TransferFunction>
		inline void Transfer(TransferFunction& transfer, void* ptr)
		{
			TRANSFER_SIMPLE(Enabled);
			TRANSFER_ENUM(Type);
			TRANSFER_SIMPLE(Radius);
			TRANSFER_SIMPLE(Degree);
			TRANSFER_SIMPLE(Position);
			TRANSFER_SIMPLE(Rotation);
			TRANSFER_SIMPLE(Scale);
		}
	};
	//生命周期内速度
	struct VelocityOverLifetimeModule
	{
		bool		Enabled = false;				//是否激活
		Vector3		Velocity = Vector3::zero;
		float		SpeedModifier = 1;				//[0,+无穷)速度修改器：将粒子速度乘以该值

		template<class TransferFunction>
		inline void Transfer(TransferFunction& transfer, void* ptr)
		{
			TRANSFER_SIMPLE(Enabled);
			TRANSFER_SIMPLE(Velocity);
			TRANSFER_SIMPLE(SpeedModifier);
		}
	};
	//生命周期内速度限制
	struct LimitVelocityOverLifetimeModule
	{
		bool		Enabled = false;				//是否激活
		float		Speed = 1;						//[0,+无穷)粒子在粒子生存期内的速度限制
		float		Dampen = 0.5f;					//[0,1]抑制，控制超过速度限制的速度应该受到的抑制，值为0.5将使超出速度减小50%，值为1使超出速度变为0

		template<class TransferFunction>
		inline void Transfer(TransferFunction& transfer, void* ptr)
		{
			TRANSFER_SIMPLE(Enabled);
			TRANSFER_SIMPLE(Speed);
			TRANSFER_SIMPLE(Dampen);
		}
	};
	//生命周期内受力
	struct ForceOverLifetimeModule
	{
		bool		Enabled = false;				//是否激活
		Vector3		Force = Vector3::zero;			//受力

		template<class TransferFunction>
		inline void Transfer(TransferFunction& transfer, void* ptr)
		{
			TRANSFER_SIMPLE(Enabled);
			TRANSFER_SIMPLE(Force);
		}
	};
	//生命周期内颜色改变
	struct ColorOverLifetimeModule
	{
		bool		Enabled = false;				//是否激活
		Color		TargetColor = Color::White;		//最终颜色

		template<class TransferFunction>
		inline void Transfer(TransferFunction& transfer, void* ptr)
		{
			TRANSFER_SIMPLE(Enabled);
			TRANSFER_SIMPLE(TargetColor);
		}
	};
	//生命周期内大小改变
	struct SizeOverLifetimeModule
	{
		bool		Enabled = false;				//是否激活
		Vector3		TargetSize = Vector3::one;		//最终大小

		template<class TransferFunction>
		inline void Transfer(TransferFunction& transfer, void* ptr)
		{
			TRANSFER_SIMPLE(Enabled);
			TRANSFER_SIMPLE(TargetSize);
		}
	};
	//序列帧纹理
	struct TextureSheetAnimationModule
	{
		bool		Enabled = false;				//是否激活
		uint		Rows = 1;						//数量(x:cols,y:rows)
		uint		Cols = 1;
		uint		StartFrame = 0;
		float		Cycles = 1.0f;					//动画序列在粒子生命周期内重复次数
		template<class TransferFunction>
		inline void Transfer(TransferFunction& transfer, void* ptr)
		{
			TRANSFER_SIMPLE(Enabled);
			TRANSFER_SIMPLE(Rows);
			TRANSFER_SIMPLE(Cols);
			TRANSFER_SIMPLE(StartFrame);
			TRANSFER_SIMPLE(Cycles);
			if (Rows < 1)Rows = 1;
			if (Cols < 1)Cols = 1;
			if (Cycles < 1.0f)Cycles = 1.0f;
		}
	};

protected:
	ParticleSystem();
	~ParticleSystem();

	virtual void Awake()override;
	virtual void Start()override;
	virtual void Update()override;
	virtual void OnDestroy()override;
	virtual void OnTransformChanged()override;
	virtual void OnDrawEditor()override;
	virtual uint64 GetEditorAction()const override { return DEFAULT_COMPONENT_ACTION | uint64(ComponentAction::Update); }

public:
	void Play();
	void Pause();
	void Stop();
	void Simulate(float dt);
	void Emit(int count);

	bool IsPlaying()const { return _isPlaying; }

	int   RandomRange(int min, int max);
	float RandomRange(float min, float max);

	ParticleShape*		GetShape()const { return _particleShape; }
	ParticleEmission*	GetEmission()const { return _particleEmission; }
	ParticleRender*		GetRender()const { return _particleRender; }

public:
	MainModule			Main;
	EmissionModule		Emission;
	ShapeModule			Shape;

	VelocityOverLifetimeModule		VelocityOverLifetime;
	LimitVelocityOverLifetimeModule LimitVelocityOverLifetime;
	ForceOverLifetimeModule			ForceOverLifetime;
	ColorOverLifetimeModule			ColorOverLifetime;
	SizeOverLifetimeModule			SizeOverLifetime;
	TextureSheetAnimationModule		TextureSheetAnimation;

private:
	ParticleShape*		_particleShape = nullptr;
	ParticleEmission*	_particleEmission = nullptr;
	ParticleRender*		_particleRender = nullptr;
	Random				_random;				//粒子随机数生成器

	Particles			_particles;				//激活的粒子

	bool				_isActive = false;		//是否激活
	bool				_isPlaying = false;
	float				_lifeTime = 0.0f;		//剩余时间，针对非循环情况

	Vector3				_oldPosition;			//记录特效之前的位置，用于处理跟随移动
};
DC_END_NAMESPACE

#include "ParticleEmission.h"
#include "ParticleSystem.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ParticleEmission, Object);
void ParticleEmission::Simulate(float dt)
{
	if (!_particleSystem->Emission.Enabled)
		return;

	//计算需要发射的粒子数量
	int count = 0;
	int roundTotalCount = _particleSystem->Emission.RateOverTime;
	_roundTime += dt;
	if (_roundTime >= 1)
	{//新的回合
		if (_roundCreateCount < _particleSystem->Emission.RateOverTime)
		{//如果还有未创建的，一次创建完成
			count = _particleSystem->Emission.RateOverTime - _roundCreateCount;
		}
		_roundTime = 0;
		_roundCreateCount = 0;
	}
	else if(_roundCreateCount < roundTotalCount)
	{
		float expectRatio = _roundTime;
		float realRatio = float(_roundCreateCount) / float(roundTotalCount);
		if (realRatio < expectRatio)
		{//如果实际发射的粒子数量少于期望的数量
			count = Math::Round<int>(expectRatio * roundTotalCount) - _roundCreateCount;
		}
		if (count + _roundCreateCount > roundTotalCount)
		{//判断是否会超出
			count = roundTotalCount - _roundCreateCount;
		}
		_roundCreateCount += count;
	}

	//创建
	if (count > 0)
	{
		_particleSystem->Emit(count);
	}
}
DC_END_NAMESPACE

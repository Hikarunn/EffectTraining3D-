#include <DxLib.h>
#include <math.h>
#include "../Manager/EffectManager.h"
#include "EffectBase.h"

EffectBase::EffectBase(EffectManager* _effect)
{
	effect_ = _effect;
}

EffectBase::~EffectBase()
{
}

bool EffectBase::Init(void)
{
	return false;
}

bool EffectBase::Terminate(void)
{
	return false;
}

bool EffectBase::Create(void)
{
	return false;
}

bool EffectBase::Delete(void)
{
	return false;
}

bool EffectBase::Step(bool _endRequest, bool _useFlag, float stepTime)
{
	return false;
}

bool EffectBase::Render(void)
{
	return false;
}


bool EffectBase::Setup(COLOR_U8 _color)
{
	return false;
}

EffectManager* EffectBase::GetEffect(void)
{
	return effect_;
}

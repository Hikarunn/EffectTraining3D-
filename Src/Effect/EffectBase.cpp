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


EffectManager* EffectBase::GetEffect(void)
{
	return effect_;
}

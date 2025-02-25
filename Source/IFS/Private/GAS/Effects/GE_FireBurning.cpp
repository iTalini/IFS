#include "GAS/Effects/GE_FireBurning.h"
#include "GAS/IBS_AttributeSet.h"
#include "ScalableFloat.h"

UGE_FireBurning::UGE_FireBurning()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    Period = 1.0f;

    FGameplayModifierInfo Modifier;
    Modifier.Attribute = UIBS_AttributeSet::GetFuelLevelAttribute();
    Modifier.ModifierOp = EGameplayModOp::Additive;
    Modifier.ModifierMagnitude = FScalableFloat(-1.0f);
    Modifiers.Add(Modifier);
}

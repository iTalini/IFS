#include "GAS/Effects/GE_FuelRefill.h"
#include "GAS/IBS_AttributeSet.h"


UGE_FuelRefill::UGE_FuelRefill()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayModifierInfo Modifier;
    Modifier.Attribute = UIBS_AttributeSet::GetFuelLevelAttribute();
    Modifier.ModifierOp = EGameplayModOp::Additive;

    Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FSetByCallerFloat());

    Modifiers.Add(Modifier);
}

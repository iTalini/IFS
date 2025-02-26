#include "GAS/IBS_AttributeSet.h"
#include "IFS/IFSCharacter.h"
#include "Fireplace/Fireplace_Base.h"
#include "GameplayPrediction.h"
#include "GameplayEffectExtension.h"
#include "GameplayAbilitySet.h"
#include "Net/UnrealNetwork.h"

UIBS_AttributeSet::UIBS_AttributeSet()
{
	FuelLevel.SetBaseValue(100.0f);
	FuelLevel.SetCurrentValue(100.0f);
	MaxFuelLevel.SetBaseValue(100.0f);
	MaxFuelLevel.SetCurrentValue(100.0f);
	IsFireOn.SetCurrentValue(0.0f);
}

void UIBS_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UIBS_AttributeSet, FuelLevel);
	DOREPLIFETIME(UIBS_AttributeSet, MaxFuelLevel);
	DOREPLIFETIME(UIBS_AttributeSet, IsFireOn);
}

void UIBS_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetFuelLevelAttribute())
	{
		if (NewValue <= 0.0f)  // Fuel is empty, turn off fire
		{
			FuelLevel.SetCurrentValue(0.0f);
			IsFireOn.SetCurrentValue(0.0f);
		}
	}
	else if (Attribute == GetMaxFuelLevelAttribute())
	{
		AddJustAttributeForMaxChange(GetFuelLevel(), GetMaxFuelLevel(), NewValue, GetFuelLevelAttribute());
	}
}

void UIBS_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTag = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	float DeltaValue{ 0.0f };

	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
	{
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	AFireplace_Base* FireplaceActor{ nullptr };

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* TargetActor{ nullptr };
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		FireplaceActor = Cast<AFireplace_Base>(TargetActor);
	}

	AActor* CauserAcor{ nullptr };

	if (Data.EffectSpec.GetEffectContext().IsValid())
	{
		CauserAcor = Data.EffectSpec.GetEffectContext().GetInstigator();
	}

	if (Data.EvaluatedData.Attribute == GetFuelLevelAttribute())
	{
		float FuelIncrease = Data.EffectSpec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.FuelIncrease")), 0.0f);

		float NewFuelLevel = FMath::Clamp(GetFuelLevel() + FuelIncrease, 0.0f, GetMaxFuelLevel());
		SetFuelLevel(NewFuelLevel);

		SetFuelLevel(FMath::Clamp(GetFuelLevel(), 0.f, GetMaxFuelLevel()));

		if (FireplaceActor && CauserAcor)
		{
			if (GetFuelLevel() <= 0.f)
			{
				IsFireOn.SetCurrentValue(0.0f);
			}
			FireplaceActor->HandleFuelLevelChanged();
		}
	}
}

void UIBS_AttributeSet::OnRep_FuelLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIBS_AttributeSet, FuelLevel, OldValue);
}

void UIBS_AttributeSet::OnRep_MaxFuelLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIBS_AttributeSet, MaxFuelLevel, OldValue);
}

void UIBS_AttributeSet::OnRep_IsFireOn(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIBS_AttributeSet, IsFireOn, OldValue);
}

void UIBS_AttributeSet::AddJustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute,
	float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();

	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (AbilityComp && !FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue))
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta =
			(CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
			: NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UIBS_AttributeSet::CheckFireState(UIBS_AttributeSet* Attributes)
{
	if (Attributes->FuelLevel.GetCurrentValue() <= 0.0f)
	{
		Attributes->IsFireOn.SetCurrentValue(0.0f); // Fire turns off
	}
}

#include "GAS/Abilities/GA_StartFire.h"
#include "AbilitySystemComponent.h"
#include "Fireplace/Fireplace_Base.h"
#include "GAS/IBS_AttributeSet.h"
#include "NiagaraComponent.h"
#include "GAS/Effects/GE_FireBurning.h"


UGA_StartFire::UGA_StartFire()
{
    SetAssetTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Ability.Fire.Start"))));
}

void UGA_StartFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
        return;

    AFireplace_Base* Fireplace = Cast<AFireplace_Base>(GetOwningActorFromActorInfo());
    if (!Fireplace)
    {
        UE_LOG(LogTemp, Error, TEXT("Fireplace Not Found!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!Fireplace || !ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("ASC Not Found!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UIBS_AttributeSet* Attributes = const_cast<UIBS_AttributeSet*>(ASC->GetSet<UIBS_AttributeSet>());
    if (!Attributes)
    {
        UE_LOG(LogTemp, Error, TEXT("Attribute Set Not Found!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (Attributes->IsFireOn.GetCurrentValue() <= 0.0f)
    {
        Attributes->IsFireOn.SetCurrentValue(1.0f);

        TArray<FGameplayAbilitySpecHandle> FoundAbilityHandles;
        FGameplayTag FireBurningTag = FGameplayTag::RequestGameplayTag(FName("Ability.Fire.DecreaseFuel"));

        ASC->FindAllAbilitiesWithTags(FoundAbilityHandles, FGameplayTagContainer(FireBurningTag));

        for (const FGameplayAbilitySpecHandle& SpecHandle : FoundAbilityHandles)
        {
            if (SpecHandle.IsValid())
            {
                ASC->TryActivateAbility(SpecHandle);
            }
        }

        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

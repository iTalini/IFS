#include "GAS/Abilities/GA_AddFuel.h"
#include "GAS/Effects/GE_FuelRefill.h"
#include "Fireplace/Fireplace_Base.h"
#include "GAS/IBS_AttributeSet.h"

UGA_AddFuel::UGA_AddFuel()
{
    FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Abilities.Fireplace.AddFuel"));
    AbilityTags.AddTag(Tag);
}

void UGA_AddFuel::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if(!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
        return;

    AFireplace_Base* Fireplace = Cast<AFireplace_Base>(GetOwningActorFromActorInfo());
    if (!Fireplace)
    {
        UE_LOG(LogTemp, Error, TEXT("Fireplace Not Found!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("ASC Not Found!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(UGE_FuelRefill::StaticClass(), 1.0f, EffectContext);

    if (SpecHandle.IsValid())
    {
        FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
        if (Spec)
        {
            float FuelToAdd = Fireplace->GetMaxFuelLevel() - Fireplace->GetFuelLevel();

            Spec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.FuelIncrease")), FuelToAdd);
        }

        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

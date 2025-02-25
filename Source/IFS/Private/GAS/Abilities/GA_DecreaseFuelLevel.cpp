#include "GAS/Abilities/GA_DecreaseFuelLevel.h"
#include "GAS/Effects/GE_FireBurning.h"
#include "GAS/IBS_AttributeSet.h"
#include "Fireplace/Fireplace_Base.h"


UGA_DecreaseFuelLevel::UGA_DecreaseFuelLevel()
{
    SetAssetTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Ability.Fire.DecreaseFuel"))));
}

void UGA_DecreaseFuelLevel::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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
    if (!ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("ASC Not Found!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UIBS_AttributeSet* Attributes = const_cast<UIBS_AttributeSet*>(ASC->GetSet<UIBS_AttributeSet>());
    if (!Attributes || Attributes->IsFireOn.GetCurrentValue() <= 0.0f)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }


    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(UGE_FireBurning::StaticClass(), 1.0f, EffectContext);

    if (SpecHandle.IsValid())
    {
        ActiveFireEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }

    GetWorld()->GetTimerManager().SetTimer(FireCheckTimerHandle, this, &UGA_DecreaseFuelLevel::CheckFireState, 1.0f, true);
}

void UGA_DecreaseFuelLevel::CheckFireState()
{
    AFireplace_Base* Fireplace = Cast<AFireplace_Base>(GetOwningActorFromActorInfo());
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

    if (!ASC || !Fireplace)
        return;

    UIBS_AttributeSet* Attributes = const_cast<UIBS_AttributeSet*>(Fireplace->GetAbilitySystemComponent()->GetSet<UIBS_AttributeSet>());

    if (!Attributes)
        return;

    if (Attributes->IsFireOn.GetCurrentValue() <= 0.0f || Attributes->GetFuelLevel() <= 0.0f)
    {
        if (ActiveFireEffectHandle.IsValid())
        {
            ASC->RemoveActiveGameplayEffect(ActiveFireEffectHandle);
        }

        GetWorld()->GetTimerManager().ClearTimer(FireCheckTimerHandle);

        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

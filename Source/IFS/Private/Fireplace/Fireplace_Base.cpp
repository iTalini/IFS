// Fill out your copyright notice in the Description page of Project Settings.


#include "Fireplace/Fireplace_Base.h"
#include "AbilitySystemComponent.h"
#include "GAS/Abilities/GA_DecreaseFuelLevel.h"
#include "GAS/IBS_AttributeSet.h"
#include "Components/WidgetComponent.h"
#include "NiagaraComponent.h"
#include "Widgets/BasicProgressBar.h"


// Sets default values
AFireplace_Base::AFireplace_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UIBS_AttributeSet>(TEXT("AttributeSet"));

	FireEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffect"));
	FireEffect->SetupAttachment(RootComponent);
	FireEffect->SetAutoActivate(false);

	bIsAbilitiesInitialized = false;

	UniformCurveScaleName = FName("UniformCurveScale");

	VelocitySpeedScaleName = FName("VelocitySpeedScale");
}

void AFireplace_Base::UpdateFireVisual_Implementation(float NewIntensity)
{
	if (FireEffect)
	{
		if (AttributeSet->IsFireOn.GetCurrentValue() <= 0.0f || (FireEffect->IsActive() && NewIntensity <= 0.0f))
		{
			FireEffect->Deactivate();
			return;
		}

		if (!FireEffect->IsActive() && NewIntensity > 0.0f)
		{
			FireEffect->Activate();
		}

		FireEffect->SetFloatParameter(UniformCurveScaleName, FMath::Clamp(NewIntensity, 0.0f, 1.0f));
		FireEffect->SetFloatParameter(VelocitySpeedScaleName, FMath::Clamp(NewIntensity, 0.0f, 1.0f));
	}
}

UNiagaraComponent* AFireplace_Base::GetFireEffect()
{
	return FireEffect;
}


void AFireplace_Base::BeginPlay()
{
	Super::BeginPlay();
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();

		if (bIsAbilitiesInitialized)
		{
			FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(UGA_DecreaseFuelLevel::StaticClass());
			if (Spec && Spec->Handle.IsValid())
			{
				AbilitySystemComponent->TryActivateAbility(Spec->Handle);
			}
		}
	}
}

// Called every frame
void AFireplace_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AFireplace_Base::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AFireplace_Base::HandleFuelLevelChanged()
{
	if (bIsAbilitiesInitialized)
	{
		OnFuelLevelChanged();
	}
}

void AFireplace_Base::TryToStartFire()
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer FireTags;
		FireTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Fireplace.Start")));

		AbilitySystemComponent->TryActivateAbilitiesByTag(FireTags);
	}
}

void AFireplace_Base::TryToAddFuel()
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer AddFuelTags;
		AddFuelTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Fireplace.AddFuel")));

		AbilitySystemComponent->TryActivateAbilitiesByTag(AddFuelTags);
	}
}

void AFireplace_Base::OnFuelLevelChanged_Implementation()
{
	if (FuelLevelBar)
	{
		float NewFuelPercent = GetFuelLevel() / GetMaxFuelLevel();

		UpdateFireVisual(NewFuelPercent);

		FuelLevelBar->SetBarPercent(NewFuelPercent);
	}

}


float AFireplace_Base::GetFuelLevel() const
{
	if (AttributeSet)
	{
		return AttributeSet->FuelLevel.GetCurrentValue();
	}
	return 1.0f;
}

float AFireplace_Base::GetMaxFuelLevel() const
{
	if (AttributeSet)
	{
		return AttributeSet->MaxFuelLevel.GetBaseValue();
	}
	return 1.0f;
}

void AFireplace_Base::AddStartupGameplayAbilities()
{
	if (/*GetLocalRole() == ROLE_Authority && */!bIsAbilitiesInitialized)
	{
		//Grant abilities, but only on the server

		for (TSubclassOf<class UGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
				StartupAbility,
				1,
				INDEX_NONE,
				this));
		}

		bIsAbilitiesInitialized = true;
	}
}

UBasicProgressBar* AFireplace_Base::GetFuelLevelBar() const
{
	if (FuelLevelBar)
	{
		return FuelLevelBar;
	}

	return nullptr;
}

void AFireplace_Base::SetFuelLevelBar(UBasicProgressBar* WidgetBar)
{
	if (!WidgetBar)
		return;

	if (FuelLevelBar)
	{
		FuelLevelBar->RemoveFromParent();
	}

	FuelLevelBar = WidgetBar;
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimMontage.h"
#include "GA_AddFuel.generated.h"



UCLASS()
class IFS_API UGA_AddFuel : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_AddFuel();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};

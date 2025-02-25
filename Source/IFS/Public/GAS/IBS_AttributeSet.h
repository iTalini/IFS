// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "IBS_AttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)



UCLASS()
class IFS_API UIBS_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UIBS_AttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;


	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FuelLevel, Category = "Attributes")
	FGameplayAttributeData FuelLevel;
	ATTRIBUTE_ACCESSORS(UIBS_AttributeSet, FuelLevel)

	UFUNCTION()
	virtual void OnRep_FuelLevel(const FGameplayAttributeData& OldValue);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxFuelLevel, Category = "Attributes")
	FGameplayAttributeData MaxFuelLevel;
	ATTRIBUTE_ACCESSORS(UIBS_AttributeSet, MaxFuelLevel);

	UFUNCTION()
	virtual void OnRep_MaxFuelLevel(const FGameplayAttributeData& OldValue);


	// Boolean-like float attribute (0 = Fire Off, 1 = Fire On)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsFireOn, Category = "Fire")
	FGameplayAttributeData IsFireOn;
	ATTRIBUTE_ACCESSORS(UIBS_AttributeSet, IsFireOn);

	UFUNCTION()
	virtual void OnRep_IsFireOn(const FGameplayAttributeData& OldValue);


	

	void AddJustAttributeForMaxChange(
		const FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty) const;


	UFUNCTION(BlueprintCallable)
	void CheckFireState(UIBS_AttributeSet* Attributes);
};

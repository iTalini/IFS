#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "Components/WidgetComponent.h"
#include "GameplayTagContainer.h"
#include "Math/Rotator.h"
#include "Fireplace_Base.generated.h"

class UAbilitySystemComponent;
class UIBS_AttributeSet;
class UBasicProgressBar;

UCLASS()
class IFS_API AFireplace_Base : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireplace_Base();

	//Niagara
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateFireVisual(float NewIntensity);

	void UpdateFireVisual_Implementation(float NewIntensity);

	UFUNCTION(BlueprintPure)
	class UNiagaraComponent* GetFireEffect();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fire")
	class UNiagaraComponent* FireEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	FName UniformCurveScaleName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	FName VelocitySpeedScaleName;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;



	virtual void HandleFuelLevelChanged();

	UFUNCTION(BlueprintCallable)
	void TryToStartFire();

	UFUNCTION(BlueprintCallable)
	void TryToAddFuel();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "FuelLevel")
	void OnFuelLevelChanged();

	virtual void OnFuelLevelChanged_Implementation();

	UFUNCTION(BlueprintPure)
	float GetFuelLevel() const;

	UFUNCTION(BlueprintPure)
	float GetMaxFuelLevel() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> GameplayAbilities;

	UPROPERTY()
	uint8 bIsAbilitiesInitialized : 1;

	void AddStartupGameplayAbilities();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UIBS_AttributeSet> AttributeSet;


	//HUD
public:
	UFUNCTION(BlueprintPure)
	class UBasicProgressBar* GetFuelLevelBar() const;

	UFUNCTION(BlueprintCallable)
	void SetFuelLevelBar(class UBasicProgressBar* WidgetBar);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	class UBasicProgressBar* FuelLevelBar;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Math/Color.h"
#include "BasicProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class IFS_API UBasicProgressBar : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// Doing setup in the C++ constructor is not as
	// useful as using NativeConstruct.
	virtual void NativeConstruct() override;

	virtual void NativePreConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	UFUNCTION(BlueprintNativeEvent)
	void ProgressBarChanged();


	UFUNCTION()
	void OnRep_ChangedProgressBar();

	UFUNCTION()
	void OnRep_ChangedText();

	UFUNCTION(reliable, server, WithValidation)
	void SERVER_SetProgressBarPercent(float NewProgressBarPercent);

	UFUNCTION(reliable, server, WithValidation)
	void SERVER_SetBarColor(FLinearColor NewBarColor);


	UPROPERTY(Replicated)
	float ProgressBarPercent;

	UPROPERTY(ReplicatedUsing = OnRep_ChangedProgressBar, EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr <class UProgressBar> ProgressBar;

	UPROPERTY(ReplicatedUsing = OnRep_ChangedText, EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr <class UTextBlock> Text;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	FText TextToPrint;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	FLinearColor BarColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	FLinearColor EmptyBarColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	FLinearColor FullBarColor;

	TWeakObjectPtr<class AIFSCharacter> OwnerCharacter;

public:
	UFUNCTION(BlueprintCallable)
	void SetBarPercent(float NewPercent);

	UFUNCTION(BlueprintCallable)
	void SetBarColor(FLinearColor NewBarColor);

	void SetOwnerCharacter(class AIFSCharacter* NewOwner);
};

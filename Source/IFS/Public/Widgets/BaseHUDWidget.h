// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseHUDWidget.generated.h"


class UBasicProgressBar;
class UHorizontalBox;

/**
 * 
 */
UCLASS()
class IFS_API UBaseHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;

	UFUNCTION(BlueprintCallable)
	void HelpTextVisibility(ESlateVisibility TextVisibility);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "Help Text")
	TObjectPtr <UHorizontalBox> HelpBox;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnimation;
};

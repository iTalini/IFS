// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BasePlayerHUD.generated.h"


class UBaseHUDWidget;

/**
 * 
 */
UCLASS()
class IFS_API ABasePlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual UBaseHUDWidget* GetCurrentWidget() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf <UBaseHUDWidget> HUDWidgetClass;

	UBaseHUDWidget* CurrentWidget;
};

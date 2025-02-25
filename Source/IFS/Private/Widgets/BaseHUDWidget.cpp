// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BaseHUDWidget.h"
#include "Widgets/BasicProgressBar.h"
#include "Components/HorizontalBox.h"


void UBaseHUDWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (HelpBox)
	{
		HelpBox->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBaseHUDWidget::HelpTextVisibility(ESlateVisibility TextVisibility)
{
	if (HelpBox && HelpBox->GetVisibility() != TextVisibility)
	{
		HelpBox->SetVisibility(TextVisibility);

		if (TextVisibility == ESlateVisibility::Visible)
		{
			PlayAnimation(FadeInAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
		}
		else
		{
			PlayAnimation(FadeInAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
		}
	}
}



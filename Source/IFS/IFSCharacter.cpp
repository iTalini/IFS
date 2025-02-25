// Copyright Epic Games, Inc. All Rights Reserved.

#include "IFSCharacter.h"
#include "IFSProjectile.h"
#include "AbilitySystemComponent.h"
#include "GAS/Abilities/GA_AddFuel.h"
#include "GAS/Abilities/GA_StartFire.h"
#include "Fireplace/Fireplace_Base.h"
#include "HUD/BasePlayerHUD.h"
#include "Widgets/BaseHUDWidget.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagsManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AIFSCharacter

AIFSCharacter::AIFSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AIFSCharacter::OnCapsuleComponentBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AIFSCharacter::OnCapsuleComponentOverlapEnd);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	bIsAbilitiesInitialized = false;

}

//////////////////////////////////////////////////////////////////////////// Input

void AIFSCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AIFSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIFSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIFSCharacter::Look);

		EnhancedInputComponent->BindAction(StartFireAction, ETriggerEvent::Triggered, this, &AIFSCharacter::StartFire);
		EnhancedInputComponent->BindAction(AddFuelAction, ETriggerEvent::Triggered, this, &AIFSCharacter::AddFuel);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}



void AIFSCharacter::OnCapsuleComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFireplace_Base* Fireplace = Cast<AFireplace_Base>(OtherActor);
	if (Fireplace)
	{
		if (!NearbyFireplace)
		{
			HUDTextVisibility(ESlateVisibility::Visible);
		}
		NearbyFireplace = FindNearbyFireplace();
	}
}

void AIFSCharacter::OnCapsuleComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFireplace_Base* Fireplace = Cast<AFireplace_Base>(OtherActor);
	if (Fireplace)
	{
		NearbyFireplace = FindNearbyFireplace();
		if (!NearbyFireplace)
		{
			HUDTextVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AIFSCharacter::HUDTextVisibility(ESlateVisibility TextVisibility)
{
	ABasePlayerHUD* HUD = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<ABasePlayerHUD>();
	if (HUD)
	{
		if (UBaseHUDWidget* Widget = HUD->GetCurrentWidget())
		{
			Widget->HelpTextVisibility(TextVisibility);
		}
	}
}

void AIFSCharacter::AddStartupGameplayAbilities()
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

void AIFSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Server GAS init
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}
}

UAbilitySystemComponent* AIFSCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

AFireplace_Base* AIFSCharacter::FindNearbyFireplace()
{
	TArray<AActor*> OverlappingActors;
	GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, AFireplace_Base::StaticClass());

	if (OverlappingActors.IsEmpty())
	{
		return nullptr;
	}

	AFireplace_Base* ClosestFireplace = nullptr;
	float MinDistance = TNumericLimits<float>::Max();
	FVector PlayerLocation = GetActorLocation();

	for (AActor* Actor : OverlappingActors)
	{
		float Distance = FVector::Dist(PlayerLocation, Actor->GetActorLocation());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestFireplace = Cast<AFireplace_Base>(Actor);
		}
	}

	return ClosestFireplace;
}

void AIFSCharacter::AddFuel()
{
	if (NearbyFireplace)
	{
		NearbyFireplace->TryToAddFuel();
	}
}

void AIFSCharacter::StartFire()
{
	if (NearbyFireplace)
	{
		NearbyFireplace->TryToStartFire();
	}
}


void AIFSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AIFSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
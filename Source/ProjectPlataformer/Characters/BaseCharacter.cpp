// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerController/BasePlayerState.h"
#include "EnhancedInputComponent.h"
#include "Common/GameplayTaggedInputAction.h"
#include "EnhancedInputSubsystems.h" 
#include "EnhancedInputSubsystemInterface.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	InputRouter = CreateDefaultSubobject<UEnhancedInputRouter>("InputRouter");
	
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f; 
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (InputRouter == nullptr)
	{
		InputRouter = NewObject<UEnhancedInputRouter>();
	}

	for (const auto Pair : AbilityTriggerActions)
	{
		for (uint8 i = 0; i < FTriggerEventFlags::Count; i++)
		{
			if (uint8 Flag = 1 << i; Pair.Value.TriggerEvents & Flag)
			{
				const ETriggerEvent TriggerEvent = static_cast<ETriggerEvent>(Flag);
				UGameplayTaggedInputAction* Action = Pair.Key;
				
					InputRouter->BindAction(
					Action,
					TriggerEvent, 
					this, 
					&ABaseCharacter::HandleInputGameplayEvent, 
					Action 
				);
			}
		}
	}
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	const ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	return PS? PS->GetAbilitySystemComponent() : nullptr;
}

void ABaseCharacter::ApplyDefaultAbilitiesEffect() const
{
	if (!HasAuthority() || !GetAbilitySystemComponent() || !DefaultAbilitiesEffect) return;
	
	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	
	FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContext.AddSourceObject(PS);
	
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultAbilitiesEffect, 1.0f, EffectContext);
	
	if (SpecHandle.IsValid())
	{
		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ABaseCharacter::HandleInputGameplayEvent(UGameplayTaggedInputAction* TaggedInputAction)
{
	if (!TaggedInputAction)
	{
		UE_LOG(LogTemp, Error, TEXT("HandleInputGameplayEvent called with null TaggedInputAction."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("HandleInputGameplayEvent called. Action: %s | Tag: %s"), *TaggedInputAction->GetName(), *TaggedInputAction->InputTag.ToString());
	
	FGameplayEventData Payload = FGameplayEventData();
	Payload.OptionalObject = TaggedInputAction;

	GetAbilitySystemComponent()->HandleGameplayEvent(TaggedInputAction->InputTag, &Payload);
}
void ABaseCharacter::HandleGameplayEvent(const FGameplayTag Tag) const
{
	const FGameplayEventData Payload = FGameplayEventData();
	GetAbilitySystemComponent()->HandleGameplayEvent(Tag, &Payload);
}

void ABaseCharacter::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MovementVector = Value.Get<FVector2D>();
		
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABaseCharacter::Look(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D LookAxisVector = Value.Get<FVector2D>();
		
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		for (const auto Pair : AbilityTriggerActions)
		{
			const UGameplayTaggedInputAction* Action = Pair.Key;
			
			for (uint8 i = 0; i < FTriggerEventFlags::Count; i++)
			{
				const ETriggerEvent TriggerEvent = static_cast<ETriggerEvent>(1 << i);
				
				EnhancedInputComponent->BindAction(
					Action, 
					TriggerEvent, 
					InputRouter, 
					&UEnhancedInputRouter::HandleRoutedInput, 
					static_cast<const UInputAction*>(Action), 
					TriggerEvent
				);
			}
		}
	}
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		
		ApplyDefaultAbilitiesEffect();
	}
	
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				for (UInputMappingContext* Context : DefaultMappingContexts)
				{
					Subsystem->AddMappingContext(Context, 9999);
				}
			}
		}
	}
}
void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
	}
}

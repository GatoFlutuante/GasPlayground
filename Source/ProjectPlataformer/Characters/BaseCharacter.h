// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "Common/EnhancedInputRouter.h"
#include "Common/GameplayTaggedInputAction.h"
#include "BaseCharacter.generated.h"


USTRUCT(BlueprintType)
struct FTriggerEventFlags
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/EnhancedInput.ETriggerEvent"))
	uint8 TriggerEvents = 0;
	
	static constexpr uint8 Count = 5;
};

UCLASS()
class PROJECTPLATAFORMER_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void ApplyDefaultAbilitiesEffect() const;
	
	UPROPERTY(BlueprintReadOnly)
	UEnhancedInputRouter* InputRouter;
	
	UFUNCTION(BlueprintCallable)
	void HandleInputGameplayEvent(UGameplayTaggedInputAction* TaggedInputAction);
	
	UFUNCTION(BlueprintCallable)
	void HandleGameplayEvent(const FGameplayTag Tag) const;
	
	virtual void Move(const FInputActionValue& Value);
	
	virtual void Look(const FInputActionValue& Value);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities|Init")
	TSubclassOf<class UGameplayEffect> DefaultAbilitiesEffect;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TMap<UGameplayTaggedInputAction*, FTriggerEventFlags> AbilityTriggerActions;
	
	void Input_PressedAbility(UGameplayTaggedInputAction* Action);
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;
	
private:
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;
};

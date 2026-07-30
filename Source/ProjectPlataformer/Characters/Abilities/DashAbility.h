// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DashAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API UDashAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION()
	FVector GetDashDirection();
	UFUNCTION()
	void OnDashFinished();
	UFUNCTION()
	void SetEnabledCameraLag(bool Enable);
	
	
	UPROPERTY(EditDefaultsOnly, Category="Dash|Attributes")
	TSubclassOf<UGameplayEffect> DashGameplayEffect;
	
	UPROPERTY(EditDefaultsOnly, Category="Dash|Attributes")
	float DashDistance = 1200.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Dash|Attributes")
	float DashDuration = 1.0f;
};

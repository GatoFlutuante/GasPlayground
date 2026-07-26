// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SprintAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API USprintAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	void EndAbility();
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	FDelegateHandle CompletedBindingHandle;
	
	UPROPERTY(EditDefaultsOnly, Category="Sprint")
	TSubclassOf<UGameplayEffect> SprintGameplayEffect;
};

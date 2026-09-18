// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RespawnAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API URespawnAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, Category="Respawn | GameplayEffect")
	TSubclassOf<UGameplayEffect> RespawnGameplayEffect;
};

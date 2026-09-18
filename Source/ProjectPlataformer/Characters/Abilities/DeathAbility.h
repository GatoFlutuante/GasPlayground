// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/BaseCharacter.h"
#include "DeathAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API UDeathAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UFUNCTION()
	void Finish();
	
	ABaseCharacter* PlayerCharacter;
	USkeletalMeshComponent* PlayerMesh;
};

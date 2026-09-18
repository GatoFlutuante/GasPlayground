// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "HitReactAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API UHitReactAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UAnimMontage* GetAnimationFromHitDirection(const FHitResult* HitResult);
	
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();
	
	UPROPERTY(EditDefaultsOnly, Category="HitReact|Animation")
	UAnimMontage* ForwardHitReactionAnim;
	UPROPERTY(EditDefaultsOnly, Category="HitReact|Animation")
	UAnimMontage* BackwardHitReactionAnim;
};

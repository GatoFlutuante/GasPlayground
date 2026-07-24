// Copyright Hermit Crab Game Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MoveAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API UMoveAbility : public UGameplayAbility
{
	GENERATED_BODY()

	FDelegateHandle TriggeredBindingHandle;
	FDelegateHandle CompletedBindingHandle;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	void EndAbility();

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};

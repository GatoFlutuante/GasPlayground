// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpAbility.h"

#include "GameFramework/Character.h"

void UJumpAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	Character->Jump();
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

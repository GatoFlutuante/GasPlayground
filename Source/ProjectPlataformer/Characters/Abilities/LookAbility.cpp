// Fill out your copyright notice in the Description page of Project Settings.


#include "LookAbility.h"

#include "Characters/BaseCharacter.h"

void ULookAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor);
	
	const UGameplayTaggedInputAction* LookAction = Cast<UGameplayTaggedInputAction>(TriggerEventData->OptionalObject);
	
	TriggeredBindingHandle = Character->InputRouter->BindAction(LookAction, ETriggerEvent::Triggered, Character, &ABaseCharacter::Look);
	CompletedBindingHandle = Character->InputRouter->BindAction(LookAction, ETriggerEvent::Completed, this, &ULookAbility::EndAbility);
}

void ULookAbility::EndAbility()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void ULookAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

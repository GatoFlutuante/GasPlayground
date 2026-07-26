// Fill out your copyright notice in the Description page of Project Settings.


#include "SprintAbility.h"

#include "AbilitySystemComponent.h"
#include "Characters/BaseCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

void USprintAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SprintGameplayEffect, GetAbilityLevel(), EffectContext);
	if (!SpecHandle.IsValid())
	{
		EndAbility();
	}
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	const UGameplayTaggedInputAction* SprintAction = Cast<UGameplayTaggedInputAction>(TriggerEventData->OptionalObject);
	CompletedBindingHandle = Character->InputRouter->BindAction(SprintAction, ETriggerEvent::Completed, 
		this, &USprintAbility::EndAbility);
}

void USprintAbility::EndAbility()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void USprintAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->RemoveActiveGameplayEffectBySourceEffect(SprintGameplayEffect, ASC, -1);
}

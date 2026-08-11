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
	if (!Character || !Character->InputRouter || !ASC || !TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SprintGameplayEffect, GetAbilityLevel(), EffectContext);
	if (!SpecHandle.IsValid())
	{
		EndAbility();
		return;
	}
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	const UGameplayTaggedInputAction* SprintAction = Cast<UGameplayTaggedInputAction>(TriggerEventData->OptionalObject);
	if (!SprintAction)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

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

	if (ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor))
	{
		Character->InputRouter->UnbindByHandle(CompletedBindingHandle);
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && SprintGameplayEffect)
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(SprintGameplayEffect, ASC, -1);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "CrouchAbility.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

void UCrouchAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	
	if (CharacterIsCrouched())
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(CrouchGameplayEffect, ASC);
		Character->UnCrouch();
		
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}
	
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CrouchGameplayEffect, GetAbilityLevel(), EffectContext);
	if (!SpecHandle.IsValid())
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
	
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	Character->Crouch();
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

bool UCrouchAbility::CharacterIsCrouched()
{
	ACharacter* Character = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor);
	return Character->IsCrouched();
}

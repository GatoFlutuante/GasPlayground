// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDeathAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	PlayerCharacter = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	if (!PlayerCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	
	PlayerCharacter->GetCharacterMovement()->DisableMovement();
	PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PlayerMesh = PlayerCharacter->GetMesh();
	PlayerMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	PlayerMesh->SetSimulatePhysics(true);

	if (UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, 5.0f))
	{
		WaitTask->OnFinish.AddDynamic(this, &UDeathAbility::Finish);
		WaitTask->ReadyForActivation();
	}
}

void UDeathAbility::Finish()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	if (ASC)
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetAvatarActorFromActorInfo();
		Payload.Target = GetAvatarActorFromActorInfo();
		
		FGameplayTag RespawnTag = FGameplayTag::RequestGameplayTag(FName("Event.Character.Respawn"));
		
		ASC->HandleGameplayEvent(RespawnTag, &Payload);
	}
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

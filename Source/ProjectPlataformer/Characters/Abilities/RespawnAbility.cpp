// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnAbility.h"

#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void URespawnAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	USkeletalMeshComponent* PlayerMesh = PlayerCharacter->GetMesh();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	PlayerMesh->SetSimulatePhysics(false);
	PlayerMesh->SetCollisionProfileName(TEXT("Pawn"));
		
	PlayerMesh->AttachToComponent(PlayerCharacter->GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	PlayerMesh->SetRelativeLocationAndRotation(FVector(0, 0, -100), FRotator(0, -90, 0));		

		
	PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		
	if (RespawnGameplayEffect)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(RespawnGameplayEffect, GetAbilityLevel(), EffectContext);
		if (!SpecHandle.IsValid())
		{
			UE_LOG(LogGameplayTags, Warning, TEXT("Spec Handle Failed"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
		
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

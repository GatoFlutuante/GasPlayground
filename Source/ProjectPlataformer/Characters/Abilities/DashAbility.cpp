// Fill out your copyright notice in the Description page of Project Settings.


#include "DashAbility.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

class ABaseCharacter;

void UDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	if (!Character)
	{
		UE_LOG(LogGameplayTags, Warning, TEXT("Character not founded!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	FVector DashDirection = GetDashDirection();
	float DashStrength = DashDistance/DashDuration;
	
	UAbilityTask_ApplyRootMotionConstantForce* DashTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
	this,
	FName("DashTask"),
	DashDirection,
	DashStrength,
	DashDuration,
	false,
	nullptr,
	ERootMotionFinishVelocityMode::SetVelocity, 
	FVector::ZeroVector,                       
	0.0f,
	false
	);
	
	if (!DashTask)
	{
		UE_LOG(LogGameplayTags, Warning, TEXT("Dash Task Failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
	DashTask->OnFinish.AddDynamic(this, &UDashAbility::OnDashFinished);
	DashTask->ReadyForActivation();
	
	
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DashGameplayEffect, GetAbilityLevel(), EffectContext);
	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogGameplayTags, Warning, TEXT("Spec Handle Failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	//Disable camera lag
	SetEnabledCameraLag(false);
}

void UDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->RemoveActiveGameplayEffectBySourceEffect(DashGameplayEffect, ASC, -1);
	SetEnabledCameraLag(true);
}

FVector UDashAbility::GetDashDirection()
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(GetCurrentActorInfo()->AvatarActor);
	if (!Character)
	{
		UE_LOG(LogGameplayTags, Warning, TEXT("Character Not Founded!"));
		return FVector(0,0,0);
	}
	
	FVector DashDirection = Character->GetMovementComponent()->GetLastInputVector();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = Character->GetActorForwardVector();
	}
	return DashDirection;
}

void UDashAbility::OnDashFinished()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UDashAbility::SetEnabledCameraLag(bool Enable)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(GetCurrentActorInfo()->AvatarActor);
	USpringArmComponent* PlayerCamera = Character->FindComponentByClass<USpringArmComponent>();
	
	PlayerCamera->bEnableCameraLag = Enable;
}

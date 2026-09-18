// Fill out your copyright notice in the Description page of Project Settings.


#include "HitReactAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	
	const FHitResult* HitResult = TriggerEventData->ContextHandle.GetHitResult();
	UAnimMontage* MontageResult = GetAnimationFromHitDirection(HitResult);
	
	if (!MontageResult)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,
	NAME_None,
	MontageResult,
	1.0f,
	NAME_None,
	false);
	
	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &UHitReactAbility::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UHitReactAbility::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UHitReactAbility::OnMontageInterrupted);
		
		MontageTask->ReadyForActivation();
	}
}

UAnimMontage* UHitReactAbility::GetAnimationFromHitDirection(const FHitResult* HitResult)
{
	UAnimMontage* MontageResult = nullptr;
	
	if (HitResult)
	{
		FVector ImpactPoint = HitResult->ImpactPoint;

		if (const AActor* TargetActor = GetCurrentActorInfo()->AvatarActor.Get())
		{
			FVector ForwardDirection = TargetActor->GetActorForwardVector();
			FVector DirectionToHit = (ImpactPoint - TargetActor->GetActorLocation()).GetSafeNormal2D();
			
			float ForwardDot = FVector::DotProduct(ForwardDirection, DirectionToHit);
			
			if (ForwardDot > 0.0f)
			{
				//Forward
				MontageResult = ForwardHitReactionAnim;
			}
			else
			{
				//Backward
				MontageResult = BackwardHitReactionAnim;
			}
		}
	}
	
	return MontageResult;
}

void UHitReactAbility::OnMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UHitReactAbility::OnMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

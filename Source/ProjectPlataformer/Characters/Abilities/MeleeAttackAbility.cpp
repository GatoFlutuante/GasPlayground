// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttackAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UMeleeAttackAbility::UMeleeAttackAbility()
{
	HitEventTag = FGameplayTag::RequestGameplayTag(FName("Project.Character.Abilities.Melee.Hit"));
}

void UMeleeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo) || !AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,
	NAME_None,
	AttackMontage,
	1.0f,
	NAME_None,
	false
	);
	
	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &UMeleeAttackAbility::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UMeleeAttackAbility::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UMeleeAttackAbility::OnMontageInterrupted);
		
		MontageTask->ReadyForActivation();
	}
	
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitEventTag,
		nullptr,
		false,
		false
	);
	
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddDynamic(this, &UMeleeAttackAbility::OnHitEventReceived);
		WaitEventTask->ReadyForActivation();
	}
}

void UMeleeAttackAbility::OnMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UMeleeAttackAbility::OnMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}

void UMeleeAttackAbility::OnHitEventReceived(FGameplayEventData Payload)
{
	PerformMeleeTrace();
}

void UMeleeAttackAbility::PerformMeleeTrace()
{
	ACharacter* Character = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor);
	if (!Character)
	{
		return;
	}
	
	FVector Start = Character->GetActorLocation();
	FVector Forward = Character->GetActorForwardVector();
	FVector End = Start + (Forward * 150.0f);
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);
	
	TArray<FHitResult> HitResults;
	
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		Start,
		End,
		60.0f,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResults,
		true
	);
	
	if (bHit && DamageEffectClass)
	{
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
		
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor)
			{
				continue;
			}
			
			UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
			if (TargetASC && SourceASC)
			{
				FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
				Context.AddHitResult(Hit);
				
				FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), Context);
				if (SpecHandle.IsValid())
				{
					SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				}
			}
		}
	}
}

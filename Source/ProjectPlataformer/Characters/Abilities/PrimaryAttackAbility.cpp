// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimaryAttackAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/BaseCharacter.h"
#include "Characters/Components/EquipmentComponent.h"
#include "Containers/Deque.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/EquipmentSubsystem.h"

UPrimaryAttackAbility::UPrimaryAttackAbility()
{
	HitEventTag = FGameplayTag::RequestGameplayTag(FName("Project.Character.Abilities.Melee.Hit"));
}

void UPrimaryAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	PlayerCharacter = Cast<ABaseCharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	EquipComponent = PlayerCharacter->GetEquipmentComponent();
	GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	EquipSys = GI ? GI->GetSubsystem<UEquipmentSubsystem>() : nullptr;
	
	if (!EquipComponent || !EquipSys)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}
	
	if (!CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}
	
	const UGameplayTaggedInputAction* AttackAction = Cast<UGameplayTaggedInputAction>(TriggerEventData->OptionalObject);
	if (!AttackAction)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	StartBindingHandle = PlayerCharacter->InputRouter->BindAction(AttackAction, ETriggerEvent::Started,
		this, &UPrimaryAttackAbility::EnableWantsPerformAttack);
	
	PerformAttack();
}

void UPrimaryAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	PlayerCharacter->InputRouter->UnbindByHandle(StartBindingHandle);
}

void UPrimaryAttackAbility::PerformAttack()
{
	UAnimMontage* AttackMontage = EquipSys->GetItemByID(EquipComponent->CurrentEquippedItemID)->ItemAnimations[ComboIndex];
	
	if (!AttackMontage)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
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
		MontageTask->OnCompleted.AddDynamic(this, &UPrimaryAttackAbility::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UPrimaryAttackAbility::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UPrimaryAttackAbility::OnMontageInterrupted);
		
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
		WaitEventTask->EventReceived.AddDynamic(this, &UPrimaryAttackAbility::OnHitEventReceived);
		WaitEventTask->ReadyForActivation();
	}
}

void UPrimaryAttackAbility::OnMontageCompleted()
{
	PlayerCharacter = Cast<ABaseCharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	EquipComponent = PlayerCharacter->GetEquipmentComponent();
	GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	EquipSys = GI ? GI->GetSubsystem<UEquipmentSubsystem>() : nullptr;
	if (WantsAttackAgain)
	{
		ComboIndex++;
		ComboIndex = (ComboIndex + 1) % EquipSys->GetItemByID(EquipComponent->CurrentEquippedItemID)->ItemAnimations.Num();
		WantsAttackAgain = false;
		PerformAttack();
		return;
	}
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UPrimaryAttackAbility::OnMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}

void UPrimaryAttackAbility::OnHitEventReceived(FGameplayEventData Payload)
{
	PerformMeleeTrace();
}

void UPrimaryAttackAbility::PerformMeleeTrace()
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

void UPrimaryAttackAbility::EnableWantsPerformAttack()
{
	WantsAttackAgain = true;
}

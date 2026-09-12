// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/BaseCharacter.h"
#include "Characters/Components/EquipmentComponent.h"
#include "Subsystems/EquipmentSubsystem.h"
#include "PrimaryAttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API UPrimaryAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPrimaryAttackAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dammage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Events")
	FGameplayTag HitEventTag;
	
	TObjectPtr<ABaseCharacter> PlayerCharacter;
	TObjectPtr<UEquipmentComponent> EquipComponent;
	TObjectPtr<UGameInstance> GI;
	TObjectPtr<UEquipmentSubsystem> EquipSys;
	
	UFUNCTION()
	void PerformAttack();
	
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();
	
	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData Payload);
	
	void PerformMeleeTrace();
	
	void EnableWantsPerformAttack();
	
private:
	UPROPERTY()
	int ComboIndex = 0;
	
	UPROPERTY()
	bool WantsAttackAgain = false;
	
	FDelegateHandle StartBindingHandle;
};

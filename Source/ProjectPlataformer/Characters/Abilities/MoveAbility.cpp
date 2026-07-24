#include "MoveAbility.h"

#include "Common/GameplayTaggedInputAction.h"

#include "EnhancedInputComponent.h"
#include "Characters/BaseCharacter.h"

#include "Common/EnhancedInputRouter.h"



void UMoveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,

const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)

{

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor);

	const UGameplayTaggedInputAction* MoveAction = Cast<UGameplayTaggedInputAction>(TriggerEventData->OptionalObject);



	TriggeredBindingHandle = Character->InputRouter->BindAction(MoveAction, ETriggerEvent::Triggered,

	Character, &ABaseCharacter::Move);


	CompletedBindingHandle = Character->InputRouter->BindAction(MoveAction, ETriggerEvent::Completed,

	this, &UMoveAbility::EndAbility);

}



void UMoveAbility::EndAbility()

{

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);

}



void UMoveAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,

const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)

{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);



	ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor);


	Character->InputRouter->UnbindByHandle(TriggeredBindingHandle);

	Character->InputRouter->UnbindByHandle(CompletedBindingHandle);

} 


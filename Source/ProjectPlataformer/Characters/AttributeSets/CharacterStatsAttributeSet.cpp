// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatsAttributeSet.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCharacterStatsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetGroundSpeedAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);  
	}
}

void UCharacterStatsAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,
                                                      float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetGroundSpeedAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (ASC)
		{
			if (ACharacter* TargetCharacter = Cast<ACharacter>(ASC->GetAvatarActor()))
			{
				if (UCharacterMovementComponent* MovementComp = TargetCharacter->GetCharacterMovement())
				{
					MovementComp->MaxWalkSpeed = NewValue;
				}
			}
		}
	}
}

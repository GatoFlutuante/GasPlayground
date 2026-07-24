// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h" // Seu arquivo de atributos

ABasePlayerState::ABasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UAttributeSet>(TEXT("AttributeSet"));
	
	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* ABasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}




// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "VitalAttributeSet.generated.h"

// Macro padrão do GAS para gerar Getters e Setters automaticamente
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PROJECTPLATAFORMER_API UVitalAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(UVitalAttributeSet, Health);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UVitalAttributeSet, MaxHealth);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS_BASIC(UVitalAttributeSet, Mana);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS_BASIC(UVitalAttributeSet, MaxMana);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS_BASIC(UVitalAttributeSet, IncomingDamage);

	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CharacterStatsAttributeSet.generated.h"

// Macro padrão do GAS para gerar Getters e Setters automaticamente
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API UCharacterStatsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData GroundSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterStatsAttributeSet, GroundSpeed)
	
	
};

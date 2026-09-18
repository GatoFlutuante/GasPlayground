// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "ItemWeaponData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API UItemWeaponData : public UItemData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	float WeaponBaseDamage;
};

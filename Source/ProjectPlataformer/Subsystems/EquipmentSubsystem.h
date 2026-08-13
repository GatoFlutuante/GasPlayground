// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ItemData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EquipmentSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPLATAFORMER_API UEquipmentSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	public:
	UFUNCTION(BlueprintCallable, Category="Equipment")
	UItemData* GetItemByID(FName ItemID);
};

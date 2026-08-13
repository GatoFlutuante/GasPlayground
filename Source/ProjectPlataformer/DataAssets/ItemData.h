// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"


UCLASS(Blueprintable)
class PROJECTPLATAFORMER_API UItemData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	USkeletalMesh* ItemMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FName AttachSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	TSubclassOf<UGameplayEffect> ItemGameplayEffect;
};

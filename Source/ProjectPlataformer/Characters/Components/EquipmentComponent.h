// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTPLATAFORMER_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquipmentComponent();
	
	virtual void BeginPlay() override;

protected:
	
	UPROPERTY(BlueprintReadOnly, Category="Equipment")
	TObjectPtr<USkeletalMeshComponent> EquippedItemMesh;
	
public:
	UPROPERTY(BlueprintReadOnly, Category="Equipment")
	FName CurrentEquippedItemID;
	
	UFUNCTION(BlueprintCallable, Category="Equipment")
	virtual void EquipItem(FName ItemID);
	UFUNCTION(BlueprintCallable, Category="Equipment")
	virtual void UnequipItem();
	
private:
	UFUNCTION(BlueprintCallable, Category="Equipment")
	virtual void AttachEquipmentToPlayer(FName EquipmentSocketName, USkeletalMeshComponent* PlayerMesh);
};

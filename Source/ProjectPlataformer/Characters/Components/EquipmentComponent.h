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
	UEquipmentComponent();

	virtual void BeginPlay() override;

	// ⚡ Obrigatório para registrar variáveis replicadas no C++
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// ⚡ Marcado com ReplicatedUsing para disparar a função OnRep nos clientes quando mudar
	UPROPERTY(ReplicatedUsing = OnRep_CurrentEquippedItemID, BlueprintReadOnly, Category="Equipment")
	FName CurrentEquippedItemID;

	// ⚡ Função chamada AUTOMATICAMENTE nos clientes quando CurrentEquippedItemID é replicado do servidor
	UFUNCTION()
	virtual void OnRep_CurrentEquippedItemID();

	// Funções de Interface chamadas pelo jogador ou UI
	UFUNCTION(BlueprintCallable, Category="Equipment")
	virtual void EquipItem(FName ItemID);

	UFUNCTION(BlueprintCallable, Category="Equipment")
	virtual void UnequipItem();

protected:
	UPROPERTY(BlueprintReadOnly, Category="Equipment")
	TObjectPtr<USkeletalMeshComponent> EquippedItemMesh;

	// ⚡ Função responsável por atualizar os visuais/mesh no Servidor e Clientes
	virtual void UpdateEquipmentVisuals();

	// ⚡ Server RPCs: Garantem que o cliente execute a ação no servidor
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EquipItem(FName ItemID);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UnequipItem();

private:
	void AttachEquipmentToPlayer(FName EquipmentSocketName, USkeletalMeshComponent* PlayerMesh);
};

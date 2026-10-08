// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Subsystems/EquipmentSubsystem.h"

UEquipmentComponent::UEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    SetIsReplicatedByDefault(true);
    CurrentEquippedItemID = NAME_None;
}

void UEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // ⚡ APENAS O SERVIDOR define o item inicial no BeginPlay
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        if (CurrentEquippedItemID == NAME_None)
        {
            EquipItem("DefaultMelee");
        }
    }
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UEquipmentComponent, CurrentEquippedItemID);
}

void UEquipmentComponent::EquipItem(FName ItemID)
{
    // 1. ⚡ REDIRECIONAMENTO DE REDE: Se for chamado pelo Cliente, manda para o Servidor
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        Server_EquipItem(ItemID);
        return;
    }

    if (ItemID.IsNone())
    {
        return;
    }

    // Se tentar equipar o item já equipado, volta para a arma padrão
    if (ItemID == CurrentEquippedItemID)
    {
        EquipItem("DefaultMelee");
        return;
    }

    // Desequipa a arma atual antes de colocar a nova
    if (CurrentEquippedItemID != NAME_None)
    {
        UnequipItem();
    }

    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    UEquipmentSubsystem* EquipSys = GI ? GI->GetSubsystem<UEquipmentSubsystem>() : nullptr;
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    UAbilitySystemComponent* ASC = Character ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character) : nullptr;

    if (!EquipSys || !Character || !ASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("EquipItem Failed: Invalid Subsystem, Character, or ASC!"));
        return;
    }

    UItemData* ItemData = EquipSys->GetItemByID(ItemID);
    if (!ItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("EquipItem Failed: ItemData is null for ID %s"), *ItemID.ToString());
        return;
    }

    // 2. ⚡ SERVIDOR: Atualiza a variável replicada
    CurrentEquippedItemID = ItemID;

    // 3. ⚡ SERVIDOR: Aplica o Gameplay Effect do Item (o GAS replica automaticamente para os clientes)
    if (ItemData->ItemGameplayEffect)
    {
        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
        EffectContext.AddSourceObject(this);
        
        FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ItemData->ItemGameplayEffect, 1.0f, EffectContext);
        if (SpecHandle.IsValid())
        {
            ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }

    // 4. ⚡ SERVIDOR: Atualiza o visual no Servidor/Listen-Server
    UpdateEquipmentVisuals();
}

void UEquipmentComponent::UnequipItem()
{
    // ⚡ REDIRECIONAMENTO DE REDE
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        Server_UnequipItem();
        return;
    }

    if (CurrentEquippedItemID == NAME_None)
    {
        return;
    }
    
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    UEquipmentSubsystem* EquipSys = GI ? GI->GetSubsystem<UEquipmentSubsystem>() : nullptr;

    if (ASC && EquipSys)
    {
        if (UItemData* ItemData = EquipSys->GetItemByID(CurrentEquippedItemID))
        {
            if (ItemData->ItemGameplayEffect)
            {
                // Remove o efeito atrelado a esta arma
                ASC->RemoveActiveGameplayEffectBySourceEffect(ItemData->ItemGameplayEffect, ASC);
            }
        }
    }
    
    // Reseta o ID no Servidor
    CurrentEquippedItemID = NAME_None;

    // Atualiza o visual no Servidor
    UpdateEquipmentVisuals();
}

// ⚡ Chamado automaticamente nos CLIENTES quando 'CurrentEquippedItemID' chega via rede
void UEquipmentComponent::OnRep_CurrentEquippedItemID()
{
    UpdateEquipmentVisuals();
}

// ⚡ Lógica visual centralizada (Roda tanto no Servidor quanto nos Clientes)
void UEquipmentComponent::UpdateEquipmentVisuals()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character)
    {
        return;
    }

    // 1. Limpa o mesh anterior para evitar vazamento de memória
    if (EquippedItemMesh)
    {
        EquippedItemMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
        EquippedItemMesh->DestroyComponent();
        EquippedItemMesh = nullptr;
    }

    // Se desequipou e ficou vazio, encerra aqui
    if (CurrentEquippedItemID.IsNone())
    {
        return;
    }

    // 2. Busca os dados do item para criar o novo mesh visual
    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
    UEquipmentSubsystem* EquipSys = GI ? GI->GetSubsystem<UEquipmentSubsystem>() : nullptr;

    if (EquipSys)
    {
        if (UItemData* ItemData = EquipSys->GetItemByID(CurrentEquippedItemID))
        {
            if (ItemData->ItemMesh)
            {
                EquippedItemMesh = NewObject<USkeletalMeshComponent>(Character);
                if (EquippedItemMesh)
                {
                    EquippedItemMesh->RegisterComponent();
                    EquippedItemMesh->SetSkeletalMesh(ItemData->ItemMesh);
                    
                    // Anexa a mesh ao socket do personagem
                    AttachEquipmentToPlayer(ItemData->AttachSocketName, Character->GetMesh());
                }
            }
        }
    }
}

void UEquipmentComponent::AttachEquipmentToPlayer(FName EquipmentSocketName, USkeletalMeshComponent* PlayerMesh)
{
    if (EquippedItemMesh && PlayerMesh)
    {
        EquippedItemMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipmentSocketName);
    }
}

// -----------------------------------------------------------------------------
// Server RPCs Implementation
// -----------------------------------------------------------------------------

void UEquipmentComponent::Server_EquipItem_Implementation(FName ItemID)
{
    EquipItem(ItemID);
}

bool UEquipmentComponent::Server_EquipItem_Validate(FName ItemID)
{
    return true;
}

void UEquipmentComponent::Server_UnequipItem_Implementation()
{
    UnequipItem();
}

bool UEquipmentComponent::Server_UnequipItem_Validate()
{
    return true;
}
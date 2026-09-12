// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Subsystems/EquipmentSubsystem.h"


// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{

}

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (CurrentEquippedItemID == NAME_None)
	{
		EquipItem("DefaultMelee");
	}
}

void UEquipmentComponent::EquipItem(FName ItemID)
{
	UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEquipmentSubsystem* EquipSys = GI ? GI->GetSubsystem<UEquipmentSubsystem>() : nullptr;
	if (EquipSys == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipSys is null!"));
		return;
	}
	UItemData* ItemData = EquipSys->GetItemByID(ItemID);
	if (ItemData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemData is null!"));
		return;
	}
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is null!"));
		return;
	}
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character);
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC is null!"));
		return;
	}
	
	if (ItemID == CurrentEquippedItemID)
	{
		EquipItem("DefaultMelee");
		return;
	}
	if (CurrentEquippedItemID != NAME_None)
	{
		UnequipItem();
	}
	
	//Cria o skeletal mesh component
	EquippedItemMesh = NewObject<USkeletalMeshComponent>(Character);
	if (EquippedItemMesh)
	{
		EquippedItemMesh->RegisterComponent();
		EquippedItemMesh->SetSkeletalMesh(ItemData->ItemMesh);
		
		//Attach to player
		AttachEquipmentToPlayer(ItemData->AttachSocketName, Character->GetMesh());
	}
	
	//Apply Gameplay Effect
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ItemData->ItemGameplayEffect, 1.0f, EffectContext);
	if (SpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	
	CurrentEquippedItemID = ItemID;
}

void UEquipmentComponent::UnequipItem()
{
	if (CurrentEquippedItemID == NAME_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentEquippedItemID is null!"));
		return;
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UEquipmentSubsystem* EquipSys = GI ? GI->GetSubsystem<UEquipmentSubsystem>() : nullptr;
	if (!ASC || !EquipSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipSys or ASC is null!"));
		return;
	}
	
	if (TSubclassOf<UGameplayEffect> EquipEffect = EquipSys->GetItemByID(CurrentEquippedItemID)->ItemGameplayEffect)
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(EquipEffect, ASC);
	}
	
	if (EquippedItemMesh)
	{
		EquippedItemMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		EquippedItemMesh = nullptr;
	}
	
	CurrentEquippedItemID = NAME_None;
}

void UEquipmentComponent::AttachEquipmentToPlayer(FName EquipmentSocketName, USkeletalMeshComponent* PlayerMesh)
{
	EquippedItemMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipmentSocketName);
}


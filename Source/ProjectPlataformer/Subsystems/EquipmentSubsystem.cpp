// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentSubsystem.h"

#include "DeveloperSettings/GameItemsDataSettings.h"
#include "Structs/FItemDataRow.h"

UItemData* UEquipmentSubsystem::GetItemByID(FName ItemID)
{
	const UGameItemsDataSettings* Settings = GetDefault<UGameItemsDataSettings>();
	if (Settings)
	{
		UDataTable* Table = Settings->GameItemsDataTable.LoadSynchronous();
		if (Table)
		{
			FItemDataRow* Row = Table->FindRow<FItemDataRow>(ItemID, TEXT("GetItemByID"), true);
			if (Row)
			{
				return Row->ItemData;
			}
		}
	}
	return nullptr;
}

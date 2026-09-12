#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataAssets/ItemData.h"

// ⚠️ REGRA DE OURO: O .generated.h DEVE SER O ÚLTIMO INCLUDE!
#include "FItemDataRow.generated.h"

USTRUCT(BlueprintType)
struct FItemDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	UItemData* ItemData;
};

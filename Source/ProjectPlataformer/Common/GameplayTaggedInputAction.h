#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h" // <-- PODE DELETAR ESSE INCLUDE
#include "InputAction.h"      // <-- ADICIONE ESSE NO LUGAR
#include "GameplayTagContainer.h"
#include "GameplayTaggedInputAction.generated.h"

UCLASS()
class PROJECTPLATAFORMER_API UGameplayTaggedInputAction : public UInputAction // <-- MUDE DE UDataAsset PARA UInputAction
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS Input")
	FGameplayTag InputTag;
};
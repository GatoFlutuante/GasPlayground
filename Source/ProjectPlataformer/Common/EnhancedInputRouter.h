
#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "UObject/Object.h"
#include "EnhancedInputRouter.generated.h"

struct FInputActionValue;
class UInputAction;
enum class ETriggerEvent : uint8;

DECLARE_MULTICAST_DELEGATE(FRoutedInputActionDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FRoutedInputActionValueDelegate, const FInputActionValue&);

struct FInputRouteKey
{
	const UInputAction* Action;
	ETriggerEvent TriggerEvent;

	bool operator==(const FInputRouteKey& Other) const 
	{
		return Action == Other.Action && TriggerEvent == Other.TriggerEvent;
	}

	friend uint32 GetTypeHash(const FInputRouteKey& Key) 
	{
		return HashCombine(GetTypeHash(Key.Action), static_cast<uint32>(Key.TriggerEvent));
	}
};

/**
 * 
 */
UCLASS(BlueprintType)
class PROJECTPLATAFORMER_API UEnhancedInputRouter : public UObject
{
	GENERATED_BODY()
	
	TMap<FInputRouteKey, TUniquePtr<FRoutedInputActionValueDelegate>> RoutedDelegates;
	
public:
	template<class UserClass, typename... VarTypes>
	FDelegateHandle BindAction(const UInputAction* Action, const ETriggerEvent TriggerEvent, UserClass* Object,
	                           FRoutedInputActionValueDelegate::FDelegate::TMethodPtr<UserClass, VarTypes...> Func, VarTypes... Vars)
	{
		const TUniquePtr<FRoutedInputActionValueDelegate>* DelegatePtr = &RoutedDelegates.FindOrAdd(
			{Action, TriggerEvent}, 
			MakeUnique<FRoutedInputActionValueDelegate>());
		
		return (*DelegatePtr)->AddUObject(Object, Func, Vars...);
	}
	
	template<class UserClass, typename... VarTypes>
	FDelegateHandle BindAction(const UInputAction* Action, const ETriggerEvent TriggerEvent, UserClass* Object,
	                           FRoutedInputActionDelegate::FDelegate::TMethodPtr<UserClass, VarTypes...> Func, VarTypes... Vars)
	{
		const TUniquePtr<FRoutedInputActionValueDelegate>* DelegatePtr = &RoutedDelegates.FindOrAdd(
			{Action, TriggerEvent}, 
			MakeUnique<FRoutedInputActionValueDelegate>());
		
		return (*DelegatePtr)->AddWeakLambda(Object, 
			[Object, Func, Vars...](const FInputActionValue& IgnoredValue)
			{
				// Unpack the captured parameters and call the user's function
				(Object->*Func)(Vars...);
			}
		);
	}
	
	void UnbindByHandle(const FDelegateHandle HandleToRemove);
	void UnbindByHandle(const UInputAction* Action, const ETriggerEvent TriggerEvent, const FDelegateHandle HandleToRemove);

	UFUNCTION(BlueprintCallable)
	void HandleRoutedInput(const FInputActionValue& Value, const UInputAction* Action, const ETriggerEvent TriggerEvent);
};

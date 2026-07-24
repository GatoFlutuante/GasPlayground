#include "EnhancedInputRouter.h"

void UEnhancedInputRouter::UnbindByHandle(const FDelegateHandle HandleToRemove)
{
	for (const auto& Pair : RoutedDelegates)
	{
		if (Pair.Value->Remove(HandleToRemove))
		{
			break;
		}
	}
}

void UEnhancedInputRouter::UnbindByHandle(const UInputAction* Action, const ETriggerEvent TriggerEvent, const FDelegateHandle HandleToRemove)
{
	const TUniquePtr<FRoutedInputActionValueDelegate>* DelegatePtr = RoutedDelegates.Find({Action, TriggerEvent}); 
	(*DelegatePtr)->Remove(HandleToRemove);
}

void UEnhancedInputRouter::HandleRoutedInput(const FInputActionValue& Value, const UInputAction* Action, const ETriggerEvent TriggerEvent)
{
	if (!Action)
	{
		UE_LOG(LogTemp, Warning, TEXT("Received input action with null pointer"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Router received input. Action: %s | TriggerEvent: %d"), *Action->GetName(), static_cast<int32>(TriggerEvent));	
	const FInputRouteKey Key{Action, TriggerEvent};
        
	if (const TUniquePtr<FRoutedInputActionValueDelegate>* DelegatePtr = RoutedDelegates.Find(Key))
	{
		(*DelegatePtr)->Broadcast(Value);
	}
}
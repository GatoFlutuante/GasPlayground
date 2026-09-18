// Fill out your copyright notice in the Description page of Project Settings.


#include "VitalAttributeSet.h"

#include "GameplayEffectExtension.h"

void UVitalAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
}

void UVitalAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UVitalAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalDamage = GetIncomingDamage();
		SetIncomingDamage(0.0f);
		
		if (LocalDamage > 0.0f)
		{
			const float NewHealth = FMath::Clamp(GetHealth() - LocalDamage, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);
			
			UAbilitySystemComponent* TargetASC = &Data.Target;
			AActor* TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			
			if (NewHealth <= 0.0f)
			{
				//💀 MORTE: Envia evento para disparar a GA_Death
				FGameplayEventData Payload;
				Payload.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
				TargetASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag(FName("Event.Character.Death")), &Payload);
			}
			else
			{
				// 💥 HIT REACT: Envia evento para disparar a GA_HitReact
				FGameplayEventData Payload;
				Payload.ContextHandle = Data.EffectSpec.GetEffectContext();
				TargetASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag(FName("Event.Character.HitReact")), &Payload);
			}
		}
	}
}

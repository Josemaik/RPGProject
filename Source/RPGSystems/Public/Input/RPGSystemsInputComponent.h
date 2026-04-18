// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "RPGInputConfig.h"
#include "RPGSystemsInputComponent.generated.h"

class URPGInputConfig;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGSystemsInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(URPGInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
	
	template<class UserClass, typename FuncType>
	void BindInventoryActions(URPGInputConfig* InputConfig, UserClass* Object, FuncType Func);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void URPGSystemsInputComponent::BindAbilityActions(URPGInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	for (const FRPGInputAction& Action : InputConfig->RPGInputActions)
	{
		if (!IsValid(Action.InputAction) || !Action.InputTag.IsValid())
		{
			continue;
		}

		if (PressedFunc)
		{
			BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
		}
		if (ReleasedFunc)
		{
			BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
		}
	}
}

template <class UserClass, typename FuncType>
void URPGSystemsInputComponent::BindInventoryActions(URPGInputConfig* InputConfig, UserClass* Object, FuncType Func)
{
	check(InputConfig);

	for (const FRPGInputAction& Action : InputConfig->RPGInventoryInputActions)
	{
		if (!IsValid(Action.InputAction)) continue;

		BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, Func, Action.InputTag);
	}
}
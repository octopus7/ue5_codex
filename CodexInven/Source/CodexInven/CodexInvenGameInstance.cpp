// Fill out your copyright notice in the Description page of Project Settings.


#include "CodexInvenGameInstance.h"

#include "CodexInvenInputConfigDataAsset.h"
#include "InputAction.h"
#include "InputMappingContext.h"

const UCodexInvenInputConfigDataAsset* UCodexInvenGameInstance::GetInputConfig() const
{
	return DefaultInputConfig;
}

const UInputMappingContext* UCodexInvenGameInstance::GetInputMappingContext() const
{
	return DefaultInputConfig != nullptr ? DefaultInputConfig->InputMappingContext : nullptr;
}

const UInputAction* UCodexInvenGameInstance::GetInputAction(const ECodexInvenConfiguredInputAction InAction) const
{
	return DefaultInputConfig != nullptr ? DefaultInputConfig->GetInputAction(InAction) : nullptr;
}


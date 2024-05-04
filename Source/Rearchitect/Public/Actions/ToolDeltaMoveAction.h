// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArchitectorTargetManager.h"
#include "ToolActionBase.h"
#include "ToolDeltaMoveAction.generated.h"

/**
 * 
 */
UCLASS()
class REARCHITECT_API UToolDeltaMoveAction : public UToolActionBase
{
	GENERATED_BODY()

public:

	virtual void PerformAction_Implementation() override;

	virtual void UndoAction_Implementation() override;


	UPROPERTY()
	TArray<FArchitectorToolTarget> Targets;

	UPROPERTY()
	FVector Amount;
};

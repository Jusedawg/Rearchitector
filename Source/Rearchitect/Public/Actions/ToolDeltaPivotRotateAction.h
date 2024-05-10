// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolActionBase.h"
#include "ToolDeltaPivotRotateAction.generated.h"

/**
 * 
 */
UCLASS()
class REARCHITECT_API UToolDeltaPivotRotateAction : public UToolActionBase
{
	GENERATED_BODY()



public:
	UPROPERTY() TArray<FArchitectorToolTarget> Targets;
	UPROPERTY() FVector Origin;
	UPROPERTY() double Angle;
	UPROPERTY() FVector Axis;

private:

	virtual void PerformAction_Implementation() override;
	virtual void UndoAction_Implementation() override;
};

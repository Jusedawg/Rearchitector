// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolActionBase.h"
#include "ToolMoveToPositionAction.generated.h"

/**
 * 
 */
UCLASS()
class REARCHITECT_API UToolMoveToPositionAction : public UToolActionBase
{
	GENERATED_BODY()

public:

	virtual void PerformAction_Implementation() override;
	virtual void UndoAction_Implementation() override;

	
	void InitAction(const TArray<FArchitectorToolTarget>& Targets)
	{
		this->Targets = Targets;

		for (const FArchitectorToolTarget& Target : Targets) WorldPositionUndoCache.Add(Target, Target.Target->GetActorLocation());
	}

	UPROPERTY() TArray<FArchitectorToolTarget> Targets;
	UPROPERTY() FVector CurrentDelta;
	
	UPROPERTY() TMap<FArchitectorToolTarget, FVector> WorldPositionUndoCache;
};

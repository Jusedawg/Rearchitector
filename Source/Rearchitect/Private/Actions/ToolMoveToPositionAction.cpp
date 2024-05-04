// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ToolMoveToPositionAction.h"

void UToolMoveToPositionAction::PerformAction_Implementation()
{
	auto RCO = GetRCO();

	FArchitectorTransformData TransformData;
	TransformData.Move = FArchitectorMoveTransform(CurrentDelta);

	RCO->ApplyTransformDataIndependent(Targets, TransformData);
}

void UToolMoveToPositionAction::UndoAction_Implementation()
{
	auto RCO = GetRCO();

	TArray<FArchitectorTargetedTransformData> TransformDatas;

	for (auto& PositionUndoCache : WorldPositionUndoCache)
	{
		FArchitectorTransformData Data;
		Data.Move = FArchitectorMoveTransform(PositionUndoCache.Value, true);
		TransformDatas.Add(FArchitectorTargetedTransformData(PositionUndoCache.Key, Data));
	}

	RCO->ApplyIndividualTransformData(TransformDatas);
}


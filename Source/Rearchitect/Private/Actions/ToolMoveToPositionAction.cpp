// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ToolMoveToPositionAction.h"

void UToolMoveToPositionAction::PerformAction_Implementation()
{
	auto RCO = GetRCO();

	FActorTransformModifyData TransformData;
	TransformData.Move = FPositionModifier(CurrentDelta);

	RCO->ApplyModifyDataToAll(Targets, TransformData);
}

void UToolMoveToPositionAction::UndoAction_Implementation()
{
	auto RCO = GetRCO();

	TArray<FTargetModifyData> TransformDatas;

	for (auto& PositionUndoCache : WorldPositionUndoCache)
	{
		FActorTransformModifyData Data;
		Data.Move = FPositionModifier(PositionUndoCache.Value, true);
		TransformDatas.Add(FTargetModifyData(PositionUndoCache.Key, Data));
	}

	RCO->ApplyModifyDataOnEach(TransformDatas);
}


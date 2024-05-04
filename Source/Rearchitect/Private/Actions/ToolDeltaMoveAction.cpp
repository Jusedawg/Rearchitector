// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ToolDeltaMoveAction.h"

void UToolDeltaMoveAction::PerformAction_Implementation()
{
	auto RCO = GetRCO();

	FArchitectorTransformData TransformData;
	TransformData.Move = FArchitectorMoveTransform(Amount);

	RCO->ApplyTransformDataIndependent(Targets, TransformData);
}

void UToolDeltaMoveAction::UndoAction_Implementation()
{
	auto RCO = GetRCO();

	FArchitectorTransformData TransformData;
	TransformData.Move = FArchitectorMoveTransform(-Amount);

	RCO->ApplyTransformDataIndependent(Targets, TransformData);
}

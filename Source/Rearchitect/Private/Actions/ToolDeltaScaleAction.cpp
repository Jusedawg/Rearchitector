// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ToolDeltaScaleAction.h"

void UToolDeltaScaleAction::PerformAction_Implementation()
{
	FActorTransformModifyData TransformData;
	TransformData.Scale = FScaleModifier(Amount);

	GetRCO()->ApplyTransformDataIndependent(Targets, TransformData);
}

void UToolDeltaScaleAction::UndoAction_Implementation()
{
	FActorTransformModifyData TransformData;
	TransformData.Scale = FScaleModifier(-Amount);

	GetRCO()->ApplyTransformDataIndependent(Targets, TransformData);
}

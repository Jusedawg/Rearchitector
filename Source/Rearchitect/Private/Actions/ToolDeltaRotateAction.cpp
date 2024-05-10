﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ToolDeltaRotateAction.h"

void UToolDeltaRotateAction::PerformAction_Implementation()
{
	auto RCO = GetRCO();

	FActorTransformModifyData TransformData;
	TransformData.Rotate = FRotationModifier(Amount, false);

	RCO->ApplyTransformDataIndependent(Targets, TransformData);	
}

void UToolDeltaRotateAction::UndoAction_Implementation()
{
	auto RCO = GetRCO();

	FActorTransformModifyData TransformData;
	TransformData.Rotate = FRotationModifier(-Amount, false);

	RCO->ApplyTransformDataIndependent(Targets, TransformData);	
}

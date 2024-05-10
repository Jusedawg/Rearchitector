﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ToolDeltaMoveAction.h"

void UToolDeltaMoveAction::PerformAction_Implementation()
{
	auto RCO = GetRCO();

	FActorTransformModifyData TransformData;
	TransformData.Move = FPositionModifier(Amount);

	RCO->ApplyTransformDataIndependent(Targets, TransformData);
}

void UToolDeltaMoveAction::UndoAction_Implementation()
{
	auto RCO = GetRCO();

	FActorTransformModifyData TransformData;
	TransformData.Move = FPositionModifier(-Amount);

	RCO->ApplyTransformDataIndependent(Targets, TransformData);
}

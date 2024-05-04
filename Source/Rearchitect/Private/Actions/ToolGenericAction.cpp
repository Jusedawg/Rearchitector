// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/ToolGenericAction.h"

void UToolGenericAction::PerformAction_Implementation()
{
	auto RCO = GetRCO();

	RCO->ApplyIndividualTransformData(Data);
}

void UToolGenericAction::UndoAction_Implementation()
{
	
}

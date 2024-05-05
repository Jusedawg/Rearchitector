// Fill out your copyright notice in the Description page of Project Settings.


#include "ArchitectorTargetManager.h"
#include "Actions/ToolDeltaMoveAction.h"
#include "Actions/ToolDeltaRotateAction.h"
#include "Actions/ToolDeltaScaleAction.h"
#include "Actions/ToolGenericAction.h"
#include "Actions/ToolMoveToPositionAction.h"
#include "Actions/ToolSetRotateAction.h"
#include "Settings/ArchitectorAxis.h"
#include "Settings/RotationSettings.h"


void FArchitectorTargetManager::DeltaMoveAllIndependent(const FVector& Move)
{
	const auto Delta = Movement.TransformVector(Move);
	auto Action = NewAction<UToolDeltaMoveAction>();
	Action->Amount = Delta;
	Action->Targets = Targets;
	Action->PerformAction();
}

void FArchitectorTargetManager::MoveAllToPosition(const FVector& NewPosition)
{
	const auto OriginPosition = GetTargetListOriginPosition();
	const auto DeltaPosition = Movement.TransformPosition(NewPosition - OriginPosition);
	
	auto Action = GetIfLastOrMakeNew<UToolMoveToPositionAction>();
	
	Action->CurrentDelta = DeltaPosition;
	Action->Targets = Targets;
	Action->PerformAction();
}

void FArchitectorTargetManager::DeltaRotateAllIndependent(const FVector& Rotate)
{
	auto const DeltaRotation = Rotation.ToDeltaRotation(Rotate);
	auto Action = NewAction<UToolDeltaRotateAction>();
	Action->Amount = DeltaRotation;
	Action->Targets = Targets;
	Action->PerformAction();
}

void FArchitectorTargetManager::SetRotationAllIndependent(const FQuat& Quat)
{
	const auto RotationValue = Rotation.AxisLock.ApplyLock(Quat.Euler()).ToOrientationQuat();
	auto Action = NewAction<UToolSetRotateAction>();
	Action->Value = RotationValue;
	Action->Targets = Targets;
	Action->PerformAction();
}

void FArchitectorTargetManager::SetRandomRotation()
{
	TArray<FArchitectorTargetedTransformData> TransformDatas;

	FArchitectorTransformData Data = FArchitectorTransformData();
	for(auto& Target : Targets)
	{
		auto RandomQuat = FQuat(FMath::VRand(), FMath::FRandRange(0.0, 360.0));
		Data.Rotate = FArchitectorRotateTransform(RandomQuat, true);
		
		TransformDatas.Add(FArchitectorTargetedTransformData(Target, Data));
	}

	auto Action = NewAction<UToolGenericAction>();
	Action->Data = TransformDatas;
	Action->PerformAction();
}

void FArchitectorTargetManager::SetRotationToTarget(AActor* Actor, EArchitectorAxis Axis)
{
	if(Actor) SetRotationToPosition(Actor->GetActorLocation(), Axis);
}

void FArchitectorTargetManager::SetRotationToPosition(const FVector& Position, EArchitectorAxis Axis)
{

	TArray<FArchitectorTargetedTransformData> TransformDatas;

	FArchitectorTransformData Data = FArchitectorTransformData();
	for(auto& Target : Targets)
	{
		if(!Target.Target) continue;
		auto PositionDelta = Position - Target.Target->GetActorLocation();
		FMatrix ActorRotation;

		switch (Axis)
		{
		case EArchitectorAxis::X: ActorRotation = FRotationMatrix::MakeFromX(PositionDelta); break;
		case EArchitectorAxis::Y: ActorRotation = FRotationMatrix::MakeFromY(PositionDelta); break;
		case EArchitectorAxis::Z: ActorRotation = FRotationMatrix::MakeFromZ(PositionDelta); break;
		default: return;
		}
		
		Data.Rotate = FArchitectorRotateTransform(ActorRotation.ToQuat(), true);
		
		TransformDatas.Add(FArchitectorTargetedTransformData(Target, Data));
	}

	auto Action = NewAction<UToolGenericAction>();
	Action->Data = TransformDatas;
	Action->PerformAction();
}

void FArchitectorTargetManager::DeltaScaleAll(const FVector& ScaleAxis)
{
	const auto DeltaScale = Scale.TransformVector(ScaleAxis);

	auto Action = NewAction<UToolDeltaScaleAction>();
	Action->Targets = Targets;
	Action->Amount = DeltaScale;
	Action->PerformAction();
}


FVector FArchitectorTargetManager::GetTargetListCenterPosition() const
{
	if(Targets.Num() == 0) return FVector::ZeroVector;

	FVector TargetPositionSum = FVector::ZeroVector;
	int TargetCount = 0;

	for (const auto& Target : Targets)
	{
		if(!Target.Target) continue;
		FVector Origin;
		FVector Bounds;
		Target.Target->GetActorBounds(false, Origin, Bounds, false);
		
		TargetPositionSum += Target.Target->GetActorLocation() + Bounds/2;
		TargetCount++;
	}

	return TargetPositionSum/TargetCount;
}

FVector FArchitectorTargetManager::GetTargetListOriginPosition() const
{
	if(Targets.Num() == 0) return FVector::ZeroVector;
	if(Targets.Num() == 1) return Targets[0].Target->GetActorLocation();
	
	double LowestZ = MAX_dbl;
	FVector TargetPositionSum = FVector::ZeroVector;
	int TargetCount = 0;
	
	for (const auto& Target : Targets)
	{
		if(!Target.Target) continue;

		auto TargetPos = Target.Target->GetActorLocation();
		if(TargetPos.Z < LowestZ) LowestZ = TargetPos.Z;
		TargetPositionSum += TargetPos;
		TargetCount++;
	}

	auto CenterPoint = TargetPositionSum/TargetCount;
	return FVector(CenterPoint.X, CenterPoint.Y, LowestZ);
}



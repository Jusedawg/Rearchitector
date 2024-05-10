// Fill out your copyright notice in the Description page of Project Settings.


#include "ArchitectorTargetManager.h"
#include "Actions/ToolDeltaMoveAction.h"
#include "Actions/ToolDeltaPivotRotateAction.h"
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

void FArchitectorTargetManager::DeltaRotate(const FVector& Axis)
{
	if(Rotation.UsePivot) DeltaRotatePivot(Axis);
	else DeltaRotateAllIndependent(Axis);
}

void FArchitectorTargetManager::DeltaRotateAllIndependent(const FVector& Axis)
{
	auto const DeltaRotation = Rotation.ToDeltaRotation(Axis);
	auto Action = NewAction<UToolDeltaRotateAction>();
	Action->Amount = DeltaRotation;
	Action->Targets = Targets;
	Action->PerformAction();
}

void FArchitectorTargetManager::DeltaRotatePivot(const FVector& Axis)
{
	const auto Origin = GetTargetListOriginPosition();
	const auto Angle = Rotation.RotateDegrees;
	const auto AxisLocked = Rotation.AxisLock.ApplyLock(Axis);
	if(AxisLocked.IsZero()) return;
	
	auto Action = NewAction<UToolDeltaPivotRotateAction>();
	Action->Targets = Targets;
	Action->Origin = GetTargetListCenterPosition();
	Action->Angle = Angle;
	Action->Axis = AxisLocked;
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
	TArray<FTargetModifyData> TransformDatas;

	FActorTransformModifyData Data = FActorTransformModifyData();
	for(auto& Target : Targets)
	{
		auto RandomQuat = FQuat(FMath::VRand(), FMath::FRandRange(0.0, 360.0));
		Data.Rotate = FRotationModifier(RandomQuat, true);
		
		TransformDatas.Add(FTargetModifyData(Target, Data));
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

	TArray<FTargetModifyData> TransformDatas;

	FActorTransformModifyData Data = FActorTransformModifyData();
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
		
		Data.Rotate = FRotationModifier(ActorRotation.ToQuat(), true);
		
		TransformDatas.Add(FTargetModifyData(Target, Data));
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
	FVector Center;
	FVector Bounds;
	UGameplayStatics::GetActorArrayBounds(GetTargetActors(), false, Center, Bounds);

	return Center;
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



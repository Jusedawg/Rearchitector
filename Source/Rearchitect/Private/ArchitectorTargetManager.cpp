// Fill out your copyright notice in the Description page of Project Settings.


#include "ArchitectorTargetManager.h"
#include "ActorUtilities.h"
#include "FGBackgroundThread.h"
#include "FGPlayerController.h"
#include "Settings/ArchitectorAxis.h"
#include "Settings/RotationSettings.h"

void FArchitectorToolTarget::MakeMovable() const
{
	if(!Target) return;
		
	Target->GetRootComponent()->SetMobility(EComponentMobility::Movable);
	if(IsAbstract) InstanceHandle.GetInstanceComponent()->SetMobility(EComponentMobility::Movable);
	
	for(auto& Component : TInlineComponentArray<UFGColoredInstanceMeshProxy*>( Target ))
	{
		UActorUtilities::RefreshInstanceHandle(Component);
	}
}

void FArchitectorToolTarget::ApplyAfterActionPatches() const
{
	//Temporary fix that removes the fog from conveyor and pipe inputs.
	//This removes the pool handles forever, but they can be spawned back with FFGRunnableThread::RegisterActor()
	//Calling that function here is inefficient because this is currently called every frame when used in "Move to position".
	if(auto Buildable = Cast<AFGBuildable>(Target))
	{
		for(auto PoolHandle : Buildable->mPoolHandles)
		{
			PoolHandle->Release();
		}
		Buildable->mPoolHandles.Empty();
	}
}

void FArchitectorToolTarget::DeltaMove(const FVector& Move) const
{
	if(!Target) return;
	
	MakeMovable();

	//Transform actor
	auto Transform = Target->GetTransform();
	Transform.AddToTranslation(Move);
	Target->SetActorTransform(Transform);
		
	if(IsAbstract)
	{
		//Transform abstract instance
		InstanceHandle.GetInstanceComponent()->GetInstanceTransform(InstanceHandle.GetHandleID(), Transform);
		Transform.AddToTranslation(Move);
		InstanceHandle.UpdateTransform(Transform);
	}

	ApplyAfterActionPatches();
}

void FArchitectorToolTarget::DeltaRotate(const FQuat& DeltaRotation) const
{
	if(!Target) return;
		
	MakeMovable();
		
	//Transform actor
	Target->SetActorRotation(DeltaRotation * Target->GetActorQuat());
		
	if(IsAbstract)
	{
		//Transform abstract instance
		FTransform Transform;
		InstanceHandle.GetInstanceComponent()->GetInstanceTransform(InstanceHandle.GetHandleID(), Transform);
			
		Transform.SetRotation(DeltaRotation * Transform.GetRotation());
			
		InstanceHandle.UpdateTransform(Transform);
	}
}

void FArchitectorToolTarget::SetRotation(const FQuat& Quat) const
{
	if(!Target) return;
		
	MakeMovable();
		
	//Transform actor
	Target->SetActorRotation(Quat);
		
	if(IsAbstract)
	{
		//Transform abstract instance
		FTransform Transform;
		InstanceHandle.GetInstanceComponent()->GetInstanceTransform(InstanceHandle.GetHandleID(), Transform);
		Transform.SetRotation(Quat);
		InstanceHandle.UpdateTransform(Transform);
	}
}

void FArchitectorTargetManager::DeltaMoveAllIndependent(const FVector& Move) const
{
	const auto Delta = Movement.TransformVector(Move);
	for (const FArchitectorToolTarget& Target : Targets)
	{
		Target.DeltaMove(Delta);
		Target.ApplyAfterActionPatches();
	}
}

void FArchitectorTargetManager::MoveAllToPosition(const FVector& NewPosition) const
{
	const auto OriginPosition = GetTargetListOriginPosition();
	const auto DeltaPosition = Movement.TransformPosition(NewPosition - OriginPosition);
	
	for(auto& Target : Targets)
	{
		Target.DeltaMove(DeltaPosition);
		Target.ApplyAfterActionPatches();
	}
}

void FArchitectorTargetManager::DeltaRotateAllIndependent(const FVector& Rotate) const
{
	auto const DeltaRotation = Rotation.ToDeltaRotation(Rotate);
	for (const FArchitectorToolTarget& Target : Targets)
	{
		Target.DeltaRotate(DeltaRotation);
		Target.ApplyAfterActionPatches();
	}
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

void FArchitectorTargetManager::SetRotationAllIndependent(const FQuat& Quat) const
{
	const auto RotationValue = Rotation.AxisLock.ApplyLock(Quat.Euler()).ToOrientationQuat();
	for (const FArchitectorToolTarget& Target : Targets)
	{
		Target.SetRotation(RotationValue);
		Target.ApplyAfterActionPatches();
	}
}

void FArchitectorTargetManager::SetRandomRotation() const
{
	for(auto& Target : Targets)
	{
		auto RandomQuat = FQuat(FMath::VRand(), FMath::FRandRange(0.0, 360.0));
		Target.SetRotation(RandomQuat);
		Target.ApplyAfterActionPatches();
	}
}

void FArchitectorTargetManager::SetRotationToTarget(AActor* Actor, EArchitectorAxis Axis) const
{
	if(Actor) SetRotationToPosition(Actor->GetActorLocation(), Axis);
}

void FArchitectorTargetManager::SetRotationToPosition(const FVector& Position, EArchitectorAxis Axis) const
{
	for(auto& Target : Targets)
	{
		if(!Target.Target) continue;
		auto PositionDelta = Position - Target.Target->GetActorLocation();
		UE::Math::TMatrix<double> ActorRotation;

		switch (Axis)
		{
		case EArchitectorAxis::X: ActorRotation = FRotationMatrix::MakeFromX(PositionDelta); break;
		case EArchitectorAxis::Y: ActorRotation = FRotationMatrix::MakeFromY(PositionDelta); break;
		case EArchitectorAxis::Z: ActorRotation = FRotationMatrix::MakeFromZ(PositionDelta); break;
		default: return;
		}
		
		Target.SetRotation(ActorRotation.ToQuat());
	}
}

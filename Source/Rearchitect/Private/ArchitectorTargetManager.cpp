// Fill out your copyright notice in the Description page of Project Settings.


#include "ArchitectorTargetManager.h"
#include "ActorUtilities.h"

void FArchitectorToolTarget::MakeMovable() const
{
	if(!Target) return;

	if(IsAbstract)
	{
		InstanceHandle.GetInstanceComponent()->SetMobility(EComponentMobility::Movable);
	}
		
	Target->GetRootComponent()->SetMobility(EComponentMobility::Movable);
	for(auto& Component : TInlineComponentArray<UFGColoredInstanceMeshProxy*>( Target ))
	{
		UActorUtilities::RefreshInstanceHandle(Component);
	}
}

void FArchitectorToolTarget::PerformMove(const FVector& Move) const
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
}

void FArchitectorToolTarget::PerformRotate(const FVector& Rotate) const
{
	if(!Target) return;
		
	MakeMovable();
		
	auto Angle = FMath::DegreesToRadians( Rotate.Dot(FVector::OneVector) );
	FQuat DeltaRotation = FQuat(Rotate.GetSafeNormal().GetAbs(), Angle);
		
	//Transform actor
	Target->SetActorRotation( DeltaRotation * Target->GetActorQuat() );
		
	if(IsAbstract)
	{
		//Transform abstract instance
		FTransform Transform;
		InstanceHandle.GetInstanceComponent()->GetInstanceTransform(InstanceHandle.GetHandleID(), Transform);
			
		Transform.SetRotation(DeltaRotation * Transform.GetRotation());
			
		InstanceHandle.UpdateTransform(Transform);
	}
}

void FArchitectorTargetManager::MoveAllToPosition(const FVector& NewPosition) const
{
	auto OriginPosition = GetTargetListOriginPosition();

	for(auto& Target : Targets)
	{
		auto TargetOffset = Target.Target->GetActorLocation() - OriginPosition;
		auto NewTargetPosition = NewPosition + TargetOffset;
		auto TargetDelta = NewTargetPosition - Target.Target->GetActorLocation();

		Target.PerformMove(TargetDelta);
	}
	
}

FVector FArchitectorTargetManager::GetTargetListCenterPosition() const
{
	FVector TargetPositionSum;
	int TargetCount = 0;

	for (const auto& Target : TArray(Targets))
	{
		if(!Target.Target) continue;
			
		TargetPositionSum += Target.Target->GetActorLocation();
		TargetCount++;
	}

	return TargetPositionSum/TargetCount;
}

FVector FArchitectorTargetManager::GetTargetListOriginPosition() const
{
	double LowestZ = MAX_dbl;
	FVector TargetPositionSum;
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

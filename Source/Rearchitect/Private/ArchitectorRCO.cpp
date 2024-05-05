#include "ArchitectorRCO.h"

#include "ActorUtilities.h"

void UArchitectorRCO::ApplyTransformDataIndependent_Implementation(const TArray<FArchitectorToolTarget>& Targets, const FArchitectorTransformData& TransformData)
{
	Multicast_ApplyTransformDataIndependent(Targets, TransformData);
}

void UArchitectorRCO::Multicast_ApplyTransformDataIndependent_Implementation(const TArray<FArchitectorToolTarget>& Targets, const FArchitectorTransformData& TransformData)
{
	for (const FArchitectorToolTarget& Target : Targets) PerformActionOnTarget(Target, TransformData);
}

void UArchitectorRCO::ApplyIndividualTransformData_Implementation(const TArray<FArchitectorTargetedTransformData>& Data)
{
	Multicast_ApplyIndividualTransformData_Implementation(Data);
}

void UArchitectorRCO::Multicast_ApplyIndividualTransformData_Implementation(const TArray<FArchitectorTargetedTransformData>& Datas)
{
	for (const FArchitectorTargetedTransformData& Data : Datas) PerformActionOnTarget(Data.Target, Data.TransformData);
}

void UArchitectorRCO::PerformActionOnTarget(const FArchitectorToolTarget& Target, const FArchitectorTransformData& TransformData)
{
	FInstanceHandle InstanceHandle;
	//Make target actor movable
	if(auto Root = Target.Target->GetRootComponent()) Root->SetMobility(EComponentMobility::Movable);
	if(Target.IsAbstract)
	{
		InstanceHandle = Target.GenerateInstanceHandle();
		InstanceHandle.GetInstanceComponent()->SetMobility(EComponentMobility::Movable);
	}
	
	for(auto& Component : TInlineComponentArray<UFGColoredInstanceMeshProxy*>( Target.Target ))
	{
		UActorUtilities::RefreshInstanceHandle(Component);
	}

	//Move
	if(TransformData.Move.IsUsed)
	{
		//Transform actor
		auto Transform = Target.Target->GetTransform();
		auto& PositionValue = TransformData.Move.Value;
			
		if(TransformData.Move.UseAsSetAction) Transform.SetLocation(PositionValue);
		else Transform.AddToTranslation(PositionValue);
			
		Target.Target->SetActorTransform(Transform);
		Target.Target->CachedActorTransform = Transform;
		

		//Transform abstract instance
		if(Target.IsAbstract)
		{
			InstanceHandle.GetInstanceComponent()->GetInstanceTransform(InstanceHandle.GetHandleID(), Transform);

			if(TransformData.Move.UseAsSetAction) Transform.SetLocation(PositionValue);
			else Transform.AddToTranslation(PositionValue);
				
			InstanceHandle.UpdateTransform(Transform);
		}
	}

	//Rotate
	if(TransformData.Rotate.IsUsed)
	{
		//Transform actor
		auto NewActorRotation = TransformData.Rotate.UseAsSetAction
		? TransformData.Rotate.Value
		: TransformData.Rotate.Value * Target.Target->GetActorQuat();
			
		Target.Target->SetActorRotation(NewActorRotation);

		//Transform abstract instance
		if(Target.IsAbstract)
		{
			FTransform Transform;
			InstanceHandle.GetInstanceComponent()->GetInstanceTransform(InstanceHandle.GetHandleID(), Transform);

			auto NewInstanceRotation = TransformData.Rotate.UseAsSetAction
			? TransformData.Rotate.Value
			: TransformData.Rotate.Value * Transform.GetRotation();

			Transform.SetRotation(NewInstanceRotation);
			InstanceHandle.UpdateTransform(Transform);
		}
	}

	//Scale
	if(TransformData.Scale.IsUsed)
	{
		//Transform actor
		auto NewActorScale = TransformData.Scale.UseAsSetAction
		? TransformData.Scale.Value
		: Target.Target->GetActorScale3D() + TransformData.Scale.Value;

		Target.Target->SetActorScale3D(NewActorScale);

		//Transform abstract instance
		if(Target.IsAbstract)
		{
			FTransform Transform;
			InstanceHandle.GetInstanceComponent()->GetInstanceTransform(InstanceHandle.GetHandleID(), Transform);

			auto NewInstanceScale = TransformData.Scale.UseAsSetAction
			? TransformData.Scale.Value
			: Transform.GetScale3D() + TransformData.Scale.Value;

			Transform.SetScale3D(NewInstanceScale);
			InstanceHandle.UpdateTransform(Transform);
		}
	}
}

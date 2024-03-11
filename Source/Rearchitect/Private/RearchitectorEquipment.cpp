
#include "RearchitectorEquipment.h"

#include "ArchitectorRCO.h"
#include "RearchitectorSubsystem.h"


void ARearchitectorEquipment::PerformMove(const FVector& MoveAmount)
{
	if(!TargetManager.HasAnyTargets()) AddActor();
	
	if(Rco)
	{
		Rco->PerformMove(TargetManager, MoveAmount * NudgeAmount);
		RefreshOutline();
	}
}

void ARearchitectorEquipment::PerformRotate(const FVector& Rotation)
{
	if(!TargetManager.HasAnyTargets()) AddActor();
	
	if(Rco)
	{
		Rco->PerformRotate(TargetManager, Rotation * RotateAmount);
		RefreshOutline();
	}
}

void ARearchitectorEquipment::MoveToAimPosition()
{
	if(!TargetManager.HasAnyTargets()) AddActor();

	bool Success;
	auto Data = GetTraceData(10000, TraceTypeQuery1, Success, true);
	if(Success && Rco)
	{
		Rco->BatchMove(TargetManager, Data.Location);
		RefreshOutline();
	}
}

void ARearchitectorEquipment::RefreshOutline()
{
	HideOutlines();
	ShowOutlines();
}

void ARearchitectorEquipment::HideOutlines()
{
	GetInstigatorCharacter()->GetOutline()->HideOutline();
	ARearchitectorSubsystem::Self->HideOutline(TargetManager.GetTargetActors());
}

void ARearchitectorEquipment::ShowOutlines()
{
	ARearchitectorSubsystem::Self->RefreshOutline(TargetManager.GetTargetActors());
}

void ARearchitectorEquipment::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	bool Success;
	auto Data = GetTraceData(10000, TraceTypeQuery1, Success);
	auto Outline = GetInstigatorCharacter()->GetOutline();
	
	if(!Success || !FArchitectorToolTarget::IsValidTarget(Data)) Outline->HideOutline();
	else
	{
		auto Target = FArchitectorToolTarget(Data);
		bool IsTargeted = !TargetManager.HasAnyTargets() || TargetManager.HasTarget(Target);
		Outline->ShowOutline(Target.Target, IsTargeted ? EOutlineColor::OC_HOLOGRAM : EOutlineColor::OC_SOFTCLEARANCE);
	}
}

void ARearchitectorEquipment::AddActor()
{
	bool Success;
	auto Target = GetTraceData(100000, ETraceTypeQuery::TraceTypeQuery1, Success);

	if(Success)
	{
		auto TargetData = FArchitectorToolTarget(Target);
		if(!TargetData.Target) return;
		
		if(TargetManager.HasTarget(TargetData)) TargetManager.RemoveTarget(TargetData);
		else TargetManager.AddTarget(TargetData);
				
		RefreshOutline();
	}
	else ShowNotification(FText::FromString("No target in sight (too far?)"));
}

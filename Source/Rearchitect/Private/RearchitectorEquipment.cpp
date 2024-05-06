
#include "RearchitectorEquipment.h"

#include "ArchitectorRCO.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "RearchitectorSubsystem.h"
#include "PhysicsEngine/PhysicsSettings.h"


void ARearchitectorEquipment::PerformMove(const FVector& MoveAmount)
{
	if(!TargetManager.HasAnyTargets()) AddActor();
	
	TargetManager.DeltaMoveAllIndependent(MoveAmount);
}

void ARearchitectorEquipment::PerformRotate(const FVector& Rotation)
{
	if(!TargetManager.HasAnyTargets()) AddActor();
	
	TargetManager.DeltaRotateAllIndependent(Rotation);
}

void ARearchitectorEquipment::MoveToAimPosition(const FInputActionValue& ActionValue)
{
	if(ActionValue.Get<bool>())
	{
		if(!TargetManager.HasAnyTargets()) AddActor();

		bool Success;
		auto Data = GetTraceData(10000, TraceTypeQuery1, Success, true);
		TargetManager.MoveAllToPosition(Success ? Data.Location : Data.TraceEnd);	
	}
	else TargetManager.StopRecordingMoveAction();
}

void ARearchitectorEquipment::SetRotation(const FQuat& Rotation)
{
	if(!TargetManager.HasAnyTargets()) AddActor();

	TargetManager.SetRotationAllIndependent(Rotation);
}

void ARearchitectorEquipment::RandomRotation()
{
	if(!TargetManager.HasAnyTargets()) AddActor();

	TargetManager.SetRandomRotation();
}

void ARearchitectorEquipment::RotateToTarget(AActor* Target, EArchitectorAxis Axis)
{
	if(!TargetManager.HasAnyTargets()) AddActor();

	TargetManager.SetRotationToTarget(Target, Axis);
}

void ARearchitectorEquipment::RotateToPosition(const FVector& Position, EArchitectorAxis Axis)
{
	if(!TargetManager.HasAnyTargets()) AddActor();
	
	TargetManager.SetRotationToPosition(Position, Axis);
}

void ARearchitectorEquipment::PerformScale(const FVector& Scale)
{
	if(!TargetManager.HasAnyTargets()) AddActor();

	TargetManager.DeltaScaleAll(Scale);
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

FHitResult ARearchitectorEquipment::GetTraceData(double TraceDistance, TEnumAsByte<ETraceTypeQuery> Channel, bool& Success, bool IgnoreTargetedActors)
{
	auto Camera = UGameplayStatics::GetPlayerCameraManager(this, 0);

	FVector Start = Camera->GetCameraLocation();
	FVector End = Camera->GetCameraLocation() + Camera->GetActorForwardVector() * TraceDistance;
	FHitResult Result;

	FCollisionQueryParams TraceParams(TEXT("LineTraceSingle"), SCENE_QUERY_STAT_ONLY(KismetTraceUtils), false);
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable; // Ask for face index, as long as we didn't disable globally
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetInstigatorCharacter());
	
	if(IgnoreTargetedActors) TargetManager.AddIgnoredActorsToTrace(TraceParams);

	Success = GetWorld()->LineTraceSingleByChannel(Result, Start, End, UEngineTypes::ConvertToCollisionChannel(Channel), TraceParams);
	return Result;
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

void ARearchitectorEquipment::InjectMappings()
{
	if(!IsLocallyControlled()) return;
		
	auto Player = Cast<AFGPlayerController>(GetInstigatorController());
	if(!Player) return;
		
		
	auto LocalPlayer = Player->GetLocalPlayer();
	if(!LocalPlayer) return;
		
		
	auto EnhancedInputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if(!EnhancedInputSystem) return;
		
		
	EnhancedInputSystem->AddMappingContext(ToolKeybinds, MappingContextPriority);
	EnhancedInputSystem->AddMappingContext(UIKeybinds, MappingContextPriority);
}

void ARearchitectorEquipment::EjectMappings(bool KeepInterfaceKeybinds)
{
	auto Player = Cast<AFGPlayerController>(GetInstigatorController());
	if(!Player) return;
		
	auto LocalPlayer = Player->GetLocalPlayer();
	if(!LocalPlayer) return;
		
	auto EnhancedInputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if(!EnhancedInputSystem) return;
		
	EnhancedInputSystem->RemoveMappingContext(ToolKeybinds);
	if(!KeepInterfaceKeybinds) EnhancedInputSystem->RemoveMappingContext(UIKeybinds);
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

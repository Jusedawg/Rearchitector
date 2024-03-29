#include "ArchitectorRCO.h"

void UArchitectorRCO::DeltaMove_Implementation(const FArchitectorTargetManager& Manager, const FVector& Move) { Manager.DeltaMoveAllIndependent(Move); }

void UArchitectorRCO::DeltaRotate_Implementation(const FArchitectorTargetManager& Manager, const FVector& Rotate) { Manager.DeltaRotateAllIndependent(Rotate); }

void UArchitectorRCO::BatchMove_Implementation(const FArchitectorTargetManager& Manager, const FVector& Position)
{
	Manager.MoveAllToPosition(Position);
}

void UArchitectorRCO::SetRotate_Implementation(const FArchitectorTargetManager& Manager, const FQuat& Rotation) { Manager.SetRotationAllIndependent(Rotation); }

void UArchitectorRCO::RandomizeRotation_Implementation(const FArchitectorTargetManager& Manager) { Manager.SetRandomRotation(); }

void UArchitectorRCO::SetRotationToTarget_Implementation(const FArchitectorTargetManager& Manager, AActor* Target, EArchitectorAxis Axis) { Manager.SetRotationToTarget(Target, Axis); }

void UArchitectorRCO::SetRotationToPosition_Implementation(const FArchitectorTargetManager& Manager, const FVector& Position, EArchitectorAxis Axis) { Manager.SetRotationToPosition(Position, Axis); }

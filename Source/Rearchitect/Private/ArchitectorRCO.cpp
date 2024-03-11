#include "ArchitectorRCO.h"

void UArchitectorRCO::PerformMove_Implementation(const FArchitectorTargetManager& Manager, const FVector& Move) { Manager.MoveAllIndependent(Move); }

void UArchitectorRCO::PerformRotate_Implementation(const FArchitectorTargetManager& Manager, const FVector& Rotate) { Manager.RotateAllIndependent(Rotate); }

void UArchitectorRCO::BatchMove_Implementation(const FArchitectorTargetManager& Manager, const FVector& Position) { Manager.MoveAllToPosition(Position); }

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractInstanceManager.h"
#include "Buildables/FGBuildable.h"
#include "Settings/ArchitectorAxis.h"
#include "Settings/MovementSettings.h"
#include "Settings/RotationSettings.h"
#include "ArchitectorTargetManager.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FArchitectorToolTarget
{
	GENERATED_BODY()

public:

	UPROPERTY() AActor* Target = nullptr;
	UPROPERTY() bool IsAbstract = false;
	UPROPERTY() FInstanceHandle InstanceHandle;

	FArchitectorToolTarget(){}
	FArchitectorToolTarget(const FHitResult& HitResult)
	{
		if(auto AbstractInstance = Cast<AAbstractInstanceManager>(HitResult.GetActor()))
		{
			AbstractInstance->ResolveHit(HitResult, InstanceHandle);
			Target = InstanceHandle.GetOwner<AActor>();
			IsAbstract = true;
		}
		else Target = Cast<AFGBuildable>(HitResult.GetActor());
	}

	void MakeMovable() const;

	void ApplyAfterActionPatches() const;

	void DeltaMove(const FVector& Move) const;

	void DeltaRotate(const FQuat& DeltaRotation) const;

	void SetRotation(const FQuat& Quat) const;

	bool operator==(const FArchitectorToolTarget& Other) const { return Target == Other.Target; }
	static bool IsValidTarget(const FHitResult& HitResult) { return HitResult.GetActor() && (HitResult.GetActor()->IsA<AFGBuildable>() || HitResult.GetActor()->IsA<AAbstractInstanceManager>()); }
};


USTRUCT(Blueprintable, BlueprintType)
struct FArchitectorTargetManager
{
	GENERATED_BODY()

public:
	void DeltaMoveAllIndependent(const FVector& Move) const;

	void MoveAllToPosition(const FVector& NewPosition) const;
	
	void DeltaRotateAllIndependent(const FVector& Rotate) const;

	FVector GetTargetListCenterPosition() const;

	FVector GetTargetListOriginPosition() const;

	TArray<AActor*> GetTargetActors() const
	{
		TArray<AActor*> Out;
		for (const FArchitectorToolTarget& Target : Targets) Out.Add(Target.Target);
		return Out;
	}

	bool HasAnyTargets() const { return Targets.Num() != 0; }
	bool HasTarget(const FArchitectorToolTarget& Target) const { return Targets.Contains(Target); }
	int TargetCount() const { return Targets.Num(); }

	void AddTarget(const FArchitectorToolTarget& Target) { Targets.Add(Target); }
	void RemoveTarget(const FArchitectorToolTarget& Target) { Targets.Remove(Target); }
	void ClearTargets() { Targets.Empty(); }
	void SetRotationAllIndependent(const FQuat& Quat) const;
	void SetRandomRotation() const;
	void SetRotationToTarget(AActor* Actor, EArchitectorAxis Axis) const;
	void SetRotationToPosition(const FVector& Position, EArchitectorAxis Axis) const;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FArchitectorTargetMovement Movement;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FArchitectorTargetRotation Rotation;
	
private:
	TArray<FArchitectorToolTarget> Targets;
};


UCLASS()
class UArchitectorToolDataFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static bool IsValidTarget(const FHitResult& HitResult) { return FArchitectorToolTarget::IsValidTarget(HitResult); }

	UFUNCTION(BlueprintPure)
	static bool HasAnyTargets(const FArchitectorTargetManager& Manager) { return Manager.HasAnyTargets(); }

	UFUNCTION(BlueprintPure)
	static int GetTargetCount(const FArchitectorTargetManager& Manager) { return Manager.TargetCount(); }

	UFUNCTION(BlueprintCallable)
	static FVector GetOriginPosition(const FArchitectorTargetManager& Manager) { return Manager.GetTargetListOriginPosition(); }

	UFUNCTION(BlueprintCallable)
	static FVector GetCenterPosition(const FArchitectorTargetManager& Manager) { return Manager.GetTargetListCenterPosition(); }

	UFUNCTION(BlueprintCallable)
	static AActor* GetHitActor(const FHitResult& HitResult)
	{
		if(auto AbstractInstance = Cast<AAbstractInstanceManager>(HitResult.GetActor()))
		{
			FInstanceHandle InstanceHandle;
			AbstractInstance->ResolveHit(HitResult, InstanceHandle);
			return InstanceHandle.GetOwner<AActor>();
		}

		return HitResult.GetActor();
	}
};

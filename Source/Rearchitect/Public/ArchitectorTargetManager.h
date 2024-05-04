// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractInstanceManager.h"
#include "Actions/ToolActionBase.h"
#include "Actions/ToolEmptyAction.h"
#include "Buildables/FGBuildable.h"
#include "Settings/ArchitectorAxis.h"
#include "Settings/MovementSettings.h"
#include "Settings/RotationSettings.h"
#include "ArchitectorTargetManager.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FArchitectorTargetManager
{
	GENERATED_BODY()

public:
	void DeltaMoveAllIndependent(const FVector& Move);

	void MoveAllToPosition(const FVector& NewPosition);
	void StopRecordingMoveAction() { History.Add(NewAction<UToolEmptyAction>()); }
	
	void DeltaRotateAllIndependent(const FVector& Rotate);

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
	void SetRotationAllIndependent(const FQuat& Quat);
	void SetRandomRotation();
	void SetRotationToTarget(AActor* Actor, EArchitectorAxis Axis);
	void SetRotationToPosition(const FVector& Position, EArchitectorAxis Axis);

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FArchitectorTargetMovement Movement;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FArchitectorTargetRotation Rotation;

	UPROPERTY()
	UObject* WorldContext;
	
private:
	UPROPERTY()
	TArray<FArchitectorToolTarget> Targets;

	UPROPERTY()
	TArray<UToolActionBase*> History;

	template<class T>
	T* NewAction()
	{
		static_assert(std::is_base_of<UToolActionBase, T>(), "Action must be derived from UToolActionBase class!");
		
		UToolActionBase* Action = NewObject<T>(WorldContext);
		History.Add(Action);
		return (T*)Action;
	}

	template<class T>
	T* GetIfLastOrMakeNew()
	{
		static_assert(std::is_base_of<UToolActionBase, T>(), "Action must be derived from UToolActionBase class!");

		if(History.Num() > 0 && History.Last()->IsA<T>()) return (T*)History.Last();

		return NewAction<T>();
	}
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

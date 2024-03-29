// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArchitectorTargetManager.h"
#include "FGRemoteCallObject.h"
#include "Net/UnrealNetwork.h"
#include "Settings/ArchitectorAxis.h"
#include "ArchitectorRCO.generated.h"



/**
 * 
 */
UCLASS()
class REARCHITECT_API UArchitectorRCO : public UFGRemoteCallObject
{
	GENERATED_BODY()

public:

	UFUNCTION(Server, Reliable)
	void DeltaMove(const FArchitectorTargetManager& Manager, const FVector& Move);

	UFUNCTION(Server, Reliable)
	void DeltaRotate(const FArchitectorTargetManager& Manager, const FVector& Rotate);

	UFUNCTION(Server, Unreliable)
	void BatchMove(const FArchitectorTargetManager& Manager, const FVector& Position);

	UFUNCTION(Server, Reliable)
	void SetRotate(const FArchitectorTargetManager& Manager, const FQuat& Rotation);

	UFUNCTION(Server, Reliable)
	void RandomizeRotation(const FArchitectorTargetManager& Manager);

	UFUNCTION(Server, Reliable)
	void SetRotationToTarget(const FArchitectorTargetManager& Manager, AActor* Target, EArchitectorAxis Axis = X);

	UFUNCTION(Server, Reliable)
	void SetRotationToPosition(const FArchitectorTargetManager& Manager, const FVector& Position, EArchitectorAxis Axis = X);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UArchitectorRCO, RandomPropertyBecauseDocsSaySo)
	}

	UPROPERTY(Replicated)
	int RandomPropertyBecauseDocsSaySo;
};

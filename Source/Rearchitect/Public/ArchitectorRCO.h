// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FGRemoteCallObject.h"
#include "Actions/ArchitectorTransform.h"
#include "Net/UnrealNetwork.h"
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
	void ApplyTransformDataIndependent(const TArray<FArchitectorToolTarget>& Targets, const FArchitectorTransformData& TransformData);

	UFUNCTION(Server, Reliable)
	void ApplyIndividualTransformData(const TArray<FArchitectorTargetedTransformData>& Data);

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyTransformDataIndependent(const TArray<FArchitectorToolTarget>& Targets, const FArchitectorTransformData& TransformData);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyIndividualTransformData(const TArray<FArchitectorTargetedTransformData>& Data);

	void PerformActionOnTarget(const FArchitectorToolTarget& Target, const FArchitectorTransformData& TransformData);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UArchitectorRCO, RandomPropertyBecauseDocsSaySo)
	}

	UPROPERTY(Replicated)
	int RandomPropertyBecauseDocsSaySo;
};

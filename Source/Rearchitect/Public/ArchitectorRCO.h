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
	void ApplyModifyDataToAll(const TArray<FArchitectorToolTarget>& Targets, const FActorTransformModifyData& TransformData);

	UFUNCTION(Server, Reliable)
	void ApplyModifyDataOnEach(const TArray<FTargetModifyData>& Data);

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyModifyDataToAll(const TArray<FArchitectorToolTarget>& Targets, const FActorTransformModifyData& TransformData);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyModifyDataOnEach(const TArray<FTargetModifyData>& Data);

	void PerformActionOnTarget(const FArchitectorToolTarget& Target, const FActorTransformModifyData& TransformData);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UArchitectorRCO, RandomPropertyBecauseDocsSaySo)
	}

	UPROPERTY(Replicated)
	int RandomPropertyBecauseDocsSaySo;
};

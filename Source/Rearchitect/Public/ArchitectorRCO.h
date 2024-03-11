// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArchitectorTargetManager.h"
#include "FGRemoteCallObject.h"
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
	void PerformMove(const FArchitectorTargetManager& Manager, const FVector& Move);

	UFUNCTION(Server, Reliable)
	void PerformRotate(const FArchitectorTargetManager& Manager, const FVector& Rotate);

	UFUNCTION(Server, Unreliable)
	void BatchMove(const FArchitectorTargetManager& Manager, const FVector& Position);
};

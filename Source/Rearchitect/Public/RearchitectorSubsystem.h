// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorUtilities.h"
#include "Subsystem/ModSubsystem.h"
#include "RearchitectorSubsystem.generated.h"

UCLASS()
class AOutlineProxyActor : public AActor
{
	GENERATED_BODY()

public:
	
};

UCLASS()
class REARCHITECT_API ARearchitectorSubsystem : public AModSubsystem
{
	GENERATED_BODY()

	ARearchitectorSubsystem()
	{
		Self = this;
		ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnClient;
	}

	virtual void BeginPlay() override
	{
		Super::BeginPlay();

		Proxy = GetWorld()->SpawnActor<AOutlineProxyActor>();
	}

public:

	static inline ARearchitectorSubsystem* Self;

	void HideOutline(const TArray<AActor*>& OutlinedActors){ UActorUtilities::HideOutlineMultiForActors(OutlinedActors, OutlineProxyData); }
	
	void RefreshOutline(const TArray<AActor*>& InActors)
	{
		UActorUtilities::HideOutlineMultiForActors(InActors, OutlineProxyData);
		UActorUtilities::ShowOutlineMultiForActors(Proxy, InActors, OutlineProxyData, EOutlineColor::OC_HOLOGRAM);
	}


	UPROPERTY()
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> OutlineProxyData;

	UPROPERTY()
	AOutlineProxyActor* Proxy;
};

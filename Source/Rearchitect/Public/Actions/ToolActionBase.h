// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArchitectorRCO.h"
#include "FGGameState.h"
#include "FGPlayerController.h"
#include "UObject/Object.h"
#include "ToolActionBase.generated.h"

/**
 * 
 */
UCLASS()
class REARCHITECT_API UToolActionBase : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PerformAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UndoAction();

	void PostActionLoad(const TArray<FArchitectorToolTarget>& InTargets)
	{
		Targets = InTargets;

		for (const FArchitectorToolTarget& Target : Targets) UndoCache.Add(Target, FActorTransformCachedData(Target.Target->GetActorTransform()));
	}
	
protected:
	UArchitectorRCO* GetRCO() const
	{
		auto Player = Cast<AFGPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if(!Player) return nullptr;

		return Player->GetRemoteCallObjectOfClass<UArchitectorRCO>();
	}

	virtual void PerformAction_Implementation(){}
	virtual void UndoAction_Implementation();

	UPROPERTY(SaveGame) TArray<FArchitectorToolTarget> Targets;
	UPROPERTY(SaveGame) TMap<FArchitectorToolTarget, FActorTransformCachedData> UndoCache;
};

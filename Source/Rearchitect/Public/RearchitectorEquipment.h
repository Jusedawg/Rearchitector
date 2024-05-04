// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArchitectorRCO.h"
#include "ArchitectorTargetManager.h"
#include "EnhancedInputComponent.h"
#include "FGPlayerController.h"
#include "Equipment/FGEquipment.h"
#include "Input/FGInputMappingContext.h"
#include "Settings/ArchitectorAxis.h"
#include "Settings/InterfaceConfiguration.h"
#include "RearchitectorEquipment.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArchitectorUIUpdated);

UCLASS(Blueprintable, BlueprintType)
class UArchitectorToolMappingContext : public UFGInputMappingContext
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UInputAction* SelectActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UInputAction* Nudge;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UInputAction* Rotate;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UInputAction* NudgeAmount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UInputAction* RotateAmount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UInputAction* MoveToAim;
};

UCLASS()
class REARCHITECT_API ARearchitectorEquipment : public AFGEquipment
{

	GENERATED_BODY()
	
public:

	virtual bool ShouldSaveState() const override { return true; }

	UFUNCTION(BlueprintCallable) void PerformMove(const FVector& Translation);
	UFUNCTION(BlueprintCallable) void MoveToAimPosition(const FInputActionValue& ActionValue);

	//Rotate

	///Delta rotate. Accepts rotation vector (i.e. 0 0 1 would rotate on Z axis), adds the converted quaternion to each target's current rotation
	UFUNCTION(BlueprintCallable) void PerformRotate(const FVector& Rotation);

	///Set rotation. Accepts rotation quaternion, sets all targets to that rotation.
	UFUNCTION(BlueprintCallable) void SetRotation(const FQuat& Rotation);

	///Sets random rotation to each target. Made for fun.
	UFUNCTION(BlueprintCallable) void RandomRotation();

	///Sets rotation of each target to look at the target with the given axis.
	UFUNCTION(BlueprintCallable) void RotateToTarget(AActor* Target, EArchitectorAxis Axis = X);
	
	///Sets rotation of each target to look at the world position with the given axis.
	UFUNCTION(BlueprintCallable) void RotateToPosition(const FVector& Position, EArchitectorAxis Axis = X);

	
	UFUNCTION() void RefreshOutline();
	UFUNCTION() void HideOutlines();
	UFUNCTION() void ShowOutlines();
	
	UFUNCTION(BlueprintCallable)
	FHitResult GetTraceData(double TraceDistance, TEnumAsByte<ETraceTypeQuery> Channel, bool& Success, bool IgnoreTargetedActors = false);

	UFUNCTION(BlueprintPure) double GetNudgeAmount() const { return TargetManager.Movement.NudgeAmount; }
	UFUNCTION(BlueprintPure) double GetRotateAmount() const { return TargetManager.Rotation.RotateDegrees; }

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void WasEquipped_Implementation() override
	{
		Super::WasEquipped_Implementation();
		InjectMappings();
		TargetManager.WorldContext = this;

		auto EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
		if(!EnhancedInput) return;
		
		EnhancedInput->BindAction(ToolKeybinds->Nudge, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::Nudge);
		EnhancedInput->BindAction(ToolKeybinds->Rotate, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::Rotate);
		EnhancedInput->BindAction(ToolKeybinds->NudgeAmount, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::SetNudgeAmount);
		EnhancedInput->BindAction(ToolKeybinds->RotateAmount, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::SetRotateAmount);
		EnhancedInput->BindAction(ToolKeybinds->SelectActor, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::AddActor);
		EnhancedInput->BindAction(ToolKeybinds->MoveToAim, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::MoveToAimPosition);
	}

	virtual void WasUnEquipped_Implementation() override
	{
		Super::WasUnEquipped_Implementation();
		if(!IsLocallyControlled()) return;
		
		EjectMappings();
		HideOutlines();
		TargetManager.ClearTargets();
	}

	virtual void OnInteractWidgetAddedOrRemoved(UFGInteractWidget* Widget, bool Added) override
	{
		Super::OnInteractWidgetAddedOrRemoved(Widget, Added);
		
		if(Added) EjectMappings(Widget->Implements<UArchitectorUI>());
		else InjectMappings();
	}

#if !WITH_EDITOR
	virtual void Destroyed() override
	{
		if(IsLocallyControlled()) EjectMappings();
		Super::Destroyed();
	}
#endif
	
	void InjectMappings();

	void EjectMappings(bool KeepInterfaceKeybinds = false);

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FArchitectorTargetManager TargetManager;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FArchitectorInterfaceConfiguration InterfaceConfig;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnArchitectorUIUpdated OnArchitectorUIUpdated;

protected:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ShowNotification(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideHudHints();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DisplayHudHints();
	
	FORCEINLINE bool IsLocallyControlled() const { return GetInstigator() ? GetInstigator()->IsLocallyControlled() : false; }

private:

	void AddActor();

	void Nudge(const FInputActionValue& Value) { PerformMove(Value.Get<FVector>()); }
	void Rotate(const FInputActionValue& Value) { PerformRotate(Value.Get<FVector>()); }

	void SetNudgeAmount(const FInputActionValue& Value)
	{
		double& NudgeAmount = TargetManager.Movement.NudgeAmount;
		
		auto ValueDouble = Value.Get<float>();
		if(NudgeAmount <= 0.1f && ValueDouble < 0) return;
		
		if(NudgeAmount < 1.0f || (NudgeAmount == 1.0f && ValueDouble < 0)) ValueDouble *= 0.1f;
		
		NudgeAmount += ValueDouble;
	}
	
	void SetRotateAmount(const FInputActionValue& Value)
	{
		double& RotateAmount = TargetManager.Rotation.RotateDegrees;
		
		auto ValueDouble = Value.Get<float>();
		if(RotateAmount <= 0.1f && ValueDouble < 0) return;

		if (RotateAmount < 1.0f || (RotateAmount == 1.0f && ValueDouble < 0)) ValueDouble *= 0.1f;
		
		RotateAmount += ValueDouble;
	}

	UPROPERTY(EditDefaultsOnly) int MappingContextPriority = MAX_int32;
	UPROPERTY(EditDefaultsOnly) UArchitectorToolMappingContext* ToolKeybinds;
	UPROPERTY(EditDefaultsOnly) UFGInputMappingContext* UIKeybinds;
};


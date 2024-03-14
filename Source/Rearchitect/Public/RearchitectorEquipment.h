// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractInstanceManager.h"
#include "ArchitectorRCO.h"
#include "ArchitectorTargetManager.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FGPlayerController.h"
#include "Equipment/FGEquipment.h"
#include "Input/FGInputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "RearchitectorEquipment.generated.h"

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UInputAction* UIMode;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UInputAction* MoveToAim;
};

UCLASS()
class REARCHITECT_API ARearchitectorEquipment : public AFGEquipment
{

	GENERATED_BODY()
	
public:

	virtual bool ShouldSaveState() const override { return true; }

	UFUNCTION() void PerformMove(const FVector& Translation);
	UFUNCTION() void PerformRotate(const FVector& Rotation);
	UFUNCTION() void MoveToAimPosition();
	UFUNCTION() void RefreshOutline();
	UFUNCTION() void HideOutlines();
	UFUNCTION() void ShowOutlines();
	UFUNCTION(BlueprintImplementableEvent) void ToggleUI(bool Open);

	UFUNCTION(BlueprintCallable) void OnMoveClicked(const FVector& Direction) { PerformMove(Direction); }
	UFUNCTION(BlueprintCallable) void OnRotateClicked(const FVector& Direction) { PerformRotate(Direction); }
	
	UFUNCTION(BlueprintCallable)
	FHitResult GetTraceData(double TraceDistance, TEnumAsByte<ETraceTypeQuery> Channel, bool& Success, bool IgnoreTargetedActors = false)
	{
		auto Camera = UGameplayStatics::GetPlayerCameraManager(this, 0);

		FVector Start = Camera->GetCameraLocation();
		FVector End = Camera->GetCameraLocation() + Camera->GetActorForwardVector() * TraceDistance;
		FHitResult Result;

		Success = UKismetSystemLibrary::LineTraceSingle(this, Start, End, Channel, false, IgnoreTargetedActors ? TargetManager.GetTargetActors() : TArray<AActor*>(), EDrawDebugTrace::None, Result, true);
		return Result;
	}

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void WasEquipped_Implementation() override
	{
		Super::WasEquipped_Implementation();
		if(IsLocallyControlled())
		{
			InjectMappings();

			RegisterRco();
			auto EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
			auto InputsContext = GetFirstContextOfType<UArchitectorToolMappingContext>();

			if(!EnhancedInput || !InputsContext) return;
			EnhancedInput->BindAction(InputsContext->Nudge, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::Nudge);
			EnhancedInput->BindAction(InputsContext->Rotate, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::Rotate);
			EnhancedInput->BindAction(InputsContext->NudgeAmount, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::SetNudgeAmount);
			EnhancedInput->BindAction(InputsContext->RotateAmount, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::SetRotateAmount);
			EnhancedInput->BindAction(InputsContext->SelectActor, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::AddActor);
			EnhancedInput->BindAction(InputsContext->UIMode, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::OnUIButtonClicked);
			EnhancedInput->BindAction(InputsContext->MoveToAim, ETriggerEvent::Triggered, this, &ARearchitectorEquipment::MoveToAimPosition);
		}
	}

	virtual void WasUnEquipped_Implementation() override
	{
		Super::WasUnEquipped_Implementation();
		if(IsLocallyControlled()) EjectMappings();

		HideOutlines();
		TargetManager.ClearTargets();
	}

	virtual void OnInteractWidgetAddedOrRemoved(UFGInteractWidget* Widget, bool Added) override
	{
		Super::OnInteractWidgetAddedOrRemoved(Widget, Added);
		if(Added) EjectMappings();
		else InjectMappings();
	}

#if !WITH_EDITOR
	virtual void Destroyed() override
	{
		if(IsLocallyControlled()) EjectMappings();
		Super::Destroyed();
	}
#endif

	template<class T>
	T* GetFirstContextOfType()
	{
		for(auto& InputsContext : InputsContexts)
		{
			if(InputsContext.Value->IsA<T>()) return Cast<T>(InputsContext.Value);
		}

		return nullptr;
	}
	
	void InjectMappings()
	{
		auto Player = Cast<AFGPlayerController>(GetInstigatorController());
		if(!Player) return;
		
		
		auto LocalPlayer = Player->GetLocalPlayer();
		if(!LocalPlayer) return;
		
		
		auto EnhancedInputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if(!EnhancedInputSystem) return;
		
		
		for(auto& InputsContext : InputsContexts) EnhancedInputSystem->AddMappingContext(InputsContext.Value, MappingContextPriority);
		ApplyScrollWheelPatch();
	}

	void EjectMappings()
	{
		auto Player = Cast<AFGPlayerController>(GetInstigatorController());
		if(!Player) return;
		
		auto LocalPlayer = Player->GetLocalPlayer();
		if(!LocalPlayer) return;
		
		auto EnhancedInputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if(!EnhancedInputSystem) return;
		
		for(auto& InputsContext : InputsContexts) EnhancedInputSystem->RemoveMappingContext(InputsContext.Value);
		RemoveScrollWheelPatch();
	}
	
	UFUNCTION(BlueprintImplementableEvent) void ApplyScrollWheelPatch();
	UFUNCTION(BlueprintImplementableEvent) void RemoveScrollWheelPatch();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MappingContextPriority = 10000;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FName, UFGInputMappingContext*> InputsContexts;

	UPROPERTY(BlueprintReadWrite)
	FArchitectorTargetManager TargetManager;

protected:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ShowNotification(const FText& Text);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideHudHints();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DisplayHudHints();


	FORCEINLINE bool IsLocallyControlled() const { return GetInstigator() ? GetInstigator()->IsLocallyControlled() : false; }

	static AActor* ResolveHit(const FHitResult& hit, FVector& Location)
	{
		Location = hit.Location;
		if(auto abstractHit = Cast<AAbstractInstanceManager>(hit.GetActor()))
		{
			FInstanceHandle handle;
			abstractHit->ResolveHit(hit, handle);
			return handle.GetOwner<AActor>();
		}
		return hit.GetActor();
	}

private:

	void RegisterRco()
	{
		auto Controller = Cast<AFGPlayerController>(GetInstigatorController());
		
		if(Controller) Rco = Controller->GetRemoteCallObjectOfClass<UArchitectorRCO>();
	}

	void AddActor();

	void Nudge(const FInputActionValue& Value) { OnMoveClicked(Value.Get<FVector>()); }
	void Rotate(const FInputActionValue& Value) { OnRotateClicked(Value.Get<FVector>()); }

	void SetNudgeAmount(const FInputActionValue& Value)
	{
		double& NudgeAmount = TargetManager.NudgeAmount;
		
		auto ValueDouble = Value.Get<float>();
		if(NudgeAmount <= 0.1f && ValueDouble < 0) return;
		
		if(NudgeAmount < 1.0f || (NudgeAmount == 1.0f && ValueDouble < 0)) ValueDouble *= 0.1f;
		
		NudgeAmount += ValueDouble;
	}
	void SetRotateAmount(const FInputActionValue& Value)
	{
		double& RotateAmount = TargetManager.RotateAmount;
		auto ValueDouble = Value.Get<float>();
		if(RotateAmount <= 0.1f && ValueDouble < 0) return;

		if (RotateAmount < 1.0f || (RotateAmount == 1.0f && ValueDouble < 0)) ValueDouble *= 0.1f;
		
		RotateAmount += ValueDouble;
	}

	void OnUIButtonClicked(const FInputActionValue& Value) { ToggleUI(Value.Get<bool>()); }
	
	UPROPERTY() UArchitectorRCO* Rco;
};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InterfaceUtilities.generated.h"

/**
 * 
 */
UCLASS()
class REARCHITECT_API UInterfaceUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static void GetWidgetUnderCursor()
	{
		FWidgetPath Path = FSlateApplication::Get().LocateWindowUnderMouse(FSlateApplication::Get().GetCursorPos(), FSlateApplication::Get().GetInteractiveTopLevelWindows(), true);
		if(Path.IsValid()) UE_LOG(LogTemp, Warning, TEXT("%s"), *Path.GetLastWidget()->ToString())
	}
};

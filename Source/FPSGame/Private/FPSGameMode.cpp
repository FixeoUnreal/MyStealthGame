// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include <Kismet/GameplayStatics.h>

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		InstigatorPawn->DisableInput(nullptr);

		if(SpectatingViewpointClass)
		{
			TArray<AActor*> Viewpoints;
			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, Viewpoints);

			// Change viewtarget if any valdid viewpoint found
			if (Viewpoints.Num() > 0)
			{
				AActor* NewViewTarget = Viewpoints[0];
				APlayerController* PlayerController = Cast<APlayerController>(InstigatorPawn->GetController());
				if (PlayerController)
				{
					PlayerController->SetViewTargetWithBlend(
						NewViewTarget,
						3.f,
						EViewTargetBlendFunction::VTBlend_Cubic
					);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SpectatingViewpointClass is missing. Please update GameMode class with valid subclass. Can not change spectating view target."));
		}
	}
	

	OnMissionCompleted(InstigatorPawn);

	
}

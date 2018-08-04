// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"
#include <Engine/World.h>
#include "FPSPlayerController.h"




void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			// Do BP things after mission completed (e.g. visualisation)
			PC->OnMissionCompleted(InstigatorPawn, bMissionSuccess);
			
			// Disable input
			APawn* ControlledPawn = PC->GetPawn();
			if (ControlledPawn)
			{
				ControlledPawn->DisableInput(PC);
			}
		}
	}
}

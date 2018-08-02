// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAICharacter.h"
#include <Perception/PawnSensingComponent.h>
#include <DrawDebugHelpers.h>


// Sets default values
AFPSAICharacter::AFPSAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
}

// Called when the game starts or when spawned
void AFPSAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAICharacter::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAICharacter::OnNoiseHeard);
}

void AFPSAICharacter::OnPawnSeen(APawn* SeenPawn)
{
	if (!SeenPawn) { return; }
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.f, 12, FColor::Orange, 10.f);
}

void AFPSAICharacter::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	DrawDebugSphere(GetWorld(), Location, 32.f, 12, FColor::Green, 10.f);
}

// Called every frame
void AFPSAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


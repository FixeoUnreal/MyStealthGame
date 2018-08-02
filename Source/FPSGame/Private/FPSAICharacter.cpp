// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAICharacter.h"
#include <Perception/PawnSensingComponent.h>
#include <DrawDebugHelpers.h>
#include "TimerManager.h"
#include "FPSGameMode.h"
#include "Engine/World.h"

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

	OriginalRotation = GetActorRotation();
}

void AFPSAICharacter::OnPawnSeen(APawn* SeenPawn)
{
	if (!SeenPawn) { return; }
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.f, 12, FColor::Orange, 10.f);

	AFPSGameMode* GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->CompleteMission(SeenPawn, false);
	}

	SetGuardState(EAIState::Alerted);
}

void AFPSAICharacter::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted) { return; }

	DrawDebugSphere(GetWorld(), Location, 32.f, 12, FColor::Green, 10.f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0;
	NewLookAt.Roll = 0;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAICharacter::ResetOrientation, 3.f);

	SetGuardState(EAIState::Suspicious);
}

void AFPSAICharacter::ResetOrientation()
{
	if (GuardState == EAIState::Alerted) { return; }

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);
}

void AFPSAICharacter::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState) { return; }

	GuardState = NewState;

	OnGuardStateChanged(GuardState);
}

// Called every frame
void AFPSAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


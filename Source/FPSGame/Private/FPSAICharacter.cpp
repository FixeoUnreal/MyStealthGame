// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAICharacter.h"
#include <Perception/PawnSensingComponent.h>
#include <DrawDebugHelpers.h>
#include "TimerManager.h"
#include "FPSGameMode.h"
#include "Engine/World.h"
#include <AIController.h>
#include <Engine/TargetPoint.h>

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

	if (!bPatrol || PatrollingPoints.Num() <= 0) { return; }
	CurrentNextPoint = PatrollingPoints[CurrentNextPointIndex];
	MoveToNextPoint(CurrentNextPoint);
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
	StopAIMovement();
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
	StopAIMovement();
}

void AFPSAICharacter::ResetOrientation()
{
	if (GuardState == EAIState::Alerted) { return; }

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);

	MoveToNextPoint(CurrentNextPoint);
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

	if (!bPatrol) { return; }

	float Distance = FVector::Distance(GetActorLocation(), CurrentNextPoint->GetActorLocation());
	if (Distance < 100)
	{
		// Move to next patrol point
		CurrentNextPointIndex = (CurrentNextPointIndex + 1) % PatrollingPoints.Num();
		CurrentNextPoint = PatrollingPoints[CurrentNextPointIndex];
		GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
		GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAICharacter::ResetOrientation, 5.f);
	}
}

void AFPSAICharacter::StopAIMovement()
{
	if (Controller)
	{
		Controller->StopMovement();
	}
}

void AFPSAICharacter::MoveToNextPoint(AActor* NextPoint)
{
	if (!Controller || PatrollingPoints.Num() <= 0) { return; }
	AAIController* AIController = Cast<AAIController>(Controller);
	if (!AIController) { return; }

	AIController->MoveToActor(NextPoint, 5.f);
}


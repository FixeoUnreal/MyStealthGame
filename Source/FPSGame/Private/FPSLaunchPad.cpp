// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSLaunchPad.h"
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>


// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	BaseComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseComp"));
	BaseComp->SetupAttachment(RootComponent);
	BaseComp->SetGenerateOverlapEvents(false);

	SurfaceComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SurfaceComp"));
	SurfaceComp->SetupAttachment(BaseComp);
	SurfaceComp->SetGenerateOverlapEvents(false);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(SurfaceComp);

}

// Called when the game starts or when spawned
void AFPSLaunchPad::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::LaunchOverlappingActor);
}

void AFPSLaunchPad::LaunchOverlappingActor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	FVector ForwardForceVector = BoxComp->GetForwardVector() * 500;
	float UpwardsForce = 1500.f;
	FVector LaunchVelocity = ForwardForceVector + FVector(0, 0, UpwardsForce);
	ACharacter* OverlappingActor = Cast<ACharacter>(OtherActor);

	if (OverlappingActor)
	{
		OverlappingActor->LaunchCharacter(LaunchVelocity, true, true);
		UGameplayStatics::SpawnEmitterAtLocation(this, LaunchEffect, GetActorLocation());
	}
	else if(OtherComp && OtherComp->IsSimulatingPhysics())
	{
		// Reset current velocity of the object
		OtherComp->SetAllPhysicsLinearVelocity(FVector(0));

		OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);
		UGameplayStatics::SpawnEmitterAtLocation(this, LaunchEffect, GetActorLocation());
	}
}

// Called every frame
void AFPSLaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


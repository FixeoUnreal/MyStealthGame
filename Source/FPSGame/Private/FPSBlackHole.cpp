// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSBlackHole.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Components/PrimitiveComponent.h>


// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);

	InnerSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComp"));
	InnerSphereComp->SetupAttachment(MeshComp);
	InnerSphereComp->SetSphereRadius(64);
	InnerSphereComp->OnComponentBeginOverlap.AddDynamic(this ,&AFPSBlackHole::OverlapInnerSphere);

	OuterSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphereComp"));
	OuterSphereComp->SetupAttachment(MeshComp);
	OuterSphereComp->SetSphereRadius(1500);
}

// Called when the game starts or when spawned
void AFPSBlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFPSBlackHole::OverlapInnerSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	OtherActor->Destroy();
}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> OverlapComps;
	OuterSphereComp->GetOverlappingComponents(OverlapComps);

	if (OverlapComps.Num() <= 0) { return; }

	for(UPrimitiveComponent* OverlapComp : OverlapComps)
	{
		if(!OverlapComp || !OverlapComp->IsSimulatingPhysics()){ continue; }
		float Radius = OuterSphereComp->GetScaledSphereRadius();
		OverlapComp->AddRadialForce(GetActorLocation(), Radius, ForceStrength, ERadialImpulseFalloff::RIF_Constant, true);
	}
}


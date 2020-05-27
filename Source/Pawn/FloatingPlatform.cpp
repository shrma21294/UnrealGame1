// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);

	bInterping = false;

	InterpSpeed =  4.f;

	InterpTime = 1.f;


}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	bInterping = false;

	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);

	Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping)
	{
		//Doing interpolation between current location and the end point
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);

		float DistanceTravelled = (GetActorLocation() - StartPoint).Size();

		//After reaching the endpoint, we need to come back to start point - like ping pong btw two points.
		//we are doing it a sec delay
		if (Distance - DistanceTravelled <= 1.f)
		{
			ToggleInterping();

			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);

			SwapVectors(StartPoint, EndPoint);
		}
	}
	

}

void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector& Vec1, FVector& Vec2)
{
	FVector Temp = Vec1;
	Vec1 = Vec2;
	Vec2 = Temp;
}


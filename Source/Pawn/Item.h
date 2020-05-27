// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class PAWN_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	//Base shape collision
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Collision")
	class USphereComponent* CollisionVolume;

	//Base mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	class UStaticMeshComponent* Mesh;
	
	//ParticleComponent needs to be created using CreateDefaultSubobject whereas Particle system does not 
	//- it simply be selected in the blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UParticleSystemComponent* IdleParticlesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | OverlapParticles")
	class UParticleSystem* OverlapParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	class USoundCue* OverlapSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties")
	bool bRotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties")
	float RotationRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//virtual function which can be overriden by child classes
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

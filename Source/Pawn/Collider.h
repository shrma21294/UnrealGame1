// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class PAWN_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* Camera;

	//help to control camera motion
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* SpringArm;

	//Anywhere in your code you have meshcomponent, it will treat it like a macro - it will take the return value
	//of GetMeshcomponent and paste it wherever GetMeshCompnent() is called - it happens at compile time.(faster)
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return MeshComponent; }
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh; }

	FORCEINLINE USphereComponent* GetSphereComponent() { return SphereComponent; }
	FORCEINLINE void SetMeshComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }

	FORCEINLINE UCameraComponent* GetCameraComponent() { return Camera; }
	FORCEINLINE void SetMeshComponent(UCameraComponent* InCamera) { Camera = InCamera; }

	FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArm; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* InSpringArm) { SpringArm = InSpringArm; }

private:
	void MoveForward(float input);
	void MoveRight(float input);


};

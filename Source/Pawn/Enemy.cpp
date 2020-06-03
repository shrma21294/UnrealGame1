// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	//added socket to the enemy on the left arm knee named - enemy socket - this is for enemy combat
	CombatCollisionEnemy = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollisionEnemy"));
	//CombatCollisionEnemy->RegisterComponent();
	//CombatCollisionEnemy->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));
	CombatCollisionEnemy->SetupAttachment(GetMesh(), FName("EnemySocket"));

	bOverlappingCombatSphere = false;

	Health = 75.f;
	MaxHealth = 100.f;
	Damage = 10.f;

	AttackMinTime = 0.5f;

	AttackMaxTime = 3.5f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//to control the Enemy - spider here, we need controller which comes form AI controller class
	//Here we are casting hte controller to AAIController
	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	CombatCollisionEnemy->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollisionEnemy->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	//setting the collsion parameters
	CombatCollisionEnemy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollisionEnemy->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollisionEnemy->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollisionEnemy->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


 void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
 {
	 if (OtherActor)
	 {
		 AMain* Main = Cast<AMain>(OtherActor);
		 if (Main)
		 {
			 MoveToTarget(Main);
		 }
	 }
 }


 void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
 {
	 if (OtherActor)
	 {
		 AMain* Main = Cast<AMain>(OtherActor);
		 if (Main)
		 {
			 SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			 if (AIController)
			 {
				 AIController->StopMovement();
			 }
		 }
	 }
 }


 void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
 {
	 if (OtherActor)
	 {
		 AMain* Main = Cast<AMain>(OtherActor);
		 {
			 if (Main)
			 {
				 Main->SetCombatTarget(this);
				 CombatTarget = Main;
				 bOverlappingCombatSphere = true;
				 Attack();
			 }
		 }
		 
	 }
 }


 void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
 {
	 if (OtherActor)
	 {
		 AMain* Main = Cast<AMain>(OtherActor);
		 if (Main)
		 {
			 if (Main-> CombatTarget == this)
			 {
				 Main->SetCombatTarget(nullptr);
			 }
			 bOverlappingCombatSphere = false;
			 if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
			 {
				 MoveToTarget(Main);
				 CombatTarget = nullptr;
			 }
			 //when player moves away from the enemy - reset the timer
			 GetWorldTimerManager().ClearTimer(AttackTimer);
		 }
	 }
 }

 //Move the enemy towards the character when come under the colliding sphere
 //first set the target
 //then move toward the target

//If not able to recognise AIController - add AIMOdule under Pawn.Build.cs file
 //Implementing enemy move to character using this function
// virtual FPathFollowingRequestResult MoveTo
// (
//	 const FAIMoveRequest& MoveRequest,
//	 FNavPathSharedPtr* OutPath
// )
// 
 void AEnemy::MoveToTarget(class AMain* Target)
 {
	 SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	 
	 if (AIController)
	 {
		// UE_LOG(LogTemp, Warning, TEXT("MoveToTarget"));
		 FAIMoveRequest MoveRequest;
		 MoveRequest.SetGoalActor(Target);

		 //distance btw the capsules collider of the enemy and the player
		 MoveRequest.SetAcceptanceRadius(10.f);

		 FNavPathSharedPtr NavPath;

		 AIController->MoveTo(MoveRequest, &NavPath);

		 //FNavPAth is a pointer here

		 //Two way to implement the following - it's an array
		// TArray<FNavPathPoint> PathPoints =  NavPath->GetPathPoints();

		//-------- Uncomment this function to see the path between enemy and the character
		 /**
		 auto PathPoints = NavPath->GetPathPoints();

		 for (auto Point : PathPoints)
		 {
			 FVector Location = Point.Location;

			 //Drawing debug spheres to see the path btw enemy and the character
			 UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
		 }
		 */
	 }

 }

 void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
 {
	 if (OtherActor)
	 {
		 AMain* Main = Cast<AMain>(OtherActor);
		 if (Main)
		 {
			 if (Main->HitParticles)
			 {
				 const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				 if (TipSocket)
				 {
					 //positon/location where the blood spurt spawing on the sword - setting the socket here
					 FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					 UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				 }

			 }
			 if (Main->HitSound)
			 {
				 UGameplayStatics::PlaySound2D(this, Main->HitSound);
			 }
			 if (DamageTypeClass)
			 {
				 //ApplyDamage ends up in the damaged actor which is Main here having his TakeDamage function called
				 UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			 }
		 }
	 }
 }


 void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
 {

 }

 void AEnemy::ActivateCollsion()
 {
	 CombatCollisionEnemy->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	 if (SwingSound)
	 {
		 UGameplayStatics::PlaySound2D(this, SwingSound);
	 }
 }


 void AEnemy::DeactivateCollsion()
 {
	 CombatCollisionEnemy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 }

 //Play the attack function - which plays the enemy monatge where you have the attack animations setup
 //play swing sound when enemey claw is swinged
 void AEnemy::Attack()
 {
	 if (AIController)
	 {
		 AIController->StopMovement();
		 SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	 }

	 if (!bAttacking)
	 {
		 //play attacking animatioon
		 bAttacking = true;
		 UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		 if (AnimInstance)
		 {
			 AnimInstance->Montage_Play(CombatMontage, 1.35f);
			 AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage); 
		 }
	 }
 }

 void AEnemy::AttackEnd()
 {
	 bAttacking = false;
	 if (bOverlappingCombatSphere)
	 {
		 //setting a timer to wait between attacks by the enemy
		 float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		 GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	 }
 }

 float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
 {
	 if (Health - DamageAmount <= 0.f)
	 {
		 Health -= DamageAmount;
		 Die();
	 }
	 else
	 {
		 Health -= DamageAmount;
	 }

	 return DamageAmount;
 }

 void AEnemy::Die()
 {
	 UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	 if (AnimInstance)
	 {
		 AnimInstance->Montage_Play(CombatMontage, 1.35f);
		 AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	 }

	 SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	 CombatCollisionEnemy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	 AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	 CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	 GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 }



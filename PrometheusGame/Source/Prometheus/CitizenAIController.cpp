// Fill out your copyright notice in the Description page of Project Settings.

#include "Prometheus.h"
#include "CitizenAIController.h"
#include "CupcakePickup.h"
#include "PrometheusCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionSystem.h"

#include "Engine.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ACitizenAIController::ACitizenAIController()
{
	// Set this controller to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehavourComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerceptionComp->ConfigureSense(*SightConfig);

	/* Match with the AI/ZombieBlackboard */
	PatrolLocationKeyName = "PatrolLocation";
	TargetFoodKeyName = "TargetFood";
	CurrentWaypointKeyName = "CurrentWaypoint";
	TargetDestinationKeyName = "TargetDestination";
	BotTypeKeyName = "BotType";
	FoodRangeCheckName = "FoodRangeCheck";
	CurrentPositionName = "CurrentPosition";
	SelfReferenceName = "SelfReference";

	/* Initializes PlayerState so we can assign a team index to AI */
	bWantsPlayerState = true;

	SetSelfReference();
	SetFoodRangeCheck(false);
}

void ACitizenAIController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);

	ACitizenAICharacter* CitiActor = Cast<ACitizenAICharacter>(InPawn);
	if (CitiActor)
	{
		SightConfig->SightRadius = CitiActor->SightRangeValue;
		SightConfig->PeripheralVisionAngleDegrees = CitiActor->PeripheralRangeValue;
		AIPerceptionComp->OnPerceptionUpdated.AddDynamic(this, &ACitizenAIController::SenseStuff);
		AIPerceptionComp->ConfigureSense(*SightConfig);

		if (CitiActor->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*CitiActor->BehaviorTree->BlackboardAsset);

			/* Make sure the Blackboard has the type of bot we possessed */
			SetBlackboardBotType(CitiActor->BotType);
		}

		BehaviorComp->StartTree(*CitiActor->BehaviorTree);
	}
}

void ACitizenAIController::UnPossess()
{
	Super::UnPossess();

	/* Stop any behavior running as we no longer have a pawn to control */
	BehaviorComp->StopTree();
}

// Called every frame
void ACitizenAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACitizenAIController::SenseStuff(TArray<AActor*> testActors)
{
	if (testActors[0])
	{
		if (testActors[0]->IsA(ACupcakePickup::StaticClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, "I see a cupcake!");
			ACupcakePickup* SensedCupcake = Cast<ACupcakePickup>(testActors[0]);
			if (SensedCupcake)
			{
				if (GetFoodTarget() == nullptr)
				{
					SetFoodTarget(SensedCupcake);
					SetTargetDestination(SensedCupcake);
				}
			}
		}
		else if (testActors[0]->IsA(APrometheusCharacter::StaticClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "I see a person!");
		}
		else
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "I see something!");
	}
}

void ACitizenAIController::FoodRangeCheck()
{
	float AIToFoodDist;

	if (GetFoodTarget() != false)
	{
		AIToFoodDist = FVector::Dist(GetCurrentPosition(), GetTargetDestination());

		if (AIToFoodDist <= 200.0f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Food is in range");
			SetFoodRangeCheck(true);
			ACitizenAICharacter* CitiActor = Cast<ACitizenAICharacter>(GetPawn());
			CitiActor->CollectFood(GetFoodTarget());
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Food is out of range");
			SetFoodRangeCheck(false);
		}
	}
}

void ACitizenAIController::EatFood(AActor* InPawn, ACupcakePickup* FoodTarget)
{
	ACitizenAICharacter* CitiActor = Cast<ACitizenAICharacter>(InPawn);
	//ACupcakePickup* SensedCupcake = Cast<ACupcakePickup>(FoodTarget);
	
	CitiActor->CollectFood(FoodTarget);
}

ACitizenTargetPoint* ACitizenAIController::GetWaypoint()
{
	if (BlackboardComp)
	{
		return Cast<ACitizenTargetPoint>(BlackboardComp->GetValueAsObject(CurrentWaypointKeyName));
	}

	return nullptr;
}

ACupcakePickup* ACitizenAIController::GetFoodTarget()
{
	if (BlackboardComp)
	{
		return Cast<ACupcakePickup>(BlackboardComp->GetValueAsObject(TargetFoodKeyName));
	}

	return nullptr;
}

FVector ACitizenAIController::GetTargetDestination()
{
	return BlackboardComp->GetValueAsVector(TargetDestinationKeyName);
}

ACitizenAIController* ACitizenAIController::GetSelfReference()
{
	if (BlackboardComp)
	{
		return Cast<ACitizenAIController>(BlackboardComp->GetValueAsObject(SelfReferenceName));
	}

	return nullptr;
}

FVector ACitizenAIController::GetCurrentPosition()
{
	SetCurrentPosition();
	return BlackboardComp->GetValueAsVector(CurrentPositionName);
}

void ACitizenAIController::SetCitizenTargetPoint(ACitizenTargetPoint * NewTargetPoint)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(CurrentWaypointKeyName, NewTargetPoint);
	}
}

void ACitizenAIController::SetFoodTarget(AActor* NewTarget)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TargetFoodKeyName, NewTarget);
	}
}

void ACitizenAIController::SetTargetDestination(AActor* NewTargetDestination)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector(TargetDestinationKeyName, NewTargetDestination->GetActorLocation());
	}
}

void ACitizenAIController::SetCurrentPosition()
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector(CurrentPositionName, GetPawn()->GetActorLocation());
	}
}

void ACitizenAIController::SetBlackboardBotType(EBotBehaviorType NewType)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsEnum(BotTypeKeyName, (uint8)NewType);
	}
}

void ACitizenAIController::SetFoodRangeCheck(bool bIsFoodInRange)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(FoodRangeCheckName, bIsFoodInRange);
	}
}

void ACitizenAIController::SetSelfReference()
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(SelfReferenceName, this);
	}
}

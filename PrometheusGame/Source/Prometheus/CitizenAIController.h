// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "CitizenAICharacter.h"
#include "CitizenTargetPoint.h"
#include "CupcakePickup.h"
#include "CitizenAIController.generated.h"

UCLASS()
class PROMETHEUS_API ACitizenAIController : public AAIController
{
	GENERATED_BODY()

	//Called whenever the controller possesses a character bot
	virtual void Possess(class APawn* InPawn) override;

	virtual void UnPossess() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UBehaviorTreeComponent* BehaviorComp;

	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetFoodKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName PatrolLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName CurrentWaypointKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetDestinationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName CurrentPositionName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName BotTypeKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName FoodRangeCheckName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName SelfReferenceName;

	/*
	UPROPERTY(BlueprintCallable, Category = "AI")
	void SearchForFood(class AActor* FoodObj);

	void SetFoodObject(class AActor* FoodObj);
	*/

	public:
	ACitizenAIController();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class UAIPerceptionSystem* AIPerceptionSystemComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class UAIPerceptionComponent* AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sight")
	class UAISenseConfig_Sight* SightConfig;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SenseStuff(TArray<AActor*> testActors);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void EatFood(AActor* InPawn, ACupcakePickup* FoodTarget);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void FoodRangeCheck();

	ACitizenTargetPoint* GetWaypoint();

	UFUNCTION(BlueprintCallable, Category = "AI")
	ACupcakePickup* GetFoodTarget();

	FVector GetTargetDestination();

	FVector GetCurrentPosition();

	ACitizenAIController* GetSelfReference();

	void SetCitizenTargetPoint(ACitizenTargetPoint* NewTargetPoint);

	void SetFoodTarget(AActor* NewTarget);

	void SetTargetDestination(AActor* NewTargetDestination);
	
	void SetBlackboardBotType(EBotBehaviorType NewType);

	void SetFoodRangeCheck(bool bIsFoodInRange);

	void SetCurrentPosition();

	void SetSelfReference();
	
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
};

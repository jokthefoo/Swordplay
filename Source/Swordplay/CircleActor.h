// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CircleActor.generated.h"

class ASwordplayCharacter;

UCLASS()
class SWORDPLAY_API ACircleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACircleActor();
	void PassCharRef(ASwordplayCharacter* charRef, bool bgmode);
	void HighlightSelection(int sel);
	void MakeSelection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		ASwordplayCharacter* charRef;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;     

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Selection")
		void HighlightFunc(int sel);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Selection")
		void SelectionMade();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Created")
		void CircleCreated();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool BoyGirlMode;
	
	UFUNCTION(BlueprintCallable, Category = "Selection")
		void CircleEnd();

};

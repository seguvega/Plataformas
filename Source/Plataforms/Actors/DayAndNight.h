// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayAndNight.generated.h"

class ADirectionalLight;

UCLASS()
class PLATAFORMS_API ADayAndNight : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADayAndNight();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	///VARIABLES
	UPROPERTY(EditAnywhere, Category = "Sky")
	ADirectionalLight* Sun;

	UPROPERTY(EditAnywhere, Category = "Sky")
	AActor* SkySphere;

	FRotator Rotacion;

	UPROPERTY(EditAnywhere, Category = "Sky")
	float Velocidad;
};

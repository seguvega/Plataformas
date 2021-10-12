// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleporterZ.generated.h"

class UBoxComponent;

UCLASS()
class PLATAFORMS_API ATeleporterZ : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleporterZ();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extra Actors", meta = (AlllowPrivateAccess = "true"))
	TArray<AActor*> PlayerStarts;///Utilizar -->  TSoftObjectPtr<AActor> Reference; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Box;

	///Validar
	bool bIsValidArray;
	
	///DELEGATE Functions Box->OnComponentBeginOverlap();
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	//Delegate 	Box->OnComponentEndOverlap();
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

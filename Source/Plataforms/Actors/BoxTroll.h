#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxTroll.generated.h"

class UBoxComponent;
class USoundBase;

UCLASS()
class PLATAFORMS_API ABoxTroll : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoxTroll();

	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* BoxVolumeTroll;
	
	//Sonido
	UPROPERTY(EditAnywhere, Category = "Audio Troll")
	USoundBase* TrollSound;
	
	UWorld* ThisWorld;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

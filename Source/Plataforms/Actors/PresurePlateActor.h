#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PresurePlateActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;
class AActor;
class APlataformStaticMesh;

UCLASS()
class PLATAFORMS_API APresurePlateActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APresurePlateActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	///VARIABLES
	UPROPERTY(EditAnywhere, Category = "Componentes", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerVolumen;

	UPROPERTY(EditAnywhere, Category = "Componentes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Componentes", meta = (AllowPrivateAccess = "true")) //Llamo a la clase APlataformStaticMesh
	TArray<APlataformStaticMesh* > Plataformas;
	
	///FUNCIONES
	//Is a dynamic event for de UBoxComponent
	//OnComponentOverlap functions ->https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/ClassCreation/CodeOnly/
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

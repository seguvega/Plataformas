// Fill out your copyright notice in the Description page of Project Settings.


#include "PresurePlateActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Plataforms/Plataform/PlataformStaticMesh.h"

// Sets default values
APresurePlateActor::APresurePlateActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolumen = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolumen"));
	if (!TriggerVolumen) return;
	RootComponent = TriggerVolumen;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	if (!Mesh) return; //if Mesh == nullptr
	Mesh->SetupAttachment(RootComponent);
	
	//Pongo la movilidad de todo el actor con el mesh en Movable
	Mesh->SetMobility(EComponentMobility::Movable);

	//Si ejecuto AddDynamic en el Constructor tengo q reiniciar el motor
	TriggerVolumen->OnComponentBeginOverlap.AddDynamic(this, &APresurePlateActor::OnOverlapBegin);//Función dinámica a OnOverlapBegin
	TriggerVolumen->OnComponentEndOverlap.AddDynamic(this, &APresurePlateActor::OnOverlapEnd);//Función dinámica a OnOverlapEnd
}

// Called when the game starts or when spawned
void APresurePlateActor::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void APresurePlateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APresurePlateActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,  AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
		FVector Location = GetActorLocation(); //No utilizar directamente el Component = Mesh->GetComponentLocation();
		Location -= FVector(0, 0, 10);
		SetActorLocation(Location);
		for (auto& Plataform : Plataformas)
		{
			Plataform->SetActivatePlataform();
		}
	}
}

void APresurePlateActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,  AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority()) 
	{
		//UE_LOG(LogTemp, Warning, TEXT("Overlap End"));
		FVector Location = GetActorLocation();//World space
		Location += FVector(0, 0, 10);
		//UE_LOG(LogTemp, Warning, TEXT("Location f %s"), *Location.ToString());
		SetActorLocation(Location);
		for (auto& Plataform : Plataformas)
		{
			Plataform->SetDeactivatePlataform();
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "PresurePlateActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Plataforms/Plataform/PlataformStaticMesh.h"
#include "Plataforms/PlataformsCharacter.h"

// Sets default values ///OJO Hay q validar todos los PUNTEROS
APresurePlateActor::APresurePlateActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;//Desactivo el tick

	TriggerVolumen = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolumen"));
	if (!TriggerVolumen)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBoxComponent is null"));
		return;
	}
	RootComponent = TriggerVolumen;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("UStaticMeshComponent is null"));
		return;
	}
	Mesh->SetupAttachment(RootComponent);
	
	//Pongo la movilidad de todo el actor con el mesh en Movable
	Mesh->SetMobility(EComponentMobility::Movable);

	//Si ejecuto AddDynamic en el Constructor tengo q reiniciar el motor
	TriggerVolumen->OnComponentBeginOverlap.AddDynamic(this, &APresurePlateActor::OnOverlapBegin);//Función dinámica a OnOverlapBegin
	TriggerVolumen->OnComponentEndOverlap.AddDynamic(this, &APresurePlateActor::OnOverlapEnd);//Función dinámica a OnOverlapEnd
	
	///REPLICATION
	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void APresurePlateActor::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		Location = Mesh->GetRelativeLocation();//Inicializo Relative space
	}
}

// Called every frame
void APresurePlateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/// Si el server pisa la consola del server impimre Aplastado server pero la consola del cliente imprime Aplastado cliente
//y si el cliete pisa la consola del server imprime Aplastado server pero la consola del cliente imprime aplastando client
//Es decir se ejecuta en el servidor y en el cliente al mismo tiempo
void APresurePlateActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,  AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlataformsCharacter* Character = Cast<APlataformsCharacter>(OtherActor);
	//UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
	if (HasAuthority())
	{
		//El código dentro del if no se ejecuta en el cliente
		Location -= FVector(0, 0, 10);
		if (Character)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Aplastado server nombre: %s"), *Character->GetOwner()->GetName());
		}
		for (auto& Plataform : Plataformas)
		{
			if (!Plataform) return;//Si es null Retorno
			Plataform->SetActivatePlataform();
		}
		Mesh->SetRelativeLocation(Location);//Muevo el actor
	}
}

void APresurePlateActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,  AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("Overlap End"));
	
	if (HasAuthority()) 
	{
		Location += FVector(0, 0, 10);
		for (auto& Plataform : Plataformas)
		{
			if (!Plataform) return;
			Plataform->SetDeactivatePlataform();///el servidor mueve a la plataforma
		}
		Mesh->SetRelativeLocation(Location);//Muevo el actor
	}
}

void APresurePlateActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APresurePlateActor, Location);
}

void APresurePlateActor::OnLocationChange()
{
	//Muevo el actor en el cliente cuando detecto cambios, ya que esta funcion se llama depues de q la variable Location se Replico en el cliente
	Mesh->SetRelativeLocation(Location);
}


///Se llama desde el Cliente pero se ejecuta en el Server
void APresurePlateActor::MovimientoPlacaServer_Implementation(FVector Mov)
{
	///Importante -> No se ejecutan las funciones si este actor no esta owned by the playercontroller 
}

void APresurePlateActor::MovimientoPlacaCliente_Implementation(FVector Mov)
{
	///Importante -> No se ejecutan las funciones si este actor no esta owned by the playercontroller 
}
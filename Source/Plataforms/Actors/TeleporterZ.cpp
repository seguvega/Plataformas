// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleporterZ.h"
#include "Components/BoxComponent.h"
#include "Plataforms/PlataformsCharacter.h"

// Sets default values
ATeleporterZ::ATeleporterZ()
{
	PrimaryActorTick.bCanEverTick = false;//No necesito el Tick
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Volume"));
	if (!Box) return;
	RootComponent = Box;

	Box->OnComponentBeginOverlap.AddDynamic(this, &ATeleporterZ::OnOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ATeleporterZ::OnEndOverlap);

	bReplicates = true;
	bAlwaysRelevant = true;

	//Para Validar el TArray PlayerStarts 
	bIsValidArray = false;
}

// Called when the game starts or when spawned
void ATeleporterZ::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerStarts.Num() > 0) 
	{
		for (auto PlayerStart : PlayerStarts)
		{
			if (!PlayerStart)
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerStarts contiene -> None"));
				bIsValidArray = false;
				return;//Sale de toda la Function -> Con break sale del loop
			}
		}
		bIsValidArray = true;
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Array Vacío"));
		bIsValidArray = false;
	}
}

///IMPORTATE 
/*Este código del OnOverlap se ejecuta tanto en el servidor como en cliente
eso quiere decir q cuando el actor APlataformsCharacter overlap a TeleporterZ este se ejecuta en el server y en cliente
porq este actor esta replicado -> bReplicates y bAlwaysRelevant ademas cuando esta acción se ejecute el servidor ejecutará el codigo
de ATeleporterZ::OnOverlap y simultaneamente el cliente hará lo mismo ya que el codigo ATeleporterZ::OnOverlap tambien esta en el cliente
es decir el Objeto -> BP_TeleporterZ existe en el server y en el cliente
Esta función se llama 2 veces una por el server y otra por el cliente pero corre en diferentes pc lo cual da la relacion 1=1 (Todo lo del cliente tiene q estar en el server)
entre el Server y el Cliente*/
void ATeleporterZ::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlataformsCharacter* TeleporterActor = Cast<APlataformsCharacter>(OtherActor);
	if (TeleporterActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Teletransportado -> %s"), *TeleporterActor->GetName());
		if (bIsValidArray)
		{
			for (auto PlayerStart : PlayerStarts)
			{
				if (TeleporterActor->Respawn(PlayerStart->GetActorLocation()))
				{
					return;
				}
				else
				{
					continue;
				}
			}
			
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("Cargue datos en el TArray<AActor*>"));
		}
		
	}
}

void ATeleporterZ::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
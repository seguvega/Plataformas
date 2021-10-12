// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxTroll.h"
#include "Components/BoxComponent.h"
#include "Plataforms/PlataformsCharacter.h"
#include "Kismet/GameplayStatics.h"//Para reproducir el audio

// Sets default values
ABoxTroll::ABoxTroll()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxVolumeTroll = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Volume"));
	RootComponent = BoxVolumeTroll;

	bReplicates = true;

	BoxVolumeTroll->OnComponentBeginOverlap.AddDynamic(this, &ABoxTroll::OnBoxOverlap);
	BoxVolumeTroll->OnComponentEndOverlap.AddDynamic(this, &ABoxTroll::OnBoxEndOverlap);
}

// Called when the game starts or when spawned
void ABoxTroll::BeginPlay()
{
	Super::BeginPlay();
	if (!TrollSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cargue un SoundCue"));
	}
	ThisWorld = GetWorld();
}

void ABoxTroll::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlataformsCharacter* TrolledCharacter = Cast<APlataformsCharacter>(OtherActor);
	if (TrolledCharacter && TrollSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(ThisWorld, TrollSound, TrolledCharacter->GetActorLocation());
	}
}

void ABoxTroll::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
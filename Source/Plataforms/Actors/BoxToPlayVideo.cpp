// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxToPlayVideo.h"
#include "Components/BoxComponent.h"
#include "Plataforms/PlataformsCharacter.h"
#include "UObject/ConstructorHelpers.h"//Para encontrar archivos
#include "MediaPlayer.h"//UMediaPlayer Importante ----> Añado el modulo "MediaAssets" en .Build.cs
#include "MediaSource.h"//UMediaSource
#include "MediaSoundComponent.h"//UMediaSoundComponent

// Sets default values
ABoxToPlayVideo::ABoxToPlayVideo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UMediaPlayer> MPVideo(TEXT("/Game/Movies/MP_Pluma"));
	static ConstructorHelpers::FObjectFinder<UMediaSource> MSource(TEXT("/Game/Movies/MS_Pluma"));
	if (MPVideo.Object && MSource.Object)
	{
		MediaPlayer = MPVideo.Object;
		MediaSource = MSource.Object;
		//UE_LOG(LogTemp, Warning, TEXT("Encontrado: %s"), *MediaPlayer->GetName());
	}

	BoxVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Volume"));
	if (!BoxVolume) return;
	RootComponent = BoxVolume;

	MediaSound = CreateDefaultSubobject<UMediaSoundComponent>(TEXT("MediaSound Component"));
	if (!MediaSound) return;
	MediaSound->SetupAttachment(RootComponent);

	BoxVolume->OnComponentBeginOverlap.AddDynamic(this, &ABoxToPlayVideo::OnOverlapBox);

	bReplicates = true;
	bAlwaysRelevant = true;
	MediaSound->SetIsReplicated(true);//Porcia xD
}

// Called when the game starts or when spawned
void ABoxToPlayVideo::BeginPlay()
{
	Super::BeginPlay();
	if (MediaPlayer && MediaSound)
	{
		MediaPlayer->SetLooping(false);//Pongo en false el loop
		MediaPlayer->PlayOnOpen = true;//Que se reproduzca el video cuando se abre en OpenSource()

		//Para escuchar el audio añadir MediaSoundComponent
		//MediaSound->SetDefaultMediaPlayer(MediaPlayer);//Da error cuando se cook
		MediaSound->SetMediaPlayer(MediaPlayer);//Pongo el audio 
	}
}

void ABoxToPlayVideo::OnOverlapBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlataformsCharacter* WinnerActor = Cast<APlataformsCharacter>(OtherActor);
	if (WinnerActor && MediaPlayer && MediaSource)
	{
		if (MediaPlayer->OpenSource(MediaSource))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Reproduciendo.."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error :)"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Cargado MediaPlayer ni MediaSource o MediaSound"));
	}
}
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxToPlayVideo.generated.h"

class UBoxComponent;
class UMediaPlayer;
class UMediaSoundComponent;
class UMediaSource;

UCLASS()
class PLATAFORMS_API ABoxToPlayVideo : public AActor
{
	GENERATED_BODY()
	
public:	

	ABoxToPlayVideo();

	//Para Rreproducir el video
	UMediaPlayer* MediaPlayer;

	//El video
	UMediaSource* MediaSource;

	//ElAudio del video
	UPROPERTY(EditAnywhere, Category = "Components")
	UMediaSoundComponent* MediaSound;

protected:
	UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxVolume;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

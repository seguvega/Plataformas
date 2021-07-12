#include "GameInstancePlataforms.h"
#include "Engine/Engine.h"

UGameInstancePlataforms::UGameInstancePlataforms(const FObjectInitializer& ObjectInitializer)
{
	UE_LOG(LogTemp, Error, TEXT("Hello GameInstance Constructor!!!"));
}

void UGameInstancePlataforms::Init()
{
	UE_LOG(LogTemp, Error, TEXT("Hellow GameInstance Init ??"));
}

void UGameInstancePlataforms::Host()
{
	UE_LOG(LogTemp, Error, TEXT("Hellow HOST"));
	UEngine* Engine = GetEngine();
	if (!Engine) return;
	Engine->AddOnScreenDebugMessage(0, 10, FColor::Green, TEXT("Hosting"));
	
}

void UGameInstancePlataforms::Join(FString& IP)
{
	UEngine* Engine = GetEngine();
	if (!Engine) return;
	Engine->AddOnScreenDebugMessage(0, 10, FColor::Green, FString::Printf(TEXT("Joining: %s"), *IP));
}

void UGameInstancePlataforms::chat(FString msg)
{
	UEngine* Engine = GetEngine();
	if (!Engine) return;
	Engine->AddOnScreenDebugMessage(0, 10, FColor::Green, FString::Printf(TEXT("Desconocido: %s"), *msg) );
}
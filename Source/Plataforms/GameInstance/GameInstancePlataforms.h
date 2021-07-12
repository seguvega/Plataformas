#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstancePlataforms.generated.h"


UCLASS()
class PLATAFORMS_API UGameInstancePlataforms : public UGameInstance
{
	GENERATED_BODY()

public:

	UGameInstancePlataforms(const FObjectInitializer& ObjectInitializer);//El constructor de GameInstance tiene un parametro por default

	virtual void Init() override;

private:
	UFUNCTION(Exec) //Exec Only in PlayerController, GameInstance, Possessed Pawns, HUDs, GameModes
	void Host();

	UFUNCTION(Exec)
	void Join(FString& IP);

	UFUNCTION(Exec)
	void chat(FString msg);
};

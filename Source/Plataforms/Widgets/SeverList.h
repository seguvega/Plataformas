// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SeverList.generated.h"

class UTextBlock;
class UButton;
class UMainMenuWidget;

//Creo una STRUC el prefijo para las Struct es -> F
USTRUCT()
struct FServerData 
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString UserName;
	uint32 Ping;
};


UCLASS()
class PLATAFORMS_API USeverList : public UUserWidget
{
	GENERATED_BODY()

public:
	bool bIsSelected = false;

	void SetText(FServerData Struct);

	void SetUp(UMainMenuWidget* WidgetMainMenu, uint32 Index);

	void SetSelected(bool IsBtnServerIdSelected);

	uint32 MyIndex;

protected:
	virtual bool Initialize() override;


private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	UButton* BtnServerId;

	UMainMenuWidget* MyMainMenu;

	//Por el Bind ->AddDynamic()
	UFUNCTION()
	void OnClickServerId();

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;//Es un Tick pero para UUserWidget
};
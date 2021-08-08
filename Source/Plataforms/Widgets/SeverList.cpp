// Fill out your copyright notice in the Description page of Project Settings.


#include "SeverList.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Plataforms/Widgets/MainMenuWidget.h"//Agrego la clase MainMenuWidget
#include "Styling/SlateColor.h"///Necesito agregar el modulo "SlateCore", para llamar a la clase FSlateColor

bool USeverList::Initialize()
{
	bool Resp = Super::Initialize();
	if (!Resp) return false;

	if (!BtnServerId) return false;
	BtnServerId->OnClicked.AddDynamic(this, &USeverList::OnClickServerId);
	return Resp;
}

void USeverList::SetText(FServerData Struct)
{
	//Omito el Struct.Name
	FText Texto = FText::FromString("Host: "+ Struct.UserName + "     Jugadores: "+FString::Printf(TEXT("%d/%d"), Struct.CurrentPlayers, Struct.MaxPlayers) + "     Ping: " + FString::Printf(TEXT("%d"), Struct.Ping));
	ServerName->SetText(Texto);
}

void USeverList::SetUp(UMainMenuWidget* WidgetMainMenu, uint32 Index)
{
	//Cargo las variables globales WidgeMainMenu y Index respectivas de cada objeto
	MyMainMenu = WidgetMainMenu;
	MyIndex = Index;
}

void USeverList::OnClickServerId()
{
	if (!MyMainMenu) return;
	MyMainMenu->SetIndex(MyIndex);
}

void USeverList::SetSelected(bool IsBtnServerIdSelected)
{
	bIsSelected = IsBtnServerIdSelected;
}

void USeverList::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	if (bIsSelected)
	{
		//Para utilizar FslateColor necesito el modulo "SlateCore" en el archivo .Build.cs
		FSlateColor myColor =  FSlateColor(FLinearColor(0, 0.29167, 0.5)); //Es el 2 constructor FSlateColor( const FLinearColor& InColor ) InColor = (R,G,B,A) los 4 son float el 4to es opcional porq por default va 1.0f
		///0.29167 es el resultado de dividir 75 para 255
		ServerName->SetColorAndOpacity(myColor);
	}
	else 
	{
		FSlateColor OtherColor = FSlateColor(FLinearColor::White);//Blanco
		if (ServerName->ColorAndOpacity == OtherColor) return;
		ServerName->SetColorAndOpacity(OtherColor);
	}
}
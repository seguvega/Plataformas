// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Plataforms/Widgets/SeverList.h"

bool UMainMenuWidget::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return Success;

	if (!Btn_Host) return false;
	Btn_Host->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickHost); //Necesitan una UFUNCION()

	if (!Btn_Join) return false;
	Btn_Join->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickJoin);

	if (!Btn_JoinGame) return false;
	Btn_JoinGame->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinMenu);

	if (!Btn_Atras) return false;
	Btn_Atras->OnClicked.AddDynamic(this, &UMainMenuWidget::ReturnMenu);

	if (!BtnExit) return false;
	BtnExit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClickExit);

	return Success;
}

void UMainMenuWidget::OnClickHost()
{
	if (!MenuInterface) return;
	MenuInterface->Host();
}

void UMainMenuWidget::OnClickJoin()
{
	if (!MenuWidgetSwitcher) return;
	if (!JoinMenu || !MenuInterface) return;
	MenuWidgetSwitcher->SetActiveWidget(JoinMenu);
	MenuInterface->BuscarServerList();//Llamo al metodo del GameInstancePlataforms
	SBServersList->ClearChildren();//Limpio el ScrollBox
}

void UMainMenuWidget::SetIndex(uint32 Index)
{
	//Agrego un index al TOptional //Almacena un null por defecto
	SelectedIndex = Index;
	TArray<UWidget*>Botones = SBServersList->GetAllChildren();
	for (auto Boton : Botones)
	{
		USeverList* MyBoton = Cast<USeverList>(Boton);///Cast de un objeto padre a uno hijo Padre(UWidget*) Hijo(USeverList*)
		if (!MyBoton) return;
		uint32 IndexBtn = MyBoton->MyIndex;//Consigo el index del USeverList -> Boton
		//UE_LOG(LogTemp, Warning, TEXT("Index Seleccionado-> %d | otro index -> %d"), Index, IndexBtn);//%d es Int
		if (Index == IndexBtn)
		{
			MyBoton->SetSelected(true);
		}
		else 
		{
			MyBoton->SetSelected(false);
		}
	}
}

void UMainMenuWidget::OnJoinMenu()
{
	if (SelectedIndex.IsSet())//Compruebo si se seteo un valor porq por default el TOptional<> es null
	{
		if (!MenuInterface) return;
		UE_LOG(LogTemp, Warning, TEXT("Index Seleccionado-> %d"), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Index Seleccionado-> Not Set"));
	}
	//Ctrl + k y Ctrl + c para comentar
	//if (!TextBoxIP) return;
	//if (!MenuInterface) return;
	//FString IpAddres = TextBoxIP->GetText().ToString();
	//UE_LOG(LogTemp, Warning, TEXT("Join Ip: %s"), *IpAddres);
	//MenuInterface->Join("");
}

void UMainMenuWidget::AgregarServidores(USeverList* List)
{
	if (!MenuInterface) return;
	if (List)
	{
		SBServersList->AddChild(List);
	}
}

void UMainMenuWidget::ReturnMenu()
{
	if (!MenuWidgetSwitcher) return;
	if (!MainMenu) return;
	MenuWidgetSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenuWidget::OnClickExit()
{
	if (!MenuInterface) return;
	MenuInterface->Exit();
}
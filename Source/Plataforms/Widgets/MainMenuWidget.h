#pragma once

#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
#include "Plataforms/Widgets/FatherWidget.h"//Cargo a la clase padre
#include "MainMenuWidget.generated.h"

class UButton;
class UWidgetSwitcher;
class UEditableTextBox;
class UFatherWidget;//Clase Padre Widget
class UPanelWidget;//ScrollBox
class USeverList;//Widget Lista

UCLASS()
class PLATAFORMS_API UMainMenuWidget : public UFatherWidget
{
	GENERATED_BODY()
	
public:
	///VARIABLES
	//Para hacer un bind al boton con el nombre de la propiedad del button
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Host;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Join;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_JoinGame;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Atras;

	UPROPERTY(meta = (BindWidget))
	UButton* BtnExit;

	//SubMenuss
	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* JoinMenu;

	//EditableTextBox
	//UPROPERTY(meta = (BindWidget))
	//UEditableTextBox* TextBoxIP;
	//Scroll Box -> PanelWidget
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* SBServersList;

	//WidgetSwitcher
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuWidgetSwitcher;

	///FUNCIONES
	//Añade a los UServerList creados en GameInstancePlataforms
	void AgregarServidores(USeverList* List);

	void SetIndex(uint32 Index);

protected:
	virtual bool Initialize() override;

private:
	///VARIABLES
	//Para guardar el id del array de UseverList seleccionado y si no selecciona nada se guarda un null
	TOptional<uint32> SelectedIndex;

	///FUNCIONES
	//Funciones del resultado de hacer el Bind = AddDynamic()
	UFUNCTION()
	void OnClickHost();

	UFUNCTION()
	void OnClickJoin();

	UFUNCTION()
	void OnJoinMenu();

	UFUNCTION()
	void ReturnMenu();

	UFUNCTION()
	void OnClickExit();
};

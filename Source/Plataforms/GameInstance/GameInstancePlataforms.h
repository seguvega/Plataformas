#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Plataforms/Widgets/Interfaces/MenuInterface.h"//Cargo a mi Interface 
#include "OnlineSubsystem.h"//Llamo al modulo OnlineSubsystem
#include "Interfaces/OnlineSessionInterface.h"//Llamo IOnlineSessionInterface para el EOnJoinSessionCompleteResult
#include "GameInstancePlataforms.generated.h"

class UUserWidget;//UMG modules
class IMenuInterface;//Interface
class UMainMenuWidget;//Menu Widget
class UInGameMenu;//InGameMenu Widget
class USeverList;//SeverList Widget
class FOnlineSessionSearch;//BuscarSessions

UCLASS()
									   //El  :  significa q hereda  todo lo q este a la derecha de : puede heredar 1 clase padre y 1 o mas interfaces
class PLATAFORMS_API UGameInstancePlataforms : public UGameInstance,  public IMenuInterface
{
	GENERATED_BODY()

public:

	UGameInstancePlataforms(const FObjectInitializer& ObjectInitializer);//El constructor de GameInstance tiene un parametro por default

	virtual void Init(); //No necesito el override ya que esa funcion no es del padre UGameInstance peor de la Interface IMenuInterface
	
	//FUNCION
	//void StartSession();//Sirver para marcar a la creada en el CreateSession() como llena, la llamo desde otra clase -> ALobbyPlataformsGameMode

private:
	///VARIABLES
	///UUserWidgets
	//Utilizo el TSubclassOf cuando no se la clase especifica -- OjO Sirve para llamar a la Clase directamente no a un Objeto
	TSubclassOf<UUserWidget> MenuWidget;

	TSubclassOf<UUserWidget> InGameMenuWidget;

	//Class ServerList
	TSubclassOf<UUserWidget> ServerList;

	//Puntero de la clase UMainMenuWidget
	UMainMenuWidget* Menu;

	//Puntero para Quitar el UInGameMenu
	UInGameMenu* InGameMenu;
	
	///OnlineSubsystem y OnlineSessionInterface
	//IOnlineSession
	//--------- Esta variable no necesitan llamar a su class pero se referencia aqui en el header->"OnlineSubsystem.h"
	IOnlineSessionPtr SessionPtr;

	//FindSession
	TSharedPtr<FOnlineSessionSearch> SessionSearch; //Crea un puntero de tipo Shared Pointers es decir es nullable (No da error si no esta inicializado)

	///Defino Variables de Steam o Null Subsystem
	uint32 PlayerId;//default es = 0
	
	FName PlayerName;

	TSharedPtr<const FUniqueNetId> NetPlayerId;//Es un Shared Pointers -> https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/SmartPointerLibrary/SharedPointer/

	///Extras Variables
	TOptional<uint32> UserSelectedIndex;//Es null por defecto

	///FUNCIONES
	///Funciones PURE -- Declaradas en la Interface -> IMenuUnterface
	UFUNCTION(Exec) //Exec Only in PlayerController, GameInstance, Possessed Pawns, HUDs, GameModes
	void Host() override;//Declaro la funcion pure de IMenuInterface

	UFUNCTION(Exec)
	void Join(uint32 Index) override;//Tambien es Pure

	void GoToMainMenu() override;//No necesita el UFUNCTION()

	void Exit() override;//override porq es un metodo del padre 

	void BuscarServerList() override;

	///BlueprintCallable Para llamarlas desde el Blueprint haciendo un cast a este objeto GameInstancePlataforms
	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	UFUNCTION(BlueprintCallable)
	void LoadInGameMenu();

	UFUNCTION(BlueprintCallable)
	void ResumeInGameMenu();

	///Sessions --- Metodos BIND se ejecutan tras acabar una accion 
	//AddUObject is like AddDynamic and all Bind functions may have UFUNCTION
	UFUNCTION()
	void OnSessionResp(FName SessionName, bool Resp);

	UFUNCTION()
	void OnEndSessionResp(FName SessionName, bool Resp);


	UFUNCTION()
	void OnFindSession(bool Resp);

	//Aca no va la UFUNCTION() ya que no se esta especificando la clase del ENumn -> EOnJoinSessionCompleteResult
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	//OnNetworkFailure Bind o delegates 
	void BroadcastNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString = TEXT(""));

	///Extras Funciones 
	void CreateSession();

	UFUNCTION(Exec)
	void chat(FString msg);
};
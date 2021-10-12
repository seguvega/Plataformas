#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"// Para los RPC y DOREPLIFETIME
#include "PresurePlateActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;
class AActor;
class APlataformStaticMesh;

UCLASS()
class PLATAFORMS_API APresurePlateActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APresurePlateActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	///VARIABLES
	UPROPERTY(ReplicatedUsing=OnLocationChange)
	FVector Location;

	UPROPERTY(EditAnywhere, Category = "Componentes", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerVolumen;

	UPROPERTY(EditAnywhere, Category = "Componentes", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Componentes", meta = (AllowPrivateAccess = "true")) //Llamo a la clase APlataformStaticMesh
	TArray<APlataformStaticMesh* > Plataformas;
	
	///FUNCIONES
	//Is a dynamic event for de UBoxComponent
	//OnComponentOverlap functions ->https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/ClassCreation/CodeOnly/
	//OnOverlapBegin OnOverlapEnd estas funciones por defecto son se ejecutan en el server y en el cliente
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	///Replicated Var Location
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
	//ReplicatedUsing function declaration estas funcion solo se ejecuta en el cliente
	//Tiene q ser UFUNCTION()
	UFUNCTION()
	void OnLocationChange();

public:
	///RPC(Remote Procedure Calls)  -> #include "Net/UnrealNetwork.h" en .h 
	///Importante -> No se ejecutan las funciones si este actor no esta owned by the playercontroller 
	///UFUNCTION(Client) ---> Se  llama desde el servidor y se ejecuta el código implementado en esa función en el Cliente
	///UFUNCTION(Server) ---> Se  llama desde el cliente y se ejecuta el código implementado en esa función en el Servidor
	///UFUNCTION(NetMulticast) --> Se llama desde el servidor y el codigo se ejecuta EVERYWHERE --EnTodosLados
	///Reliable --> asegura el envio de la info
	///Unreliable --> F 
	///Para definir en el .h y .cpp, van el nombre de la funcion + _Implementation ->MovimientoPlacaServer_Implementation
	///la funcion MovimientoPlacaServer NO Se implementa pero con esa se llama al MovimientoPlacaServer_Implementation()
	UFUNCTION(Server, Reliable)
	void MovimientoPlacaServer(FVector Mov);
	virtual void MovimientoPlacaServer_Implementation(FVector Mov);

	UFUNCTION(Client, Reliable)
	void MovimientoPlacaCliente(FVector Mov);
	virtual void MovimientoPlacaCliente_Implementation(FVector Mov);
};
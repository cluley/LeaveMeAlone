// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "LevelActors/Pickup/LMAHealthPickup.h"
#include "Components/SphereComponent.h"
#include "Player/LMADefaultCharacter.h"
#include "Components/LMAHealthComponent.h"

// Sets default values
ALMAHealthPickup::ALMAHealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    SetRootComponent(SphereComponent);
}

// Called when the game starts or when spawned
void ALMAHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
    PickupWasTaken();
}

// Called every frame
void ALMAHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALMAHealthPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    const auto Charcter = Cast<ALMADefaultCharacter>(OtherActor);
    if (GivePickup(Charcter))
    {
        PickupWasTaken();
    }
}

bool ALMAHealthPickup::GivePickup(ALMADefaultCharacter* Character)
{
    if (IsValid(Character))
    {
        const auto HealthComponent = Character->GetHealthComponent();
        if (!IsValid(HealthComponent)) return false;
        return HealthComponent->AddHealth(HealthFromPickup);
    }

    return false;
}

void ALMAHealthPickup::PickupWasTaken()
{
    SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    GetRootComponent()->SetVisibility(false, true);
    FTimerHandle RespawnTimerHandle;
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ALMAHealthPickup::RespawnPickup, RespawnTime);
}

void ALMAHealthPickup::RespawnPickup()
{
    SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    GetRootComponent()->SetVisibility(true, true);
    FTimerHandle DisposeTimerHandle;
    GetWorldTimerManager().SetTimer(DisposeTimerHandle, this, &ALMAHealthPickup::PickupWasTaken, BeingDuration);
}
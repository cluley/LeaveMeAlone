// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->SetUsingAbsoluteRotation(true);
    SpringArmComponent->TargetArmLength = ArmLength;
    SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
    SpringArmComponent->bDoCollisionTest = false;
    SpringArmComponent->bEnableCameraLag = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent);
    CameraComponent->SetFieldOfView(FOV);
    CameraComponent->bUsePawnControlRotation = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    if (IsValid(CursorMaterial))
    {
        CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
    }

    HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
    OnHealthChanged(HealthComponent->GetHealth());
    HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (!(HealthComponent->IsDead()))
    {
        RotationPlayerOnCursor();
    }
}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);
    PlayerInputComponent->BindAxis("CameraZoom", this, &ALMADefaultCharacter::CameraZoom);
}

void ALMADefaultCharacter::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::CameraZoom(float Value)
{
    if (Value > 0)
    {
        ArmLength -= 100;
    }
    else if (Value < 0)
    {
        ArmLength += 100;
    }
    ArmLength = FMath::Clamp(ArmLength, ArmLengthMin, ArmLengthMax);
    SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::OnDeath()
{
    CurrentCursor->DestroyRenderState_Concurrent();
    PlayAnimMontage(DeathMontage);
    //GetCharacterMovement()->DisableMovement();
    SetLifeSpan(5.0f);
    if (IsValid(Controller))
    {
        Controller->ChangeState(NAME_Spectating);
    }
}

void ALMADefaultCharacter::RotationPlayerOnCursor()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (IsValid(PC))
    {
        FHitResult ResultHit;
        PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
        float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
        SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
        if (IsValid(CurrentCursor))
        {
            CurrentCursor->SetWorldLocation(ResultHit.Location);
        }
    }
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}
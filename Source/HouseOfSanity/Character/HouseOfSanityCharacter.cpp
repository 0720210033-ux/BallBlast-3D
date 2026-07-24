#include "Character/HouseOfSanityCharacter.h"
#include "Components/SanityComponent.h"
#include "Components/InteractionComponent.h"
#include "Components/SiblingAppearanceComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"

AHouseOfSanityCharacter::AHouseOfSanityCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head"));
	FirstPersonCamera->bUsePawnControlRotation = true;

	SanityComponent = CreateDefaultSubobject<USanityComponent>(TEXT("SanityComponent"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	AppearanceComponent = CreateDefaultSubobject<USiblingAppearanceComponent>(TEXT("AppearanceComponent"));

	DisguiseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DisguiseMesh"));
	DisguiseMesh->SetupAttachment(GetMesh());
	DisguiseMesh->SetVisibility(false);
	DisguiseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHouseOfSanityCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHouseOfSanityCharacter, SiblingIndex);
}

void AHouseOfSanityCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AHouseOfSanityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHouseOfSanityCharacter::HandleMove);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHouseOfSanityCharacter::HandleLook);
		}
		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AHouseOfSanityCharacter::HandleInteract);
		}
	}
}

void AHouseOfSanityCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!Controller)
	{
		return;
	}

	const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), MoveInput.Y);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), MoveInput.X);
}

void AHouseOfSanityCharacter::HandleLook(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AHouseOfSanityCharacter::HandleInteract(const FInputActionValue& Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}
}

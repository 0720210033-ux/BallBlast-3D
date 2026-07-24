#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HouseOfSanityCharacter.generated.h"

class UCameraComponent;
class USanityComponent;
class UInteractionComponent;
class USiblingAppearanceComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class HOUSEOFSANITY_API AHouseOfSanityCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHouseOfSanityCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "House of Sanity")
	TObjectPtr<USanityComponent> SanityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "House of Sanity")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "House of Sanity")
	TObjectPtr<USiblingAppearanceComponent> AppearanceComponent;

	// Hidden by default; USiblingAppearanceComponent shows this instead of
	// the real mesh, per-viewer, once that viewer's own sanity is low enough.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "House of Sanity")
	TObjectPtr<USkeletalMeshComponent> DisguiseMesh;

	// Cosmetic identity only (which sibling this is / which skin to use) -
	// assign per PlayerStart or via GameMode on spawn.
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "House of Sanity")
	int32 SiblingIndex = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleInteract(const FInputActionValue& Value);
};

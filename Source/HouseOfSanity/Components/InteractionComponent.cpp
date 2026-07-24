#include "Components/InteractionComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

FVector UInteractionComponent::GetTraceStart() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->GetController())
	{
		FVector ViewLocation;
		FRotator ViewRotation;
		OwnerPawn->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);
		return ViewLocation;
	}
	return GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
}

FVector UInteractionComponent::GetTraceDirection() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->GetController())
	{
		FVector ViewLocation;
		FRotator ViewRotation;
		OwnerPawn->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);
		return ViewRotation.Vector();
	}
	return GetOwner() ? GetOwner()->GetActorForwardVector() : FVector::ForwardVector;
}

AActor* UInteractionComponent::FindFocusedInteractable() const
{
	const FVector Start = GetTraceStart();
	const FVector End = Start + GetTraceDirection() * TraceDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld() && GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params))
	{
		if (Hit.GetActor() && Hit.GetActor()->Implements<UInteractableInterface>())
		{
			return Hit.GetActor();
		}
	}
	return nullptr;
}

void UInteractionComponent::TryInteract()
{
	AActor* Focused = FindFocusedInteractable();
	if (!Focused)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	IInteractableInterface::Execute_Interact(Focused, OwnerPawn);
}

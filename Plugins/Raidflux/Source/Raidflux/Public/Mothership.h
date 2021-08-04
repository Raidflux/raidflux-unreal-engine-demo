// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mothership.generated.h"

USTRUCT(BlueprintType)
struct RAIDFLUX_API FMothership
{
	GENERATED_BODY()
public:

	FMothership():
		id(""),
		distanceToUser(0.0),
		name(""),
		latitude(0.0),
		longitude(0.0) 
	{}

	FMothership(FString id, float distanceToUser, FString name, float latitude, float longitude):
		id(id),
		distanceToUser(distanceToUser),
		name(name),
		latitude(latitude),
		longitude(longitude)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	FString id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	float distanceToUser;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	FString name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	float latitude;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	float longitude;
};

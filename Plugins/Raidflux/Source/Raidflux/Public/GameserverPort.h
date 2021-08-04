// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameserverPort.generated.h"

USTRUCT(BlueprintType)
struct RAIDFLUX_API FGameserverPort
{
	GENERATED_BODY()
public:

	FGameserverPort() :
		port(0),
		internalPort(0),
		protocol("")
	{}

	FGameserverPort(int port, int internalPort, FString protocol) :
		port(port),
		internalPort(internalPort),
		protocol(protocol)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	int port;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	int internalPort;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	FString protocol;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "GameserverPort.h"
#include "Gameserver.generated.h"


USTRUCT(BlueprintType)
struct RAIDFLUX_API FGameserver
{
	GENERATED_BODY()
public:
	FGameserver() :
		id(0),
		playerCount(0),
		maxPlayerCount(0),
		ipv4(""),
		ipv6(""),
		ports(TArray<FGameserverPort>()),
		hostname("")
	{}

	FGameserver(int id, int playerCount, int maxPlayerCount, FString ipv4, FString ipv6, TArray<FGameserverPort> ports, FString hostname) :
		id(id),
		playerCount(playerCount),
		maxPlayerCount(maxPlayerCount),
		ipv4(ipv4),
		ipv6(ipv6),
		ports(ports),
		hostname(hostname)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	int id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	int playerCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	int maxPlayerCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	FString ipv4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	FString ipv6;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	TArray<FGameserverPort> ports;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raidflux")
	FString hostname;
};

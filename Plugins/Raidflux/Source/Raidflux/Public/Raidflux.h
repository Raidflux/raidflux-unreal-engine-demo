// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Raidflux.generated.h"

struct FMothership;
struct FGameserver;

DECLARE_DYNAMIC_DELEGATE(FOnCompleted);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMothershipsFetched, const TArray<FMothership>&, Motherships);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGameserversFetched, const TArray<FGameserver>&, Gameservers);

DECLARE_LOG_CATEGORY_EXTERN(LogRaidflux, Display, All);

class IHttpRequest;

UCLASS(Blueprintable)
class RAIDFLUX_API URaidflux : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, DisplayName = "Initialize Raidflux", Category = "Raidflux")
	void Init(int maxPlayers, FOnCompleted onCompletedCallback);

	UFUNCTION(BlueprintCallable, DisplayName = "Report player count to Raidflux", Category = "Raidflux")
	void ReportPlayerCount(int currentPlayers, int maxPlayers, FOnCompleted onCompletedCallback);

	UFUNCTION(BlueprintCallable, DisplayName = "Fetch motherships from deployment", Category = "Raidflux")
	void FetchMotherships(const FString& deploymentID, FOnMothershipsFetched onCompletedCallback);

	UFUNCTION(BlueprintCallable, DisplayName = "Fetch gameservers from mothership", Category = "Raidflux")
	void FetchGameservers(const FString& mothershipID, FOnGameserversFetched onCompletedCallback);

	UFUNCTION(BlueprintCallable, DisplayName = "Get Raidflux Instance", Category = "Raidflux")
	static URaidflux* GetInstance(UGameInstance* gameInstance);
private:

	static const FString REGISTER_API;
	static const FString REPORT_API;
	static const FString MATCHMAKER_HOST;
	static const FString MATCHMAKER_MOTHERSHIPS;
	static const FString MATCHMAKER_GAMESERVERS;

	static FString GetSDKHost();
	static FString GetSeatID();

	static FString MakeSDKUrl(const FString& path);
	static FString MakeMatchmakerUrl(const FString& path, const FString& id);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> MakeRequest(const FString& path, const FString& verb, TSharedPtr<FJsonObject> jsonObject = {});

	bool initialized = false;
	int lastCurrentPlayers = 0;
	int lastMaxPlayers = 0;
	uint64 gameserverId = 0;
};

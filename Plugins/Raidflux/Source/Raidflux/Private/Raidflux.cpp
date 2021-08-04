// Copyright Epic Games, Inc. All Rights Reserved.

#include "Raidflux.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Mothership.h"
#include "Gameserver.h"
#include "GameserverPort.h"

DEFINE_LOG_CATEGORY(LogRaidflux);

const FString URaidflux::REGISTER_API = TEXT("/rfsdk/gameserver/register");
const FString URaidflux::REPORT_API = TEXT("/rfsdk/gameserver/report");
const FString URaidflux::MATCHMAKER_HOST = TEXT("https://matchmaker.raidflux.com");
const FString URaidflux::MATCHMAKER_MOTHERSHIPS = TEXT("/discovery/deployment/{0}/motherships");
const FString URaidflux::MATCHMAKER_GAMESERVERS = TEXT("/discovery/mothership/{0}/gameservers");

void URaidflux::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URaidflux::Deinitialize()
{
	Super::Deinitialize();
}

void URaidflux::Init(int maxPlayers, FOnCompleted onCompletedCallback)
{
	if(this->initialized) {
		onCompletedCallback.ExecuteIfBound();
		return;
	}

	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
	jsonObject->SetStringField(TEXT("seat_id"), this->GetSeatID());
	jsonObject->SetNumberField(TEXT("max_player_count"), maxPlayers);

	FString registerUrl = URaidflux::MakeSDKUrl(URaidflux::REGISTER_API);
	auto request = this->MakeRequest(registerUrl, TEXT("POST"), jsonObject);

	request->OnProcessRequestComplete().BindLambda([&, onCompletedCallback](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful) {
		if (bWasSuccessful && response->GetContentType().Equals(TEXT("application/json"), ESearchCase::IgnoreCase))
		{
			TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
			TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response->GetContentAsString());

			FJsonSerializer::Deserialize(jsonReader, jsonObject, FJsonSerializer::EFlags::StoreNumbersAsStrings);

			auto gameserverIdField = jsonObject->TryGetField("gameserver_id");
			if (gameserverIdField.Get() != nullptr) {
				gameserverIdField.Get()->TryGetNumber(this->gameserverId);
			}
			
			UE_LOG(LogRaidflux, Display, TEXT("Raidflux GameserverID %llu \n"), this->gameserverId);
		}
		else {
			UE_LOG(LogRaidflux, Display, TEXT("Raidflux initialized in local mode, http error can be ignored"));
		}

		onCompletedCallback.ExecuteIfBound();
	});
	request->ProcessRequest();

	this->initialized = true;
}

void URaidflux::ReportPlayerCount(int currentPlayers, int maxPlayers, FOnCompleted onCompletedCallback)
{
	if (!this->initialized || (this->lastCurrentPlayers == currentPlayers && this->lastMaxPlayers == maxPlayers) || this->GetSeatID().Equals(TEXT("null"))) {
		onCompletedCallback.ExecuteIfBound();
		return;
	}

	if (currentPlayers < 0)
	{
		currentPlayers = 0;
	}

	if (maxPlayers < 0)
	{
		maxPlayers = 0;
	}

	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());

	auto gameserverValue = MakeShared<FJsonValueNumberString>(FString::Printf(TEXT("%llu"), this->gameserverId));
	jsonObject->SetField(TEXT("gameserver_id"), gameserverValue);
	jsonObject->SetNumberField(TEXT("player_count"), currentPlayers);
	jsonObject->SetNumberField(TEXT("max_player_count"), maxPlayers);

	FString reportUrl = URaidflux::MakeSDKUrl(URaidflux::REPORT_API);
	auto request = this->MakeRequest(reportUrl, TEXT("POST"), jsonObject);

	request->OnProcessRequestComplete().BindLambda([onCompletedCallback](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful) {
		onCompletedCallback.ExecuteIfBound();
		UE_LOG(LogRaidflux, Display, TEXT("Raidflux playercount reported"));
	});
	request->ProcessRequest();
}

void URaidflux::FetchMotherships(const FString& deploymentID, FOnMothershipsFetched onCompletedCallback) {
	FString registerUrl = URaidflux::MakeMatchmakerUrl(URaidflux::MATCHMAKER_MOTHERSHIPS, deploymentID);
	auto request = this->MakeRequest(registerUrl, TEXT("GET"));
	request->OnProcessRequestComplete().BindLambda([onCompletedCallback](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful) {
		TArray<FMothership> motherships = TArray<FMothership>();

		if (bWasSuccessful && response->GetContentType().Equals(TEXT("application/json"), ESearchCase::IgnoreCase))
		{
			TSharedPtr<FJsonValue> jsonParsed;
			TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response->GetContentAsString());

			FJsonSerializer::Deserialize(jsonReader, jsonParsed, FJsonSerializer::EFlags::StoreNumbersAsStrings);

			if (jsonParsed.IsValid() && jsonParsed->Type == EJson::Array) {
				auto jsonList = jsonParsed->AsArray();
				for (int32 i = 0; i < jsonList.Num(); i++) {
					auto mothershipJson = jsonList[i]->AsObject();
					auto regionJson = mothershipJson->GetObjectField("vm")->GetObjectField("zone")->GetObjectField("region");

					motherships.Add(FMothership(
						mothershipJson->GetStringField("id"),
						mothershipJson->GetNumberField("distance_to_user"),
						regionJson->GetStringField("name"),
						regionJson->GetNumberField("latitude"),
						regionJson->GetNumberField("longitude")
					));
				}
			}
		}

		onCompletedCallback.ExecuteIfBound(motherships);
	});
	request->ProcessRequest();
}

void URaidflux::FetchGameservers(const FString& mothershipID, FOnGameserversFetched onCompletedCallback) {
	FString registerUrl = URaidflux::MakeMatchmakerUrl(URaidflux::MATCHMAKER_GAMESERVERS, mothershipID);
	auto request = this->MakeRequest(registerUrl, TEXT("GET"));
	request->OnProcessRequestComplete().BindLambda([onCompletedCallback](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful) {
		TArray<FGameserver> gameservers = TArray<FGameserver>();

		if (bWasSuccessful && response->GetContentType().Equals(TEXT("application/json"), ESearchCase::IgnoreCase))
		{
			TSharedPtr<FJsonValue> jsonParsed;
			TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(response->GetContentAsString());

			FJsonSerializer::Deserialize(jsonReader, jsonParsed, FJsonSerializer::EFlags::StoreNumbersAsStrings);

			if (jsonParsed.IsValid() && jsonParsed->Type == EJson::Array) {
				auto jsonList = jsonParsed->AsArray();
				for (int32 i = 0; i < jsonList.Num(); i++) {
					auto gameserverJson = jsonList[i]->AsObject();

					TArray<FGameserverPort> gameserverPorts = TArray<FGameserverPort>();

					auto portsList = gameserverJson->GetArrayField("ports");
					for (int32 j = 0; j < portsList.Num(); j++) {
						auto gameserverPortJson = portsList[j]->AsObject();
						gameserverPorts.Add(FGameserverPort(
							gameserverPortJson->GetIntegerField("port"),
							gameserverPortJson->GetIntegerField("internal_port"),
							gameserverPortJson->GetStringField("protocol")
						));
					}

					FString ipv4 = "";
					FString ipv6 = "";
					FString hostname = "";

					gameserverJson->GetObjectField("fleetship_instance")->TryGetStringField("ipv4", ipv4);
					gameserverJson->GetObjectField("fleetship_instance")->TryGetStringField("ipv6", ipv6);
					gameserverJson->TryGetStringField("hostname", hostname);

					gameservers.Add(FGameserver(
						gameserverJson->GetIntegerField("id"),
						gameserverJson->GetIntegerField("current_player_count"),
						gameserverJson->GetIntegerField("max_player_count"),
						ipv4,
						ipv6,
						gameserverPorts,
						hostname
					));
				}
			}
		}

		onCompletedCallback.ExecuteIfBound(gameservers);
	});

	request->ProcessRequest();
}



URaidflux* URaidflux::GetInstance(UGameInstance* gameInstance)
{
	return gameInstance->GetSubsystem<URaidflux>();
}

FString URaidflux::GetSDKHost()
{

#if PLATFORM_LINUX
	FString sdkHost = FLinuxPlatformMisc::GetEnvironmentVariable(TEXT("RAIDFLUX_SDK_HOST"));
#elif PLATFORM_WINDOWS
	FString sdkHost = FWindowsPlatformMisc::GetEnvironmentVariable(TEXT("RAIDFLUX_SDK_HOST"));
#else
	FString sdkHost = FGenericPlatformMisc::GetEnvironmentVariable(TEXT("RAIDFLUX_SDK_HOST"));
#endif

	if (sdkHost.IsEmpty()) {
		sdkHost = TEXT("localhost:8010");
	}

	return sdkHost;
}

FString URaidflux::GetSeatID()
{
#if PLATFORM_LINUX
	FString seatID = FLinuxPlatformMisc::GetEnvironmentVariable(TEXT("RAIDFLUX_SEAT_ID"));
#elif PLATFORM_WINDOWS
	FString seatID = FWindowsPlatformMisc::GetEnvironmentVariable(TEXT("RAIDFLUX_SEAT_ID"));
#else
	FString seatID = FGenericPlatformMisc::GetEnvironmentVariable(TEXT("RAIDFLUX_SEAT_ID"));
#endif

	if (seatID.IsEmpty()) {
		seatID = TEXT("null");
	}

	return seatID;
}

FString URaidflux::MakeSDKUrl(const FString& path)
{
	return FString(TEXT("http://"))
		.Append(URaidflux::GetSDKHost())
		.Append(path);
}

FString URaidflux::MakeMatchmakerUrl(const FString& path, const FString& id)
{
	TArray< FStringFormatArg > args;
	args.Add(FStringFormatArg(id));
	FString formatted = FString::Format(*path, args);

	return  URaidflux::MATCHMAKER_HOST + formatted;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> URaidflux::MakeRequest(const FString& path, const FString& verb, TSharedPtr<FJsonObject> jsonObject)
{
	FHttpModule& http = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = http.CreateRequest();

	if (jsonObject.IsValid()) {
		FString outputString;
		TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<>::Create(&outputString);
		FJsonSerializer::Serialize(jsonObject.ToSharedRef(), jsonWriter);

		request->SetContentAsString(outputString);
		request->SetHeader("Content-Type", "application/json");

		UE_LOG(LogRaidflux, Display, TEXT("JSON request created %s"), *outputString);
	}

	request->SetVerb(verb);
	request->SetURL(path);

	return request;
}

#pragma once

#include "CoreMinimal.h"
#include "NegotiationResponse.generated.h"

USTRUCT()
struct FNegotiationTransport
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString Transport;

	UPROPERTY()
	TArray<FString> TransferFormats;
};

USTRUCT()
struct FNegotiationResponse
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int32 NegotiateVersion;

	UPROPERTY()
	FString ConnectionId;

	UPROPERTY()
	FString ConnectionToken;

	UPROPERTY()
	FString Url;

	UPROPERTY()
	FString AccessToken;

	UPROPERTY()
	FString Error;

	UPROPERTY()
	TArray<FNegotiationTransport> AvailableTransports;
};

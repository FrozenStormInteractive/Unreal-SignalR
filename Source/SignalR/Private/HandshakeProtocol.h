// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IHubProtocol;
class FJsonObject;

class SIGNALR_API FHandshakeProtocol
{
public:
    static FString CreateHandshakeMessage(TSharedPtr<IHubProtocol> InProtocol);
    static TTuple<TSharedPtr<FJsonObject>, FString> ParseHandshakeResponse(const FString& Response);
};

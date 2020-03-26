// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SignalRValue.h"

class SIGNALR_API IHubProtocol
{
public:
    virtual ~IHubProtocol();

    virtual FName Name() const = 0;
    virtual int Version() const = 0;

    virtual FString ConvertMessage(TSharedPtr<FSignalRValue>) const = 0;
    virtual TArray<TSharedPtr<FSignalRValue>> ParseMessages(const FString&) const = 0;
};

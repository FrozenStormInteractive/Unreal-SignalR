// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IHubProtocol.h"

class SIGNALR_API FJsonHubProtocol : public IHubProtocol
{
public:
    static constexpr TCHAR RecordSeparator = TEXT('\x1e');

    virtual FName Name() const override;
    virtual int Version() const override;

    virtual FString ConvertMessage(TSharedPtr<FSignalRValue>) const override;
    virtual TArray<TSharedPtr<FSignalRValue>> ParseMessages(const FString&) const override;

protected:

};

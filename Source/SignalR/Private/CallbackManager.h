// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IHubConnection.h"

class SIGNALR_API FCallbackManager
{
public:
    FCallbackManager();
    ~FCallbackManager();

    TTuple<FName, FOnMethodCompletion&> RegisterCallback();
    bool InvokeCallback(FName InCallbackId, TSharedPtr<FSignalRValue> InArguments, bool InRemoveCallback);
    bool RemoveCallback(FName InCallbackId);
    void Clear(TSharedPtr<FSignalRValue> InArguments);

private:
    FName GenerateCallbackId();

    TMap<FName, FOnMethodCompletion> Callbacks;
    FCriticalSection CallbacksLock;

    TAtomic<int> CurrentId;
};

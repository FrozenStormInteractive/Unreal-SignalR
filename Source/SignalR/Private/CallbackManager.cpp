// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CallbackManager.h"
#include "Misc/ScopeLock.h"

FCallbackManager::FCallbackManager()
{
}

FCallbackManager::~FCallbackManager()
{
    Clear(nullptr);
}

TTuple<FName, FOnMethodCompletion&> FCallbackManager::RegisterCallback()
{
    FName Id = GenerateCallbackId();

    FScopeLock Lock(&CallbacksLock);
    FOnMethodCompletion& qssq = Callbacks.Add(Id);

    return TTuple<FName, FOnMethodCompletion&>(Id, qssq);
}

bool FCallbackManager::InvokeCallback(FName InCallbackId, TSharedPtr<FSignalRValue> InArguments, bool InRemoveCallback)
{
    FOnMethodCompletion Callback;

    {
        FScopeLock Lock(&CallbacksLock);

        if(!Callbacks.Contains(InCallbackId))
        {
            return false;
        }

        Callback = Callbacks[InCallbackId];

        if (InRemoveCallback)
        {
            Callbacks.Remove(InCallbackId);
        }
    }

    Callback.ExecuteIfBound(InArguments);
    return true;
}

bool FCallbackManager::RemoveCallback(FName InCallbackId)
{
    {
        FScopeLock Lock(&CallbacksLock);
        return Callbacks.Remove(InCallbackId) != 0;
    }
}

void FCallbackManager::Clear(TSharedPtr<FSignalRValue> InArguments)
{
    {
        FScopeLock Lock(&CallbacksLock);

        for (auto& El : Callbacks)
        {
            El.Value.ExecuteIfBound(InArguments);
        }

        Callbacks.Empty();
    }
}

FName FCallbackManager::GenerateCallbackId()
{
    const auto CallbackId = CurrentId++;
    return *FString::FromInt(CallbackId);
}

// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SignalRModule.h"

#include "HubConnection.h"
#include "Modules/ModuleManager.h"
#include "IHubConnection.h"
#include "WebSocketsModule.h"

IMPLEMENT_MODULE(FSignalRModule, SignalR);

DEFINE_LOG_CATEGORY(LogSignalR);

FSignalRModule* FSignalRModule::Singleton = nullptr;

FSignalRModule& FSignalRModule::Get()
{
    if (nullptr == Singleton)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FSignalRModule>("SignalR");
    }
    check(Singleton);
    return *Singleton;
}

TSharedPtr<IHubConnection> FSignalRModule::CreateHubConnection(const FString& InUrl, const TMap<FString, FString>& InHeaders)
{
    check(bInitialized);

    return MakeShared<FHubConnection>(InUrl, InHeaders);
}

void FSignalRModule::StartupModule()
{
    Singleton = this;
    bInitialized = true;
    FModuleManager::LoadModuleChecked<FWebSocketsModule>("WebSockets");
}

void FSignalRModule::ShutdownModule()
{
    bInitialized = false;
}

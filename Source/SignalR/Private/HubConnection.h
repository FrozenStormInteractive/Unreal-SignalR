// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CallbackManager.h"
#include "CoreMinimal.h"
#include "IHubConnection.h"
#include "IHubProtocol.h"
#include "Tickable.h"

class FConnection;

class SIGNALR_API FHubConnection : public IHubConnection, FTickableGameObject
{
public:
    static const constexpr float PingTimer = 10.0f;

    FHubConnection(const FString& InUrl, const TMap<FString, FString>& InHeaders);
    virtual ~FHubConnection();

    virtual FOnMethodInvocation& On(FName EventName) override;

    virtual FOnMethodCompletion& Invoke(FName EventName, TSharedPtr<FSignalRValue> InArguments = nullptr) override;

    virtual void Send(FName InEventName, TSharedPtr<FSignalRValue> InArguments = nullptr) override;

    virtual void Tick(float DeltaTime) override;
    TStatId GetStatId() const override;
    virtual ETickableTickType GetTickableTickType() const override
    {
        return ETickableTickType::Always;
    }
    virtual bool IsTickable() const override
    {
        return true;
    }
    virtual bool IsTickableInEditor() const override
    {
        return true;
    }
    virtual bool IsTickableWhenPaused() const override
    {
        return true;
    }

protected:
    void ProcessMessage(const FString& InMessageStr);

private:
    void OnConnectionStarted();
    void OnConnectionClosed(int32 StatusCode, const FString& Reason, bool bWasClean);

    void Ping();
    void InvokeHubMethod(FName MethodName, TSharedPtr<FSignalRValue> InArguments, FName CallbackId);

    FString Host;

    TSharedPtr<IHubProtocol> HubProtocol;
    TSharedPtr<FConnection> Connection;
    TMap<FName, FOnMethodInvocation> InvocationHandlers;
    FCallbackManager CallbackManager;

    bool bHandshakeReceived = false;

    float TickTimeCounter = 0;
};

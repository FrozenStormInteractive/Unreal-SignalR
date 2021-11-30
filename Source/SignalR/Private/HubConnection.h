/*
 * MIT License
 *
 * Copyright (c) 2020-2021 FrozenStorm Interactive
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

    TArray<FString> WaitingCalls;
};

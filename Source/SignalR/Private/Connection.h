// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IWebSocket.h"
#include "Interfaces/IHttpRequest.h"

class SIGNALR_API FConnection
{
public:
    FConnection(const FString& InHost, const TMap<FString, FString>& InHeaders);

    void Connect();

    bool IsConnected();

    void Send(const FString& Data);

    void Close(int32 Code = 1000, const FString& Reason = FString());

    IWebSocket::FWebSocketConnectedEvent& OnConnected();

    IWebSocket::FWebSocketConnectionErrorEvent& OnConnectionError();

    IWebSocket::FWebSocketClosedEvent& OnClosed();

    IWebSocket::FWebSocketMessageEvent& OnMessage();

private:
    void Negotiate();
    void OnNegotiateResponse(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnectedSuccessfully);
    void StartWebSocket();

    TSharedPtr<IWebSocket> Connection;
    FString Host;
    TMap<FString, FString> Headers;

    IWebSocket::FWebSocketConnectedEvent OnConnectedEvent;
    IWebSocket::FWebSocketConnectionErrorEvent OnConnectionErrorEvent;
    IWebSocket::FWebSocketClosedEvent OnClosedEvent;
    IWebSocket::FWebSocketMessageEvent OnMessageEvent;

    FString ConnectionToken;
    FString ConnectionId;
};

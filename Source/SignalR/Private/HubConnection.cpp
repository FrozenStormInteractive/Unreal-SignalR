/*
 * MIT License
 *
 * Copyright (c) 2020-2022 Frozen Storm Interactive, Yoann Potinet
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

#include "HubConnection.h"
#include "JsonHubProtocol.h"
#include "SignalRModule.h"
#include "Dom/JsonObject.h"
#include "MessageType.h"
#include "Connection.h"
#include "HandshakeProtocol.h"
#include "StringUtils.h"

FHubConnection::FHubConnection(const FString& InUrl, const TMap<FString, FString>& InHeaders):
    FTickableGameObject(),
    ConnectionState(EConnectionState::Disconnected),
    Host(InUrl),
    HubProtocol(MakeShared<FJsonHubProtocol>())
{
    Connection = MakeShared<FConnection>(Host, InHeaders);

    Connection->OnConnected().AddRaw(this, &FHubConnection::OnConnectionStarted);
    Connection->OnConnectionFailed().AddRaw(this, &FHubConnection::OnConnectionFailed);
    Connection->OnMessage().AddRaw(this, &FHubConnection::ProcessMessage);
    Connection->OnConnectionError().AddRaw(this, &FHubConnection::OnConnectionError);
    Connection->OnClosed().AddRaw(this, &FHubConnection::OnConnectionClosed);
}

FHubConnection::~FHubConnection()
{
	if(Connection.IsValid() && Connection->IsConnected())
	{
	    SendCloseMessage();
        Connection->Close();
	}
}

void FHubConnection::Start()
{
    if (ConnectionState != EConnectionState::Disconnected)
    {
        UE_LOG(LogSignalR, Error, TEXT("Hub connection can only be started if it is in the disconnected state"));
        return;
    }
    ConnectionState = EConnectionState::Connecting;
    Connection->Connect();
}

void FHubConnection::Stop()
{
    if (ConnectionState == EConnectionState::Disconnected)
    {
        UE_LOG(LogSignalR, Log, TEXT("Stop ignored because the connection is already disconnected"));
        return;
    }
    SendCloseMessage();
    ConnectionState = EConnectionState::Disconnecting;
    Connection->Close();
}

IHubConnection::FOnMethodInvocation& FHubConnection::On(const FString& InEventName)
{
    static FOnMethodInvocation BadDelegate;

    if(StringUtils::IsEmptyOrWhitespace(InEventName))
    {
        UE_LOG(LogSignalR, Error, TEXT("EventName cannot be empty."));
        return BadDelegate;
    }

    if(InvocationHandlers.Contains(InEventName))
    {
        UE_LOG(LogSignalR, Error, TEXT("An action for this event has already been registered. event name: %s"), *InEventName);
        return BadDelegate;
    }

    return InvocationHandlers.Add(InEventName);
}

IHubConnection::FOnMethodCompletion& FHubConnection::Invoke(const FString& InEventName, const TArray<FSignalRValue>& InArguments)
{
    TTuple<FName, FOnMethodCompletion&> Callback = CallbackManager.RegisterCallback();
    InvokeHubMethod(InEventName, InArguments, Callback.Key);
    return Callback.Value;
}

void FHubConnection::Send(const FString& InEventName, const TArray<FSignalRValue>& InArguments)
{
    InvokeHubMethod(InEventName, InArguments, NAME_None);
}

void FHubConnection::Tick(float DeltaTime)
{
    TickTimeCounter += DeltaTime;
	if(TickTimeCounter > PingTimer)
	{
        Ping();
        TickTimeCounter = 0;
	}
}

TStatId FHubConnection::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(FHubConnection, STATGROUP_Tickables);
}

void FHubConnection::ProcessMessage(const FString& InMessageStr)
{
    FString MessageStr = InMessageStr;

	if(!bHandshakeReceived)
	{
        auto Res = FHandshakeProtocol::ParseHandshakeResponse(MessageStr);

        const TSharedPtr<FJsonObject> HandshakeResponseObject = Res.Get<0>();

		if(HandshakeResponseObject.IsValid())
		{
		    if(HandshakeResponseObject->HasField(TEXT("error")))
		    {
                UE_LOG(LogSignalR, Error, TEXT("Handshake error: %s"), *HandshakeResponseObject->GetStringField(TEXT("error")));
                return;
		    }
            else if (HandshakeResponseObject->HasField(TEXT("type")))
            {
                UE_LOG(LogSignalR, Error, TEXT("Received unexpected message while waiting for the handshake response."));
                return;
            }
            else
            {
                bHandshakeReceived = true;
                ConnectionState = EConnectionState::Connected;
                OnHubConnectedEvent.Broadcast();

                MessageStr = Res.Get<1>();

                for (const FString& Call : WaitingCalls)
                {
                    Connection->Send(Call);
                }
            }
		}
        else
        {
            UE_LOG(LogSignalR, Error, TEXT("Bad handshake response."));
            return;
        }
	}

    auto Messages = HubProtocol->ParseMessages(MessageStr);

    for (const TSharedPtr<FHubMessage> Message : Messages)
    {
        switch (Message->MessageType)
        {
        case ESignalRMessageType::Invocation:
        {
            TSharedPtr<FInvocationMessage> InvocationMessage = StaticCastSharedPtr<FInvocationMessage>(Message);
            check(InvocationMessage != nullptr);

            const FString& MethodName = InvocationMessage->Target;
            if(InvocationHandlers.Contains(MethodName))
            {
                InvocationHandlers[MethodName].ExecuteIfBound(InvocationMessage->Arguments);
            }
            break;
        }
        case ESignalRMessageType::StreamInvocation:
            UE_LOG(LogSignalR, Warning, TEXT("Received unexpected message type 'StreamInvocation'"));
            break;
        case ESignalRMessageType::StreamItem:
            // TODO
            break;
        case ESignalRMessageType::Completion:
        {
            TSharedPtr<FCompletionMessage> CompletionMessage = StaticCastSharedPtr<FCompletionMessage>(Message);
            check(CompletionMessage != nullptr);

            if(!CompletionMessage->Error.IsEmpty())
            {
                UE_LOG(LogSignalR, Error, TEXT("%s"), *CompletionMessage->Error);
            }
            else
            {
                FName InvocationId = FName(*CompletionMessage->InvocationId);
                if (!CallbackManager.InvokeCallback(InvocationId, CompletionMessage->Result, true))
                {
                    UE_LOG(LogSignalR, Warning, TEXT("No callback found for id: %s"), *InvocationId.ToString());
                }
            }
            break;
        }
        case ESignalRMessageType::CancelInvocation:
            UE_LOG(LogSignalR, Warning, TEXT("Received unexpected message type 'CancelInvocation'"));
            break;
        case ESignalRMessageType::Ping:
            UE_LOG(LogSignalR, VeryVerbose, TEXT("Ping received"));
            break;
        case ESignalRMessageType::Close:
        {
            TSharedPtr<FCloseMessage> CloseMessage = StaticCastSharedPtr<FCloseMessage>(Message);
            check(CloseMessage != nullptr);

            if (CloseMessage->Error.IsSet())
            {
                FString CloseErrorMessage = CloseMessage->Error.GetValue();
                UE_LOG(LogSignalR, Warning, TEXT("Received close message with error: %s"), *CloseErrorMessage);
                OnHubConnectionErrorEvent.Broadcast(CloseErrorMessage);
            }

            bReceivedCloseMessage = true;
            bShouldReconnect = CloseMessage->bAllowReconnect.Get(false);

            Stop();
            break;
        }
        default:
            break;
        }
    }
}

void FHubConnection::OnConnectionStarted()
{
    UE_LOG(LogSignalR, Verbose, TEXT("Connected to %s."), *Host);

    UE_LOG(LogSignalR, Verbose, TEXT("Send handshake request"));

    bHandshakeReceived = false;

    Connection->Send(FHandshakeProtocol::CreateHandshakeMessage(HubProtocol));
}

void FHubConnection::OnConnectionFailed()
{
    UE_LOG(LogSignalR, Verbose, TEXT("Connection to %s failed."), *Host)

    OnHubConnectionErrorEvent.Broadcast(TEXT("Could not connect to host"));
}

void FHubConnection::OnConnectionError(const FString& InError)
{
    OnHubConnectionErrorEvent.Broadcast(InError);

    if (bShouldReconnect)
    {
        bShouldReconnect = false;
        UE_LOG(LogSignalR, Verbose, TEXT("Reconnecting"));
        Start();
    }
}

void FHubConnection::OnConnectionClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
    if (!bReceivedCloseMessage)
    {
        UE_LOG(LogSignalR, Warning, TEXT("The server was unexpectedly disconnected"));
    }

	if(Connection.IsValid())
	{
        CallbackManager.Clear(TEXT("Connection was stopped before invocation result was received."));
	}
    ConnectionState = EConnectionState::Disconnected;
    OnHubConnectionClosedEvent.Broadcast();

    if (bReceivedCloseMessage)
    {
        bReceivedCloseMessage = false;
        if (bShouldReconnect)
        {
            bShouldReconnect = false;
            UE_LOG(LogSignalR, Verbose, TEXT("Reconnecting"));
            Start();
        }
    }
}

void FHubConnection::Ping()
{
    if (bHandshakeReceived)
    {
        FPingMessage Ping;
        const auto Message = HubProtocol->SerializeMessage(&Ping);
        Connection->Send(Message);
        UE_LOG(LogSignalR, VeryVerbose, TEXT("Ping sent"));
    }
}

void FHubConnection::InvokeHubMethod(const FString& MethodName, const TArray<FSignalRValue>& InArguments, FName CallbackId)
{
    FString CallbackIdStr;
    if (CallbackId.IsValid() && !CallbackId.IsNone())
    {
        CallbackIdStr = CallbackId.ToString();
    }

    const FInvocationMessage Invocation(CallbackIdStr, MethodName, InArguments);

    const auto Message = HubProtocol->SerializeMessage(&Invocation);

    if (bHandshakeReceived)
    {
        Connection->Send(Message);
    }
    else
    {
        WaitingCalls.Add(Message);
    }
}

void FHubConnection::SendCloseMessage()
{
    const FCloseMessage CloseMessage;
    const auto Message = HubProtocol->SerializeMessage(&CloseMessage);
    Connection->Send(Message);
}

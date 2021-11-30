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

#include "HubConnection.h"
#include "JsonHubProtocol.h"
#include "SignalRModule.h"
#include "Dom/JsonObject.h"
#include "MessageType.h"
#include "WebSocketsModule.h"
#include "Connection.h"
#include "HandshakeProtocol.h"
#include "Serialization/JsonSerializer.h"

FHubConnection::FHubConnection(const FString& InUrl, const TMap<FString, FString>& InHeaders):
    FTickableGameObject(),
    Host(InUrl),
    HubProtocol(MakeShared<FJsonHubProtocol>())
{
    Connection = MakeShared<FConnection>(Host, InHeaders);

    Connection->OnConnected().AddRaw(this, &FHubConnection::OnConnectionStarted);
    Connection->OnMessage().AddRaw(this, &FHubConnection::ProcessMessage);
    Connection->OnClosed().AddRaw(this, &FHubConnection::OnConnectionClosed);

    Connection->Connect();
}

FHubConnection::~FHubConnection()
{
	if(Connection.IsValid() && Connection->IsConnected())
	{
        Connection->Close();
	}
}

FOnMethodInvocation& FHubConnection::On(FName InEventName)
{
    static FOnMethodInvocation BadDelegate;

    if(InEventName.IsNone())
    {
        UE_LOG(LogSignalR, Error, TEXT("EventName cannot be none."));
        return BadDelegate;
    }

    if(InvocationHandlers.Contains(InEventName))
    {
        UE_LOG(LogSignalR, Error, TEXT("An action for this event has already been registered. event name: %s"), *InEventName.ToString());
        return BadDelegate;
    }

    return InvocationHandlers.Add(InEventName);
}

FOnMethodCompletion& FHubConnection::Invoke(FName InEventName, TSharedPtr<FSignalRValue> InArguments)
{
    static FOnMethodCompletion BadDelegate;

    if(InArguments->Type != EJson::Array)
    {
        UE_LOG(LogSignalR, Error, TEXT("arguments should be an array"));
        return BadDelegate;
    }

    TTuple<FName, FOnMethodCompletion&> Callback = CallbackManager.RegisterCallback();

    InvokeHubMethod(InEventName, InArguments, Callback.Key);

    return Callback.Value;
}

void FHubConnection::Send(FName InEventName, TSharedPtr<FSignalRValue> InArguments)
{
    if (InArguments->Type != EJson::Array)
    {
        UE_LOG(LogSignalR, Error, TEXT("arguments should be an array"));
        return;
    }

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

    for (const TSharedPtr<FSignalRValue> Message : Messages)
    {
        if(Message->Type != EJson::Object)
        {
            UE_LOG(LogSignalR, Warning, TEXT("Unexpected response received from the server: %s"), *MessageStr);
            return;
        }

        const TSharedPtr<FJsonObject> Obj = Message->AsObject();

        switch (Obj->GetIntegerField(TEXT("type")))
        {
        case ESignalRMessageType::Invocation:
        {
            FName MethodName = FName(*Obj->GetStringField(TEXT("target")));

            if(InvocationHandlers.Contains(MethodName))
            {
                FOnMethodInvocation InvocationHandler = InvocationHandlers[MethodName];

                TSharedPtr<FSignalRValue> Arguments = Obj->TryGetField(TEXT("arguments"));

                if(Arguments.IsValid())
                {
                    InvocationHandler.ExecuteIfBound(Arguments);
                }
                else
                {
                    InvocationHandler.ExecuteIfBound(nullptr);
                }
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
            if(Obj->HasField(TEXT("error")))
            {
                UE_LOG(LogSignalR, Error, TEXT("%s"), *Obj->GetStringField(TEXT("error")));
            }
            else
            {
                TSharedPtr<FJsonValue> Result = Obj->TryGetField(TEXT("result"));
                if(Result == nullptr)
                {
                    UE_LOG(LogSignalR, Warning, TEXT("result is missing"));
                }
                else if(Result->Type != EJson::Object)
                {
                    UE_LOG(LogSignalR, Warning, TEXT("Expected object"));
                }
                else
                {
                    const TSharedPtr<FJsonValue> InvocationIdVal = Result->AsObject()->TryGetField(TEXT("invocationId"));
                    if (Result == nullptr)
                    {
                        UE_LOG(LogSignalR, Warning, TEXT("invocationId is missing"));
                    }
                    else if (InvocationIdVal->Type != EJson::String)
                    {
                        UE_LOG(LogSignalR, Warning, TEXT("invocationId is not a string"));
                    }
                    else
                    {
                        FName InvocationId = FName(*InvocationIdVal->AsString());
                        if (!CallbackManager.InvokeCallback(InvocationId, Result, true))
                        {
                            UE_LOG(LogSignalR, Warning, TEXT("No callback found for id: %s"), *InvocationId.ToString());
                        }
                    }
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
            break;
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

void FHubConnection::OnConnectionClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	if(Connection.IsValid())
	{
        TSharedPtr<FJsonObject> Args = MakeShared<FJsonObject>();
        Args->Values.Add(TEXT("error"), MakeShared<FJsonValueString>(TEXT("Connection was stopped before invocation result was received.")));
        CallbackManager.Clear(MakeShared<FJsonValueObject>(Args));
	}
}

void FHubConnection::Ping()
{
    if (bHandshakeReceived)
    {
        TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
        Obj->Values = {
            {
                { "type", MakeShared<FJsonValueNumber>(StaticCast<double>(ESignalRMessageType::Ping)) },
            }
        };
        const TSharedPtr<FJsonValueObject> ObjValueObj = MakeShared<FJsonValueObject>(Obj);
        const auto Message = HubProtocol->ConvertMessage(ObjValueObj);

        Connection->Send(Message);
        UE_LOG(LogSignalR, VeryVerbose, TEXT("Ping sent"));
    }
}

void FHubConnection::InvokeHubMethod(FName MethodName, TSharedPtr<FSignalRValue> InArguments, FName CallbackId)
{
    TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
    Obj->Values = {
        {
            { "type", MakeShared<FJsonValueNumber>(StaticCast<double>(ESignalRMessageType::Invocation)) },
            { "target", MakeShared<FJsonValueString>(MethodName.ToString()) },
            { "arguments", InArguments }
        }
    };

    if (CallbackId.IsValid() && !CallbackId.IsNone())
    {
        Obj->Values["invocationId"] = MakeShared<FJsonValueString>(CallbackId.ToString());
    }

    const TSharedPtr<FJsonValueObject> ObjValueObj = MakeShared<FJsonValueObject>(Obj);
    const auto Message = HubProtocol->ConvertMessage(ObjValueObj);

    if (bHandshakeReceived)
    {
        Connection->Send(Message);
    }
    else
    {
        WaitingCalls.Add(Message);
    }
}

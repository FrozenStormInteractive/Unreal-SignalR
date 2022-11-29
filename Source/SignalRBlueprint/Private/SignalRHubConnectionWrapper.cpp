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

#include "SignalRHubConnectionWrapper.h"

#include "IHubConnection.h"
#include "LatentActions.h"
#include "SignalRValueWrapper.h"

void USignalRHubConnectionWrapper::SetHubConnection(const TSharedPtr<IHubConnection>& InHubConnection)
{
    HubConnection = InHubConnection;

    if (HubConnection.IsValid())
    {
        HubConnection->OnConnected().AddUObject(this, &ThisClass::BroadcastOnHubConnected);
        HubConnection->OnConnectionError().AddUObject(this, &ThisClass::BroadcastOnHubConnectionError);
        HubConnection->OnClosed().AddUObject(this, &ThisClass::BroadcastOnHubConnectionClosed);
    }
}

void USignalRHubConnectionWrapper::Start()
{
    if (HubConnection.IsValid())
    {
        HubConnection->Start();
    }
}

void USignalRHubConnectionWrapper::Stop()
{
    if (HubConnection.IsValid())
    {
        HubConnection->Stop();
    }
}

void USignalRHubConnectionWrapper::Send(FName EventName, const TArray<FSignalRValueWrapper>& Arguments)
{
    if (HubConnection.IsValid())
    {
        TArray<FSignalRValue> NativeArguments;
        Algo::Transform(Arguments, NativeArguments, [](const FSignalRValueWrapper& ValueWrapper) { return ValueWrapper.InternalValue; });
        HubConnection->Send(EventName, NativeArguments);
    }
}

void USignalRHubConnectionWrapper::Invoke(FName EventName, const TArray<FSignalRValueWrapper>& InArguments, const FOnInvokeCompleted& OnCompleted)
{
    if (HubConnection.IsValid())
    {
        TArray<FSignalRValue> NativeArguments;
        Algo::Transform(InArguments, NativeArguments, [](const FSignalRValueWrapper& ValueWrapper) { return ValueWrapper.InternalValue; });
        HubConnection->Invoke(EventName, NativeArguments).BindUObject(this, &ThisClass::OnInvokeCompleted, OnCompleted);
    }
}

void USignalRHubConnectionWrapper::OnInvokeCompleted(const FSignalRInvokeResult& Result, FOnInvokeCompleted Delegate)
{
    Delegate.ExecuteIfBound(Result);
}

void USignalRHubConnectionWrapper::BroadcastOnHubConnected()
{
    OnHubConnected.Broadcast();
}

void USignalRHubConnectionWrapper::BroadcastOnHubConnectionError(const FString& InError)
{
    OnHubConnectionError.Broadcast(InError);
}

void USignalRHubConnectionWrapper::BroadcastOnHubConnectionClosed()
{
    OnHubConnectionClosed.Broadcast();
}

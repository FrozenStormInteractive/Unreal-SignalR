// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
#include "SignalRValue.h"

DECLARE_DELEGATE_OneParam(FOnMethodInvocation, TSharedPtr<FSignalRValue>)
DECLARE_DELEGATE_OneParam(FOnMethodCompletion, TSharedPtr<FSignalRValue>)

typedef TFunction<void(TSharedPtr<FSignalRValue>)> FMethodInvocationHandler;
typedef TFunction<void(TSharedPtr<FSignalRValue>)> FMethodInvocationCallback;

class SIGNALR_API IHubConnection : public TSharedFromThis<IHubConnection>
{
public:
    virtual FOnMethodInvocation& On(FName EventName) = 0;

    virtual FOnMethodCompletion& Invoke(FName EventName, TSharedPtr<FSignalRValue> InArguments = nullptr) = 0;

    virtual void Send(FName EventName, TSharedPtr<FSignalRValue> InArguments = nullptr) = 0;

protected:

    /**
     * Destructor
     */
    virtual ~IHubConnection();
};

typedef TSharedPtr<IHubConnection> IHubConnectionPtr;

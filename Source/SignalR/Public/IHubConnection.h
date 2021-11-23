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
